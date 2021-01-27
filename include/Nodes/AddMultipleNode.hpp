#pragma once

#include <Nodes/INode.hpp>

struct AddMultipleNode : public INode {
    int offset;

    explicit AddMultipleNode(int _offset, int _value) : offset(_offset) {
        symbol = 's';
        value = _value;
    }

    void accept(NodeVisitor &visitor) override {
        visitor.visitAddMultipleNode(this);
    }
};
