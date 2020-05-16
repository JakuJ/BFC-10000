#pragma once

#include <Nodes/INode.hpp>

struct SetNode : public INode {
    int offset;

    explicit SetNode(int _offset, int _value) : offset(_offset) {
        symbol = 's';
        value = _value;
    }

    void accept(NodeVisitor &visitor) override {
        visitor.visitSetNode(this);
    }
};
