#pragma once

#include <Nodes/IOperation.hpp>

struct AddNode : public IOperation {
    int value;

    explicit AddNode(int value) : IOperation(value > 0 ? '+' : '-'), value(value) {}

    ~AddNode() override = default;

    void accept(INodeVisitor &visitor) override {
        visitor.visitAddNode(this);
    }
};
