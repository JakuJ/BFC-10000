#pragma once

#include "IOperation.hpp"

struct MoveNode : public IOperation {
    int distance;

    explicit MoveNode(int distance) : IOperation(distance > 0 ? '>' : '<'), distance(distance) {}

    ~MoveNode() override = default;

    void accept(INodeVisitor &visitor) override {
        visitor.visitMoveNode(this);
    }
};

