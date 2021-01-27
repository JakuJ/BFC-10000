#include "Visitors/Passes/FoldingPass.hpp"

#include <iostream>

#include <Nodes/SequenceNode.hpp>
#include <Nodes/MoveNode.hpp>
#include <Nodes/LoopNode.hpp>
#include <Nodes/AddNode.hpp>

void FoldingPass::dumpStats() const {
    std::cerr << "Folded instruction sequences: " << hits << std::endl;
}

void FoldingPass::visitLoopNode(LoopNode *node) {
    node->inside->accept(*this);
}

void FoldingPass::visitSequenceNode(SequenceNode *node) {
    if (node->nodes.empty()) {
        return;
    }

    // Allocate space for the new sequence
    std::vector < INode * > folded;
    folded.reserve(node->nodes.size());

    // Initialize add/move counters
    int foldedValue = 0;
    char foldedSymbol = '+';

    // Add the Move/Add node when folding is done
    auto endFold = [&]() {
        if (foldedValue != 0) {
            hits++;
            if (foldedSymbol == '>') {
                folded.push_back(new MoveNode(foldedValue));
            } else if (foldedSymbol == '+') {
                folded.push_back(new AddNode(foldedValue));
            }
        }

        // Start new fold
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
            case '>':
                if (n->symbol == foldedSymbol) {
                    foldedValue += n->value;
                    delete n;
                } else {
                    endFold();
                    foldedSymbol = n->symbol;
                    foldedValue = n->value;
                }
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
