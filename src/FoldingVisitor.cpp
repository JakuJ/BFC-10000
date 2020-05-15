#include "Visitors/FoldingVisitor.hpp"

void FoldingVisitor::visitLoopNode(LoopNode *node) {
    INodeVisitor::visitLoopNode(node);
}

void FoldingVisitor::visitSequenceNode(SequenceNode *node) {
    INodeVisitor::visitSequenceNode(node);
}
