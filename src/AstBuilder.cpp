#include "AstBuilder.hpp"

#include <Nodes/LoopNode.hpp>

ASTBuilder::ASTBuilder() {
    auto *root = new SequenceNode();
    stack.push(root);
}

ASTBuilder::~ASTBuilder() {
    if (!owned) {
        while (stack.size() > 1) {
            stack.pop();
        }
        delete stack.top();
    }
}

void ASTBuilder::addOperation(INode *op) {
    stack.top()->nodes.emplace_back(op);
}

void ASTBuilder::beginLoop() {
    auto *loop = new LoopNode();
    addOperation(dynamic_cast<INode *>(loop));
    stack.push(loop->inside);
}

void ASTBuilder::endLoop() {
    if (stack.size() == 1) {
        throw std::runtime_error("No matching loop to finish");
    }
    stack.pop();
}

std::unique_ptr<INode> ASTBuilder::getAST() {
    owned = true;
    return std::unique_ptr<INode>(stack.top());
}


