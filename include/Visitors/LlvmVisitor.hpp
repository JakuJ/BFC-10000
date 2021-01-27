#pragma once

#include <memory>
#include <string>

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/LegacyPassManager.h"

#include "NodeVisitor.hpp"

using namespace llvm;

class LLVMVisitor : public NodeVisitor {
    LLVMContext context;
    std::unique_ptr<Module> module;
    std::unique_ptr<legacy::FunctionPassManager> fpm;
    std::unique_ptr<IRBuilder<>> builder;

    Function *main;
    Type *int_type;
    Value *index, *tape;
    FunctionCallee write_func, getchar_func;

public:
    LLVMVisitor();

    Value *getCurrentPtr();

    void finalize();

    void optimize();

    void dumpCode(const std::string &);

    int compile(const std::string &);

    void visitMoveNode(MoveNode *) override;

    void visitAddNode(AddNode *) override;

    void visitSetNode(AddMultipleNode *node) override;

    void visitInputNode(InputNode *) override;

    void visitOutputNode(OutputNode *) override;

    void visitLoopNode(LoopNode *) override;
};
