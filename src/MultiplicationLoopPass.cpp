#include "Visitors/Passes/MultiplicationLoopPass.hpp"

#include <iostream>
#include <unordered_map>

#include <Nodes/LoopNode.hpp>
#include <Nodes/AddMultipleNode.hpp>

void MultiplicationLoopPass::dumpStats() const {
    std::cerr << "Multiplication loops simplified: " << hits << std::endl;
}

void MultiplicationLoopPass::visitLoopNode(LoopNode *node) {
    node->inside->accept(*this);
}

std::optional <std::vector<AddMultipleNode *>> MultiplicationLoopPass::trySimplify(const std::vector<INode *> &body) {

    bool condition = body[0]->symbol == '+' && body[0]->value == -1;

    if (!condition) {
        return std::nullopt;
    }

    std::unordered_map<char, std::unordered_map<char, char>> machine{
            {'B', {{'>', 'C'}}},
            {'C', {{'>', 'D'}, {'+', 'E'}}},
            {'D', {{'>', 'D'}, {'+', 'E'}}},
            {'E', {{'>', 'F'}, {'+', 'G'}}},
            {'G', {{'>', 'F'}, {'+', 'G'}}},
            {'F', {{'>', 'H'}, {'+', 'I'}}},
            {'H', {{'>', 'H'}, {'+', 'I'}}},
            {'I', {{'>', 'F'}, {'+', 'J'}}},
            {'J', {{'>', 'F'}, {'+', 'J'}}}
    };

    std::vector < AddMultipleNode * > ret;
    int offset = 0, mult = 0;
    char state = 'B';

    for (int i = 1; i < body.size(); i++) {
        char action = body[i]->symbol;

        if (machine[state].find(action) == machine[state].end()) {
            goto failure;
        } else {
            state = machine[state][action];
        }

        if (state == 'F') {
            ret.push_back(new AddMultipleNode(offset, mult));
            mult = 0;
        }

        if (action == '+') {
            mult += body[i]->value;
        } else if (action == '>') {
            offset += body[i]->value;
        }
    }

    if ((state == 'F' || state == 'H') && offset == 0) {
        ret.push_back(new AddMultipleNode(0, 0));
        return std::optional(ret);
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
