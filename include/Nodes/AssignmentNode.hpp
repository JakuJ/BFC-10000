#pragma once

#include <Nodes/INode.hpp>

struct AssignmentNode : public INode {
    explicit AssignmentNode(int _value) {
        symbol = 'a';
        value = _value;
    }

    void accept(NodeVisitor &visitor) override {
        visitor.visitAssignmentNode(this);
    }
};
