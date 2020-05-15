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

    void finalize();

    void optimize();

    void dumpCode(const std::string &);

    int compile(const std::string &);

    void visitMoveNode(MoveNode *) override;

    void visitAddNode(AddNode *) override;

    void visitSetNode(SetNode *node) override;

    void visitInputNode(InputNode *) override;

    void visitOutputNode(OutputNode *) override;

    void visitLoopNode(LoopNode *) override;

    void visitSequenceNode(SequenceNode *) override;
};
