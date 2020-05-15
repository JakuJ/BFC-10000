#pragma once

#include <Visitors/INodeVisitor.hpp>

class ZeroOutVisitor : public INodeVisitor {
public:
    void visitLoopNode(LoopNode *node) override;

    void visitSequenceNode(SequenceNode *node) override;
};
