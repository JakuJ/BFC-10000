#pragma once

#include "INode.hpp"

struct MoveNode : public INode {
    int distance;

    explicit MoveNode(int distance) : INode(distance > 0 ? '>' : '<'), distance(distance) {}

    ~MoveNode() override = default;

    void accept(INodeVisitor &visitor) override {
        visitor.visitMoveNode(this);
    }
};

