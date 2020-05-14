#pragma once

#include <vector>
#include "INodeVisitor.hpp"

class InterpretingVisitor : public INodeVisitor {

    std::vector<int> tape;
    int pointer = 0;

public:
    InterpretingVisitor();

    void visitMoveNode(MoveNode *node) override;

    void visitAddNode(AddNode *node) override;

    void visitInputNode(InputNode *node) override;

    void visitOutputNode(OutputNode *node) override;

    void visitLoopNode(LoopNode *node) override;

    void visitSequenceNode(SequenceNode *node) override;
};
