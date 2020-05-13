#pragma once

#include <INode.hpp>

struct AddNode : public INode {
    int value;

    explicit AddNode(int value) : value(value) {}

    ~AddNode() override = default;

    void accept(INodeVisitor &visitor) override {
        visitor.visitAddNode(this);
    }
};
