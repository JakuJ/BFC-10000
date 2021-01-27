#include "Visitors/PrintingVisitor.hpp"

#include <iostream>
#include <fstream>

#include <Nodes/MoveNode.hpp>
#include <Nodes/LoopNode.hpp>
#include <Nodes/AddNode.hpp>
#include <Nodes/AddMultipleNode.hpp>
#include <Nodes/AssignmentNode.hpp>
#include <Nodes/MemsetNode.hpp>

PrintingVisitor::PrintingVisitor(const std::string &path) {
    out = new std::ofstream(path);
}

PrintingVisitor::~PrintingVisitor() {
    delete out;
}

void PrintingVisitor::indent() {
    for (int i = 0; i < loopDepth; i++) {
        *out << "  ";
    }
}

void PrintingVisitor::visitMoveNode(MoveNode *node) {
    indent();
    *out << "Move " << abs(node->value) << " cells to the " << (node->value >= 0 ? "right" : "left") << std::endl;
}

void PrintingVisitor::visitAddNode(AddNode *node) {
    indent();
    *out << "Add " << node->value << " to current cell" << std::endl;
}

void PrintingVisitor::visitAddMultipleNode(AddMultipleNode *node) {
    indent();
    if (node->offset == 0 && node->value == 0) {
        *out << "Clear current cell" << std::endl;
    } else {
        *out << "Add <current cell value> * " << node->value << " to cell at offset " << node->offset << std::endl;
    }
}

void PrintingVisitor::visitInputNode(InputNode *node) {
    indent();
    *out << "Read user input" << std::endl;
}

void PrintingVisitor::visitOutputNode(OutputNode *node) {
    indent();
    *out << "Print current cell" << std::endl;
}

void PrintingVisitor::visitLoopNode(LoopNode *node) {
    indent();
    *out << "In loop:" << std::endl;

    loopDepth++;
    node->inside->accept(*this);
    loopDepth--;

    indent();
    *out << "End loop" << std::endl;
}

void PrintingVisitor::visitAssignmentNode(AssignmentNode *node) {
    indent();
    *out << "Set current cell to " << node->value << std::endl;
}

void PrintingVisitor::visitMemsetNode(MemsetNode *node) {
    indent();
    *out << "Memset cells up to index <current " << (node->range > 0 ? '+' : '-') << " " << abs(node->range) << "> to "
         << node->value << std::endl;
}
