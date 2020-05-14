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

#define OPTIMIZE 1

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

    // Declare the "memset" function
//    std::vector<Type *> memset_params{
//            Type::getInt8PtrTy(context),
//            Type::getInt8Ty(context),
//            Type::getInt64Ty(context),
//            Type::getInt32Ty(context),
//            Type::getInt1Ty(context),
//    };
//    FunctionType *memsetType = FunctionType::get(Type::getVoidTy(context), memset_params, false);
//    Function::Create(memsetType, Function::ExternalLinkage, "memset", module.get());

    // Declare the "printf" function
    std::vector<Type *> printf_params{Type::getInt8PtrTy(context)};
    FunctionType *printfType = FunctionType::get(Type::getInt64Ty(context), printf_params, true);
    Function::Create(printfType, Function::ExternalLinkage, "printf", module.get());

    // Declare the "main" function of type void(void)
    FunctionType *mainType = FunctionType::get(Type::getInt64Ty(context), false);
    main = Function::Create(mainType, Function::ExternalLinkage, "main", module.get());

    // Begin "main" body
    BasicBlock *BB = BasicBlock::Create(context, "entry", main);
    builder->SetInsertPoint(BB);

    // Create global variables
    builder->CreateGlobalString("%c\n", "format");

    // Create a named stack variable - an i64 index
    variables["index"] = builder->CreateAlloca(Type::getInt64Ty(context), nullptr, "index");
    builder->CreateStore(ConstantInt::get(Type::getInt64Ty(context), 0), variables["index"]);

    // Create an array of i8s
    auto array_type = ArrayType::get(Type::getInt8Ty(context), 30000);
    auto array = builder->CreateAlloca(array_type, nullptr, "tape");

//    std::vector<Value *> memset_args{
//            builder->CreateBitCast(array, Type::getInt8PtrTy(context)),
//            ConstantInt::get(context, APInt(8, 0, true)),
//            ConstantInt::get(context, APInt(64, 30000, false)),
//            ConstantInt::get(context, APInt(32, 8, false)),
//            ConstantInt::get(context, APInt(1, 0, false))
//    };

//    builder->CreateCall(module->getFunction("memset"), memset_args);

    variables["tape"] = array;
}

Value *LLVMVisitor::getCurrentPtr() {
    // Cast the index to the beginning of the tape to an integer
    auto tape_ptr = builder->CreatePtrToInt(variables["tape"], Type::getInt64Ty(context), "tape_int");

    // Load the current index value
    auto index = builder->CreateLoad(variables["index"], "index");

    auto ptr_int = builder->CreateAdd(tape_ptr, index, "pointer_int");

    //

//    auto offset = builder->CreateGEP(variables["tape"], index, "pointer");
//    return offset;

    //

    return builder->CreateIntToPtr(ptr_int, Type::getInt8PtrTy(context), "pointer");
}

void LLVMVisitor::visitMoveNode(MoveNode *node) {
    auto index = variables["index"];

    auto const1 = ConstantInt::get(context, APInt(64, node->distance, true));

    auto loaded = builder->CreateLoad(index, "tmp_ptr");
    auto next = builder->CreateAdd(loaded, const1, "inc_tmp");
    builder->CreateStore(next, index);
}

void LLVMVisitor::visitAddNode(AddNode *node) {
    auto current_ptr = getCurrentPtr();

    auto current = builder->CreateLoad(current_ptr, "current");
    auto value = ConstantInt::get(context, APInt(8, node->value, true));

    auto next = builder->CreateAdd(current, value, "new");
    builder->CreateStore(next, current_ptr);
}

void LLVMVisitor::visitInputNode(InputNode *node) {
    INodeVisitor::visitInputNode(node);
}

void LLVMVisitor::visitOutputNode([[maybe_unused]] OutputNode *node) {
    auto format_ptr = builder->CreateBitCast(module->getNamedGlobal("format"), Type::getInt8PtrTy(context));

    auto current = builder->CreateLoad(getCurrentPtr(), "current");

    std::vector<Value *> args{format_ptr, current};

    builder->CreateCall(module->getFunction("printf"), args);
}

void LLVMVisitor::visitLoopNode(LoopNode *node) {
    node->inside->accept(*this);
}

void LLVMVisitor::visitSequenceNode(SequenceNode *node) {
    for (auto *op : node->nodes) {
        op->accept(*this);
    }
}

void LLVMVisitor::dumpCode() {
    // return void
    builder->CreateRet(ConstantInt::get(Type::getInt64Ty(context), 0));

    // verify function and run optimizations
    verifyFunction(*main);

#if OPTIMIZE
    fpm->run(*main);
#endif

//    auto Filename = "output.ll";
//    std::error_code EC;
//    raw_fd_ostream dest(Filename, EC, sys::fs::OF_None);
//
//    if (EC) {
//        std::cerr << "Could not open file: " << EC.message();
//    } else {
//        module->print(dest, nullptr);
//        dest.flush();
//        dest.close();
//    }

    module->print(outs(), nullptr);
}

int LLVMVisitor::compile() {
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

//    auto CPU = "generic";
//    auto Features = "";

    auto CPU = "x86-64";
    auto Features = "+64bit";

    TargetOptions opt;
    auto RM = Optional<Reloc::Model>();
    auto TargetMachine = Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

    module->setDataLayout(TargetMachine->createDataLayout());
    module->setTargetTriple(TargetTriple);

    auto Filename = "output.o";
    std::error_code EC;
    raw_fd_ostream dest(Filename, EC, sys::fs::OF_None);

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

#if OPTIMIZE
    fpm->run(*main);
#endif

    pass.run(*module);
    dest.flush();
    dest.close();

    return 0;
}
