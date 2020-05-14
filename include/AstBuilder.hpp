#pragma once

#include <stack>
#include <memory>
#include <Nodes/IOperation.hpp>
#include <Nodes/SequenceNode.hpp>

class ASTBuilder {
    bool owned = false;

    std::stack<SequenceNode *> stack;

public:
    ASTBuilder();

    virtual ~ASTBuilder();

    void addOperation(INode *);

    void beginLoop();

    void endLoop();

    [[nodiscard]] std::unique_ptr<INode> getAST();
};