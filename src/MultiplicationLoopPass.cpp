#include "Visitors/Passes/MultiplicationLoopPass.hpp"

#include <iostream>
#include <unordered_map>

#include <Nodes/LoopNode.hpp>
#include <Nodes/AddMultipleNode.hpp>
#include <Nodes/AssignmentNode.hpp>

void MultiplicationLoopPass::dumpStats() const {
    std::cerr << "Multiplication loops simplified: " << hits << std::endl;
}

void MultiplicationLoopPass::visitLoopNode(LoopNode *node) {
    node->inside->accept(*this);
}

std::optional <std::vector<INode *>> MultiplicationLoopPass::trySimplify(const std::vector<INode *> &body) {
    std::vector < INode * > ret;
    int offset = 0, multiplier = 0;
    int loop_cell_change = 0;
    char last = body[0]->symbol;

    auto dump = [&]() {
        if (offset == 0) {
            loop_cell_change += multiplier;
        } else {
            ret.push_back(new AddMultipleNode(offset, multiplier));
        }
        multiplier = 0;
    };

    // Interpret
    for (INode *n : body) {
        if (n->symbol == '>') {
            if (last != '>') {
                dump();
            }
            offset += n->value;
        } else if (n->symbol == '+') {
            multiplier += n->value;
        } else {
            goto failure;
        }
        last = n->symbol;
    }

    if (multiplier != 0) {
        dump();
    }

    if (offset == 0 && loop_cell_change == -1) {
        ret.push_back(new AssignmentNode(0));
        return ret;
    }

    failure:
    for (auto *n : ret) {
        delete n;
    }
    return std::nullopt;
}

void MultiplicationLoopPass::visitSequenceNode(SequenceNode *node) {
    if (node->nodes.empty()) {
        return;
    }

    std::vector < INode * > folded;
    folded.reserve(node->nodes.size());

    for (auto *n : node->nodes) {
        if (n->symbol == '[') {
            auto &inside = dynamic_cast<LoopNode *>(n)->inside->nodes;
            auto optRed = trySimplify(inside);
            if (optRed.has_value()) {
                for (auto *d : inside) {
                    delete d;
                }
                inside.clear();
                inside.insert(inside.end(), optRed->begin(), optRed->end());
                hits++;
            } else {
                n->accept(*this);
            }
        }
        folded.push_back(n);
    }

    node->nodes = folded;
}
