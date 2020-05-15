#pragma once

#include "INode.hpp"

struct MoveNode : public INode {
    explicit MoveNode(int distance) {
        value = distance;
        symbol = '>';
    }

    ~MoveNode() override = default;

    void accept(INodeVisitor &visitor) override {
        visitor.visitMoveNode(this);
    }
};

