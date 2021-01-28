#include "Visitors/Passes/MemsetPass.hpp"

#include <iostream>

#include <Nodes/SequenceNode.hpp>
#include <Nodes/MoveNode.hpp>
#include <Nodes/LoopNode.hpp>
#include <Nodes/AddNode.hpp>
#include <Nodes/MemsetNode.hpp>

void MemsetPass::dumpStats() const {
    std::cerr << "Memsets found: " << hits << std::endl;
}

void MemsetPass::visitLoopNode(LoopNode *node) {
    node->inside->accept(*this);
}

void MemsetPass::visitSequenceNode(SequenceNode *node) {
    if (node->nodes.empty()) {
        return;
    }

    // Allocate space for the new sequence
    std::vector < INode * > folded;
    folded.reserve(node->nodes.size());

    int i = 0;
    while (i + 2 < node->nodes.size()) {
        INode *n = node->nodes[i];

        if (n->symbol == 'a') {
            // Folding
            auto next = node->nodes[i + 1];
            auto next2 = node->nodes[i + 2];

            // Check if this is even memset
            if (next->symbol == '>' && abs(next->value) == 1) {
                if (next2->symbol == 'a' && next2->value == n->value) {
                    int howMany = 1, direction = next->value;
                    i++;

                    // Memset more than once
                    while (true) {
                        i += 2;

                        if (i + 1 >= node->nodes.size()) {
                            // Out of range
                            break;
                        }

                        next = node->nodes[i];
                        next2 = node->nodes[i + 1];

                        if (next->symbol == '>' && next->value == direction) {
                            if (next2->symbol == 'a' && next2->value == n->value) {
                                howMany++;
                                delete next;
                                delete next2;
                                continue;
                            }
                        }

                        break;
                    }

                    // Finished, add memset and move nodes
                    folded.push_back(new MemsetNode(howMany * direction, n->value));
                    folded.push_back(new MoveNode(howMany * direction));
                    hits++;
                    delete n;

                    // Next loop
                    continue;
                }
            }
        } else if (n->symbol == '[') {
            n->accept(*this);
        }

        folded.push_back(n);
        i++;
    }

    // Copy left-over nodes
    for (; i < node->nodes.size(); i++) {
        INode *n = node->nodes[i];

        if (n->symbol == '[') {
            n->accept(*this);
        }

        folded.push_back(n);
    }

    node->nodes = folded;
}
