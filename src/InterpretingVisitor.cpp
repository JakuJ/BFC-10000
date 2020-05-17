#include "Visitors/InterpretingVisitor.hpp"

#include <iostream>

#include <Nodes/MoveNode.hpp>
#include <Nodes/AddNode.hpp>
#include <Nodes/LoopNode.hpp>
#include <Nodes/SetNode.hpp>

InterpretingVisitor::InterpretingVisitor() : tape(65535, 0), pointer(0) {}

void InterpretingVisitor::visitMoveNode(MoveNode *node) {
    pointer += node->value;
}

void InterpretingVisitor::visitAddNode(AddNode *node) {
    tape[pointer] += node->value;
}

void InterpretingVisitor::visitSetNode(SetNode *node) {
    if (node->offset == 0 && node->value == 0) {
        tape[pointer] = 0;
    } else {
        tape[pointer + node->offset] += tape[pointer] * node->value;
    }
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
