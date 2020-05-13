#pragma once

#include <INode.hpp>

struct MoveNode : public INode {
    int distance;

    explicit MoveNode(int distance) : distance(distance) {}

    ~MoveNode() override = default;

    void accept(INodeVisitor &visitor) override {
        visitor.visitMoveNode(this);
    }
};

