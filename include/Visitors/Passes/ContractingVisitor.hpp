#pragma once

#include "Visitors/INodeVisitor.hpp"

class ContractingVisitor : public INodeVisitor {
    void visitLoopNode(LoopNode *node) override;

    void visitSequenceNode(SequenceNode *node) override;
};
