#pragma once

#include "INodeVisitor.hpp"

class FoldingVisitor : public INodeVisitor {
    void visitLoopNode(LoopNode *node) override;

    void visitSequenceNode(SequenceNode *node) override;
};
