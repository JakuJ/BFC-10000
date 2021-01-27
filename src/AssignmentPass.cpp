#include "Visitors/Passes/AssignmentPass.hpp"

#include <iostream>

#include <Nodes/LoopNode.hpp>
#include <Nodes/AssignmentNode.hpp>

void AssignmentPass::dumpStats() const {
    std::cerr << "Assignments simplified: " << hits << std::endl;
}

void AssignmentPass::visitLoopNode(LoopNode *node) {
    node->inside->accept(*this);
}

void AssignmentPass::visitSequenceNode(SequenceNode *node) {
    if (node->nodes.size() < 2) {
        return;
    }

    // Allocate space for the new sequence
    std::vector < INode * > folded;
    folded.reserve(node->nodes.size());

    // Initialize state
    int foldedValue = 0;
    bool folding = false;
    bool notJustClear = false;

    // Add the Assignment node when folding is done
    auto endFold = [&]() {
        if (folding) {
            folded.push_back(new AssignmentNode(foldedValue));
            if (notJustClear) {
                hits++;
            }
        }

        // Start new fold
        folding = false;
        notJustClear = false;
        foldedValue = 0;
    };

    for (INode *n : node->nodes) {
        switch (n->symbol) {
            case '[':
                endFold();
                n->accept(*this);
                folded.push_back(n);
                break;
            case '+':
                if (folding) {
                    foldedValue += n->value;
                    notJustClear = true;
                    delete n;
                } else {
                    folded.push_back(n);
                }
                break;
            case 'a':
                folding = true;
                foldedValue = node->value;
                delete n;
                break;
            default:
                endFold();
                folded.push_back(n);
                break;
        }
    }

    // Sequence might end with a fold
    endFold();

    node->nodes = folded;
}