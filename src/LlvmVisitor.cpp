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
#include "llvm/Transforms/Utils.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

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

using namespace llvm;

LLVMVisitor::LLVMVisitor() {
    // Create a Module
    module = std::make_unique<Module>("BF JIT", context);

    // Create the optimizer
    fpm = std::make_unique<legacy::FunctionPassManager>(module.get());

    fpm->add(createPromoteMemoryToRegisterPass());
    fpm->add(createInstructionCombiningPass());
    fpm->add(createReassociatePass());
    fpm->add(createGVNPass());
    fpm->add(createCFGSimplificationPass());

    fpm->doInitialization();

    // Create an IRBuilder
    builder = std::make_unique<IRBuilder<>>(context);

    // Declare the "write" function
    std::vector<Type *> write_args{builder->getInt32Ty(), builder->getInt8PtrTy(), builder->getInt32Ty()};
    Function::Create(FunctionType::get(builder->getInt32Ty(), write_args, false),
                     Function::ExternalLinkage, "write", module.get());

    // Declare the "getchar" function
    FunctionType *getcharType = FunctionType::get(builder->getInt32Ty(), {}, false);
    Function::Create(getcharType, Function::ExternalLinkage, "getchar", module.get());

    // Declare the "main" function of type void(void)
    FunctionType *mainType = FunctionType::get(builder->getInt32Ty(), false);
    main = Function::Create(mainType, Function::ExternalLinkage, "main", module.get());

    // Begin "main" body
    BasicBlock *BB = BasicBlock::Create(context, "entry", main);
    builder->SetInsertPoint(BB);

    // Create global variables
    builder->CreateGlobalString("%c", "format");

    // Create a named stack variable - an i64 index
    variables["index"] = builder->CreateAlloca(builder->getInt64Ty(), nullptr, "index");
    builder->CreateStore(ConstantInt::get(builder->getInt64Ty(), 0), variables["index"]);

    // Create a global array of i8s
    auto array_type = ArrayType::get(builder->getInt8Ty(), 65535);
    module->getOrInsertGlobal("tape", array_type);

    GlobalVariable *array = module->getNamedGlobal("tape");
    array->setAlignment(MaybeAlign(1));
    array->setInitializer(ConstantAggregateZero::get(array_type));

    variables["tape"] = builder->CreateBitCast(array, builder->getInt8PtrTy());
}

void LLVMVisitor::finalize() {
    // add a return
    builder->CreateRet(ConstantInt::get(builder->getInt32Ty(), 0));

    // verify the main function
    verifyFunction(*main);
}

void LLVMVisitor::optimize() {
    fpm->run(*main);
}

Value *LLVMVisitor::getCurrentPtr() {
    // Load the current index value
    auto index = builder->CreateLoad(variables["index"], "tmp_index");

    // Return the array pointer offset by that index
    return builder->CreateGEP(variables["tape"], index, "cur_arr_ptr");
}

void LLVMVisitor::visitMoveNode(MoveNode *node) {
    auto index = variables["index"];

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

    auto value = ConstantInt::get(context, APInt(8, static_cast<char>(node->value), true));

    builder->CreateStore(value, current_ptr);
}

void LLVMVisitor::visitInputNode(InputNode *node) {
    auto read = builder->CreateCall(module->getFunction("getchar"), {}, "int_char");

    auto scope = builder->GetInsertBlock()->getParent();

    BasicBlock *then = BasicBlock::Create(context, "eof", scope);
    BasicBlock *_else = BasicBlock::Create(context, "not_eof");
    BasicBlock *merge = BasicBlock::Create(context, "merge");

    auto cmp = builder->CreateICmpEQ(read, ConstantInt::get(builder->getInt32Ty(), -1, true), "is_eof");
    builder->CreateCondBr(cmp, then, _else);

    builder->SetInsertPoint(then);
    builder->CreateStore(ConstantInt::get(builder->getInt8Ty(), 0), getCurrentPtr());
    builder->CreateBr(merge);

    scope->getBasicBlockList().push_back(_else);
    builder->SetInsertPoint(_else);
    auto byte = builder->CreateIntCast(read, builder->getInt8Ty(), false, "byte_char");
    builder->CreateStore(byte, getCurrentPtr());
    builder->CreateBr(merge);

    scope->getBasicBlockList().push_back(merge);
    builder->SetInsertPoint(merge);
}

void LLVMVisitor::visitOutputNode([[maybe_unused]] OutputNode *node) {
    auto const1 = ConstantInt::get(builder->getInt32Ty(), 1);
    std::vector<Value *> args{const1, getCurrentPtr(), const1};
    builder->CreateCall(module->getFunction("write"), args);
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

void LLVMVisitor::visitSequenceNode(SequenceNode *node) {
    for (auto *op : node->nodes) {
        op->accept(*this);
    }
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

    LLVMInitializeX86Target();
    LLVMInitializeX86TargetInfo();
    LLVMInitializeX86TargetMC();
    LLVMInitializeX86AsmPrinter();

    std::string Error;
    auto Target = TargetRegistry::lookupTarget(TargetTriple, Error);

    // Print an error and exit if we couldn't find the requested target.
    // This generally occurs if we've forgotten to initialise the
    // TargetRegistry or we have a bogus target triple.
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
