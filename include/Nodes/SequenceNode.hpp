#pragma once

#include <vector>
#include <memory>
#include <Nodes/INode.hpp>

struct SequenceNode : public INode {
    std::vector<INode *> nodes;

    ~SequenceNode() override {
        for (auto *ptr : nodes) {
            delete ptr;
        }
    };

    void accept(INodeVisitor &visitor) override {
        visitor.visitSequenceNode(this);
    };
};
