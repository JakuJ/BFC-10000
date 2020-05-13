#include "PrintingVisitor.hpp"

#include <iostream>
#include <MoveNode.hpp>

void PrintingVisitor::visitMoveNode(MoveNode *node) {
    std::cout << "Move " << node->distance << std::endl;
    if (node->next) {
        node->next->accept(*this);
    }
}

void PrintingVisitor::visitAddNode(AddNode *node) {
    INodeVisitor::visitAddNode(node);
}

void PrintingVisitor::visitInputNode(InputNode *node) {
    INodeVisitor::visitInputNode(node);
}

void PrintingVisitor::visitOutputNode(OutputNode *node) {
    INodeVisitor::visitOutputNode(node);
}

void PrintingVisitor::visitLoopNode(LoopNode *node) {
    INodeVisitor::visitLoopNode(node);
}
