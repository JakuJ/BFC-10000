#include "Visitors/InterpretingVisitor.hpp"

#include <iostream>
#include <algorithm>

#include <Nodes/MoveNode.hpp>
#include <Nodes/AddNode.hpp>
#include <Nodes/LoopNode.hpp>
#include <Nodes/AddMultipleNode.hpp>
#include <Nodes/AssignmentNode.hpp>
#include <Nodes/MemsetNode.hpp>

void InterpretingVisitor::visitMoveNode(MoveNode *node) {
    pointer += node->value;
}

void InterpretingVisitor::visitAddNode(AddNode *node) {
    tape[pointer] += node->value;
}

void InterpretingVisitor::visitAddMultipleNode(AddMultipleNode *node) {
    tape[pointer + node->offset] += tape[pointer] * node->value;
}

void InterpretingVisitor::visitInputNode([[maybe_unused]] InputNode *node) {
    int code = getchar();

    if (code == EOF) {
        code = 0;
    }

    tape[pointer] = code;
}

void InterpretingVisitor::visitOutputNode([[maybe_unused]] OutputNode *node) {
    std::cout << static_cast<char>(tape[pointer]);
    charCount++;

    if (tape[pointer] == 10) {
        // newline encountered, resetting flush counter
        charCount = 0;
    } else if (charCount >= 100) {
        // flush every 100 characters
        charCount = 0;
        std::cout << std::flush;
    }
}

void InterpretingVisitor::visitLoopNode(LoopNode *node) {
    while (tape[pointer] != 0) {
        node->inside->accept(*this);
    }
}

void InterpretingVisitor::visitAssignmentNode(AssignmentNode *node) {
    tape[pointer] = node->value;
}

void InterpretingVisitor::visitMemsetNode(MemsetNode *node) {
    int first = pointer;
    int second = pointer + node->range;

    if (first > second) {
        std::swap(first, second);
    }

    std::fill(&tape[first], &tape[second] + 1, node->value);
}
