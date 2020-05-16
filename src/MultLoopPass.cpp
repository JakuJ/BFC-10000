#include "Visitors/Passes/MultLoopPass.hpp"

#include <iostream>
#include <map>

#include <Nodes/LoopNode.hpp>
#include <Nodes/SetNode.hpp>

void MultLoopPass::dumpStats() const {
    std::cerr << "Multiplication loops simplified: " << hits << std::endl;
}

void MultLoopPass::visitLoopNode(LoopNode *node) {
    node->inside->accept(*this);
}

std::optional<std::vector<SetNode *>> MultLoopPass::trySimplify(const std::vector<INode *> &body) {

    bool condition = body[0]->symbol == '+' &&
                     body[0]->value == -1 &&
                     body[1]->symbol == '>';

    if (!condition) {
        return std::nullopt;
    }

    std::map<char, std::map<char, char>> machine{
            {'S', {{'>', 'M'}}},
            {'M', {{'>', 'M'}, {'+', 'A'}}},
            {'A', {{'+', 'A'}, {'>', 'E'}}},
            {'E', {{'>', 'M'}, {'+', 'A'}}}
    };

    std::vector<SetNode *> ret;
    int offset = 0, mult = 0;
    char state = 'S';

    for (int i = 1; i < body.size(); i++) {
        char action = body[i]->symbol;

        if (machine[state].find(action) == machine[state].end()) {
            goto failure;
        } else {
            state = machine[state][action];
        }

        switch (state) {
            case 'M':
                offset += body[i]->value;
                break;
            case 'A':
                mult += body[i]->value;
                break;
            case 'E':
                ret.push_back(new SetNode(offset, mult));
                offset += body[i]->value;
                mult = 0;
                break;
            default:
                break;
        }
    }

    if ((state == 'E' || state == 'M') && offset == 0) {
        ret.push_back(new SetNode(0, 0));
        return std::optional(ret);
    }

    failure:
    for (auto *n : ret) {
        delete n;
    }
    return std::nullopt;
}

void MultLoopPass::visitSequenceNode(SequenceNode *node) {
    if (node->nodes.empty()) {
        return;
    }

    std::vector<INode *> folded;
    folded.reserve(node->nodes.size());

    for (auto *n : node->nodes) {
        switch (n->symbol) {
            case '[': {
                auto inside = dynamic_cast<LoopNode *>(n)->inside->nodes;
                auto optRed = trySimplify(inside);
                if (optRed.has_value()) {
                    folded.insert(folded.end(), optRed->begin(), optRed->end());
                    hits++;
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

