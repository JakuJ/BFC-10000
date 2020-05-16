#include "Visitors/LlvmVisitor.hpp"

#include <iostream>

#include "llvm/ADT/APFloat.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Verifier.h"

#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"

#include "llvm/ADT/Optional.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"

#include <Nodes/MoveNode.hpp>
#include <Nodes/LoopNode.hpp>
#include <Nodes/AddNode.hpp>
#include <Nodes/SetNode.hpp>

#define ZERO_EOF 1

using namespace llvm;

LLVMVisitor::LLVMVisitor() {
    // Create a Module
    module = std::make_unique<Module>("BF JIT", context);

    LLVMInitializeX86Target();

    // Create an IRBuilder
    builder = std::make_unique<IRBuilder<>>(context);

    // Create the optimizer
    fpm = std::make_unique<legacy::FunctionPassManager>(module.get());

    fpm->add(createInstructionCombiningPass());
    fpm->add(createCFGSimplificationPass());
    fpm->add(createInstructionCombiningPass());
    fpm->add(createJumpThreadingPass());
    fpm->add(createCFGSimplificationPass());
    fpm->add(createInstructionCombiningPass());
    fpm->add(createCFGSimplificationPass());
    fpm->add(createReassociatePass());
    fpm->add(createLoopRotatePass());
    fpm->add(createLICMPass());
    fpm->add(createLoopUnswitchPass(false));
    fpm->add(createInstructionCombiningPass());
    fpm->add(createIndVarSimplifyPass());
    fpm->add(createLoopDeletionPass());
    fpm->add(createLoopUnrollPass());
    fpm->add(createInstructionCombiningPass());
    fpm->add(createGVNPass());
    fpm->add(createSCCPPass());
    fpm->add(createInstructionCombiningPass());
    fpm->add(createJumpThreadingPass());
    fpm->add(createDeadStoreEliminationPass());
    fpm->add(createAggressiveDCEPass());
    fpm->add(createCFGSimplificationPass());

    fpm->doInitialization();

    // Cache LLVM declarations for write() and getchar().
    int_type = sizeof(int) == 4 ? IntegerType::getInt32Ty(context) : IntegerType::getInt64Ty(context);
    write_func = module->getOrInsertFunction("write", int_type, builder->getInt8PtrTy(), int_type);
    getchar_func = module->getOrInsertFunction("getchar", int_type);

    // Declare the "main" function of type int(void)
    FunctionType *mainType = FunctionType::get(int_type, false);
    main = Function::Create(mainType, Function::ExternalLinkage, "main", module.get());

    // Begin "main" body
    BasicBlock *BB = BasicBlock::Create(context, "entry", main);
    builder->SetInsertPoint(BB);

    // Create a named stack variable - an i64 index
    index = builder->CreateAlloca(builder->getInt64Ty(), nullptr, "index");
    builder->CreateStore(ConstantInt::get(builder->getInt64Ty(), 0), index);

    // Create a global array of i8s
    auto array_type = ArrayType::get(builder->getInt8Ty(), 65535);
    module->getOrInsertGlobal("tape", array_type);

    GlobalVariable *array = module->getNamedGlobal("tape");
    array->setAlignment(MaybeAlign(1));
    array->setInitializer(ConstantAggregateZero::get(array_type));

    tape = builder->CreateBitCast(array, builder->getInt8PtrTy());
}

void LLVMVisitor::finalize() {
    // add a return
    builder->CreateRet(ConstantInt::get(int_type, 0));

    // verify the main function
    verifyFunction(*main);
}

void LLVMVisitor::optimize() {
    fpm->run(*main);
}

Value *LLVMVisitor::getCurrentPtr() {
    // Load the current index value
    auto tmp_index = builder->CreateLoad(index, "tmp_index");

    // Return the array pointer offset by that index
    return builder->CreateInBoundsGEP(tape, tmp_index, "new_index");
}

void LLVMVisitor::visitMoveNode(MoveNode *node) {
    auto dist = ConstantInt::get(context, APInt(64, node->value, true));

    auto loaded = builder->CreateLoad(index, "tmp_index");
    auto next = builder->CreateAdd(loaded, dist, "new_index");
    builder->CreateStore(next, index);
}

void LLVMVisitor::visitAddNode(AddNode *node) {
    auto current_ptr = getCurrentPtr();

    auto current = builder->CreateLoad(current_ptr, "current");
    auto value = ConstantInt::get(context, APInt(8, static_cast<char>(node->value), true));

    auto next = builder->CreateAdd(current, value, "new");
    builder->CreateStore(next, current_ptr);
}

void LLVMVisitor::visitSetNode(SetNode *node) {
    auto current_ptr = getCurrentPtr();

    if (node->offset == 0 && node->value == 0) {
        builder->CreateStore(ConstantInt::get(builder->getInt8Ty(), 0), current_ptr);
    } else {
        auto target = builder->CreateInBoundsGEP(current_ptr, ConstantInt::get(int_type, node->offset), "set_target");

        if (node->value != 0) {
            auto current_value = builder->CreateLoad(current_ptr, "current");
            auto new_value = builder->CreateMul(current_value,
                                                ConstantInt::get(builder->getInt8Ty(), node->value, true),
                                                "mult");

            auto target_value = builder->CreateLoad(target);
            builder->CreateStore(builder->CreateAdd(target_value, new_value), target);
        }
    }
}

void LLVMVisitor::visitInputNode(InputNode *node) {
    auto read = builder->CreateCall(getchar_func);
#if ZERO_EOF
    auto scope = builder->GetInsertBlock()->getParent();

    BasicBlock *then = BasicBlock::Create(context, "eof", scope);
    BasicBlock *_else = BasicBlock::Create(context, "not_eof");
    BasicBlock *merge = BasicBlock::Create(context, "merge");

    auto cmp = builder->CreateICmpEQ(read, ConstantInt::get(int_type, -1, true), "is_eof");
    builder->CreateCondBr(cmp, then, _else);

    builder->SetInsertPoint(then);
    builder->CreateStore(ConstantInt::get(builder->getInt8Ty(), 0), getCurrentPtr());
    builder->CreateBr(merge);

    scope->getBasicBlockList().push_back(_else);
    builder->SetInsertPoint(_else);
#endif
    auto byte = builder->CreateIntCast(read, builder->getInt8Ty(), false, "byte_char");
    builder->CreateStore(byte, getCurrentPtr());
#if ZERO_EOF
    builder->CreateBr(merge);

    scope->getBasicBlockList().push_back(merge);
    builder->SetInsertPoint(merge);
#endif
}

void LLVMVisitor::visitOutputNode([[maybe_unused]] OutputNode *node) {
    auto const1 = ConstantInt::get(int_type, 1);
    std::vector<Value *> args{const1, getCurrentPtr(), const1};
    builder->CreateCall(write_func, args);
}

void LLVMVisitor::visitLoopNode(LoopNode *node) {
    auto scope = builder->GetInsertBlock()->getParent();

    BasicBlock *check = BasicBlock::Create(context, "loop_check", scope);
    BasicBlock *body = BasicBlock::Create(context, "loop_body");
    BasicBlock *end = BasicBlock::Create(context, "loop_end");;

    builder->CreateBr(check);
    builder->SetInsertPoint(check);

    auto current = builder->CreateLoad(getCurrentPtr(), "current");
    auto nonzero = builder->CreateICmpEQ(current, ConstantInt::get(builder->getInt8Ty(), 0), "nonzero");
    builder->CreateCondBr(nonzero, end, body);

    scope->getBasicBlockList().push_back(body);
    builder->SetInsertPoint(body);
    node->inside->accept(*this);
    builder->CreateBr(check);

    scope->getBasicBlockList().push_back(end);
    builder->SetInsertPoint(end);
}

void LLVMVisitor::dumpCode(const std::string &path) {
    std::error_code EC;
    raw_fd_ostream dest(path, EC, sys::fs::OF_None);

    if (EC) {
        std::cerr << "Could not open file: " << EC.message();
        return;
    }

    module->print(dest, nullptr);
}

int LLVMVisitor::compile(const std::string &path) {
    auto TargetTriple = sys::getDefaultTargetTriple();

    LLVMInitializeX86TargetInfo();
    LLVMInitializeX86TargetMC();
    LLVMInitializeX86AsmPrinter();

    std::string Error;
    auto Target = TargetRegistry::lookupTarget(TargetTriple, Error);

    if (!Target) {
        std::cerr << Error;
        return 1;
    }

    auto CPU = "x86-64";
    auto Features = "";

    TargetOptions opt;
    auto RM = Optional<Reloc::Model>();
    auto TargetMachine = Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

    module->setDataLayout(TargetMachine->createDataLayout());
    module->setTargetTriple(TargetTriple);

    std::error_code EC;
    raw_fd_ostream dest(path, EC, sys::fs::OF_None);

    if (EC) {
        std::cerr << "Could not open file: " << EC.message();
        return 1;
    }

    legacy::PassManager pass;
    auto FileType = CGFT_ObjectFile;

    if (TargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
        std::cerr << "TargetMachine can't emit a file of this type";
        return 1;
    }

    pass.run(*module);
    dest.flush();
    dest.close();

    return 0;
}
