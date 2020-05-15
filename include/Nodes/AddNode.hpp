#pragma once

#include <Nodes/INode.hpp>

struct AddNode : public INode {
    int value;

    explicit AddNode(int value) : INode(value > 0 ? '+' : '-'), value(value) {}

    ~AddNode() override = default;

    void accept(INodeVisitor &visitor) override {
        visitor.visitAddNode(this);
    }
};
