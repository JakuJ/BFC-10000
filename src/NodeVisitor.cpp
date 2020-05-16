#include <Visitors/NodeVisitor.hpp>

#include <Nodes/SequenceNode.hpp>

void NodeVisitor::visitSequenceNode(SequenceNode *node) {
    for (auto *n : node->nodes) {
        n->accept(*this);
    }
}
