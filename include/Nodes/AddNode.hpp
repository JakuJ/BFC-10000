#pragma once

#include <Nodes/INode.hpp>

struct AddNode : public INode {
    explicit AddNode(int _value) {
        value = _value;
        symbol = '+';
    }

    ~AddNode() override = default;

    void accept(INodeVisitor &visitor) override {
        visitor.visitAddNode(this);
    }
};
