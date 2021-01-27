#pragma once

#include <Nodes/INode.hpp>

struct MemsetNode : public INode {
    int range;

    explicit MemsetNode(int _range, int _value) : range(_range) {
        symbol = 'm';
        value = _value;
    }

    void accept(NodeVisitor &visitor) override {
        visitor.visitMemsetNode(this);
    }
};
