#pragma once

#include "INode.hpp"

struct MoveNode : public INode {
    explicit MoveNode(int distance) {
        value = distance;
        symbol = '>';
    }

    void accept(NodeVisitor &visitor) override {
        visitor.visitMoveNode(this);
    }
};

