#pragma once

#include "INodeVisitor.hpp"

class FoldingVisitor : INodeVisitor {
    void visitLoopNode(LoopNode *node) override;

    void visitSequenceNode(SequenceNode *node) override;
};
