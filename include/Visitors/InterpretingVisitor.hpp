#pragma once

#include <vector>
#include "NodeVisitor.hpp"

class InterpretingVisitor : public NodeVisitor {

    std::vector<int> tape;
    int pointer = 0;

public:
    InterpretingVisitor();

    void visitMoveNode(MoveNode *node) override;

    void visitAddNode(AddNode *node) override;

    void visitSetNode(AddMultipleNode *node) override;

    void visitInputNode(InputNode *node) override;

    void visitOutputNode(OutputNode *node) override;

    void visitLoopNode(LoopNode *node) override;
};
