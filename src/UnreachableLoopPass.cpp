#include "Visitors/Passes/UnreachableLoopPass.hpp"

#include <iostream>
#include <Nodes/LoopNode.hpp>

void UnreachableLoopPass::dumpStats() const {
    std::cerr << "Unreachable loops removed: " << hits << std::endl;
}

void UnreachableLoopPass::visitLoopNode(LoopNode *node) {
    node->inside->accept(*this);
}

void UnreachableLoopPass::visitSequenceNode(SequenceNode *node) {
    if (node->nodes.empty()) {
        return;
    }

    std::vector<INode *> folded;
    folded.reserve(node->nodes.size());

    bool lastLoop = true;

    for (auto *n : node->nodes) {
        switch (n->symbol) {
            case '[':
                if (lastLoop) {
                    delete n;
                    hits++;
                } else {
                    n->accept(*this);
                    folded.push_back(n);
                    lastLoop = true;
                }
                break;
            default:
                lastLoop = false;
                folded.push_back(n);
                break;
        }
    }

    node->nodes = folded;
}
