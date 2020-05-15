#pragma once

#include "INodeVisitor.hpp"

class ContractingVisitor : public INodeVisitor {
    void visitLoopNode(LoopNode *node) override;

    void visitSequenceNode(SequenceNode *node) override;
};
