#pragma once

#include "INodeVisitor.hpp"

class PrintingVisitor : public INodeVisitor {
public:
    void visitMoveNode(MoveNode *node) override;

    void visitAddNode(AddNode *node) override;

    void visitInputNode(InputNode *node) override;

    void visitOutputNode(OutputNode *node) override;

    void visitLoopNode(LoopNode *node) override;

    void visitSequenceNode(SequenceNode *node) override;
};
