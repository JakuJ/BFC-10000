#pragma once

#include <Nodes/INode.hpp>

struct SetNode : public INode {
    explicit SetNode(int _value) {
        symbol = 's';
        value = _value;
    }

    void accept(INodeVisitor &visitor) override {
        visitor.visitSetNode(this);
    }
};
