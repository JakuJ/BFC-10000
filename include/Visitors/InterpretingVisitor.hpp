#pragma once

#include <array>
#include "NodeVisitor.hpp"

class InterpretingVisitor : public NodeVisitor {

    std::array<int, 65535> tape;
    int pointer = 0;

public:
    void visitMoveNode(MoveNode *node) override;

    void visitAddNode(AddNode *node) override;

    void visitAddMultipleNode(AddMultipleNode *node) override;

    void visitInputNode(InputNode *node) override;

    void visitOutputNode(OutputNode *node) override;

    void visitLoopNode(LoopNode *node) override;

    void visitAssignmentNode(AssignmentNode *node) override;

    void visitMemsetNode(MemsetNode *node) override;
};
