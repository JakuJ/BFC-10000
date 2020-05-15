#include "Visitors/FoldingVisitor.hpp"

#include <Nodes/SequenceNode.hpp>
#include <Nodes/LoopNode.hpp>
#include <Nodes/AddNode.hpp>

void FoldingVisitor::visitLoopNode(LoopNode *node) {
    node->inside->accept(*this);
}

void FoldingVisitor::visitSequenceNode(SequenceNode *node) {
    if (node->nodes.empty()) {
        return;
    }

    std::vector<INode *> folded;
    folded.reserve(node->nodes.size());

    INode *lastNode = node->nodes.front();

    folded.push_back(lastNode);

    for (int i = 1; i < node->nodes.size(); i++) {
        INode *n = node->nodes[i];

        switch (n->symbol) {
            case '[':
                n->accept(*this);
                folded.push_back(n);
                lastNode = n;
                break;
            case '+':
                if (lastNode->symbol == '+') {
                    lastNode->value += n->value;
                    delete n;
                } else {
                    folded.push_back(n);
                    lastNode = n;
                }
                break;
            case '>':
                if (lastNode->symbol == '>') {
                    lastNode->value += n->value;
                    delete n;
                } else {
                    folded.push_back(n);
                    lastNode = n;
                }
                break;
            default:
                folded.push_back(n);
                lastNode = n;
                break;
        }
    }

    node->nodes = folded;
}
