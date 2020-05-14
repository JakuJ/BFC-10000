#pragma once

#include <map>
#include <memory>

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/LegacyPassManager.h"

#include "INodeVisitor.hpp"

using namespace llvm;

class LLVMVisitor : public INodeVisitor {
    LLVMContext context;
    std::unique_ptr<Module> module;
    std::unique_ptr<legacy::FunctionPassManager> fpm;
    std::unique_ptr<IRBuilder<>> builder;

    Function *main;

    std::map<std::string, Value *> variables;

public:
    LLVMVisitor();

    Value *getCurrentPtr();

    void dumpCode();

    int compile();

    void visitMoveNode(MoveNode *node) override;

    void visitAddNode(AddNode *node) override;

    void visitInputNode(InputNode *node) override;

    void visitOutputNode(OutputNode *node) override;

    void visitLoopNode(LoopNode *node) override;

    void visitSequenceNode(SequenceNode *node) override;
};
