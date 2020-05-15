#include "Visitors/PrintingVisitor.hpp"

#include <iostream>
#include <fstream>

#include <Nodes/MoveNode.hpp>
#include <Nodes/LoopNode.hpp>
#include <Nodes/AddNode.hpp>

PrintingVisitor::PrintingVisitor(const std::string &path) {
    out = new std::ofstream(path);
}

PrintingVisitor::~PrintingVisitor() {
    delete out;
}

void PrintingVisitor::visitMoveNode(MoveNode *node) {
    *out << "Move " << node->value << std::endl;
}

void PrintingVisitor::visitAddNode(AddNode *node) {
    *out << "Add " << node->value << std::endl;
}

void PrintingVisitor::visitInputNode(InputNode *node) {
    *out << "Read" << std::endl;
}

void PrintingVisitor::visitOutputNode(OutputNode *node) {
    *out << "Print" << std::endl;
}

void PrintingVisitor::visitLoopNode(LoopNode *node) {
    *out << "In loop:" << std::endl;
    node->inside->accept(*this);
    *out << "End loop" << std::endl;
}

void PrintingVisitor::visitSequenceNode(SequenceNode *node) {
    for (auto *n : node->nodes) {
        n->accept(*this);
    }
}
