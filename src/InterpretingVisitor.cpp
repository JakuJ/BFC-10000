#include "Visitors/InterpretingVisitor.hpp"

#include <iostream>

#include <Nodes/MoveNode.hpp>
#include <Nodes/AddNode.hpp>
#include <Nodes/LoopNode.hpp>

InterpretingVisitor::InterpretingVisitor() : tape(30000, 0), pointer(0) {}

void InterpretingVisitor::visitMoveNode(MoveNode *node) {
    pointer += node->value;
}

void InterpretingVisitor::visitAddNode(AddNode *node) {
    tape[pointer] += node->value;
}

void InterpretingVisitor::visitInputNode([[maybe_unused]] InputNode *node) {
    int code = getchar();

    if (code == EOF) {
        code = 0;
    }

    tape[pointer] = code;
}

void InterpretingVisitor::visitOutputNode([[maybe_unused]] OutputNode *node) {
    std::cout << static_cast<char>(tape[pointer]) << std::flush;
}

void InterpretingVisitor::visitLoopNode(LoopNode *node) {
    while (tape[pointer] != 0) {
        node->inside->accept(*this);
    }
}

void InterpretingVisitor::visitSequenceNode(SequenceNode *node) {
    for (auto *op : node->nodes) {
        op->accept(*this);
    }
}
