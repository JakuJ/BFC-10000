#include "Visitors/PrintingVisitor.hpp"

#include <iostream>

#include <Nodes/MoveNode.hpp>
#include <Nodes/LoopNode.hpp>
#include <Nodes/AddNode.hpp>

void PrintingVisitor::visitMoveNode(MoveNode *node) {
    std::cout << "Move " << node->value << std::endl;
}

void PrintingVisitor::visitAddNode(AddNode *node) {
    std::cout << "Add " << node->value << std::endl;
}

void PrintingVisitor::visitInputNode(InputNode *node) {
    std::cout << "Read" << std::endl;
}

void PrintingVisitor::visitOutputNode(OutputNode *node) {
    std::cout << "Print" << std::endl;
}

void PrintingVisitor::visitLoopNode(LoopNode *node) {
    std::cout << "In loop:" << std::endl;
    node->inside->accept(*this);
    std::cout << "End loop" << std::endl;
}

void PrintingVisitor::visitSequenceNode(SequenceNode *node) {
    for (auto *n : node->nodes) {
        n->accept(*this);
    }
}
