#include "Visitors/Passes/ZeroOutVisitor.hpp"

#include <Nodes/LoopNode.hpp>
#include <Nodes/SetNode.hpp>

void ZeroOutVisitor::visitLoopNode(LoopNode *node) {
    node->inside->accept(*this);
}

void ZeroOutVisitor::visitSequenceNode(SequenceNode *node) {
    if (node->nodes.empty()) {
        return;
    }

    std::vector<INode *> folded;
    folded.reserve(node->nodes.size());


    for (auto *n : node->nodes) {
        switch (n->symbol) {
            case '[': {
                const auto &loop_nodes = dynamic_cast<LoopNode *>(n)->inside->nodes;
                if (loop_nodes.size() == 1 && loop_nodes[0]->symbol == '+') {
                    auto setNode = new SetNode(0);
                    folded.push_back(setNode);
                    delete n;
                } else {
                    n->accept(*this);
                    folded.push_back(n);
                }
                break;
            }
            default:
                folded.push_back(n);
                break;
        }
    }

    node->nodes = folded;
}
