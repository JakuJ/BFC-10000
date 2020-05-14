#pragma once

#include <Nodes/IOperation.hpp>

class OutputNode : public IOperation {
public:
    OutputNode() : IOperation('.') {}

    ~OutputNode() override = default;

    void accept(INodeVisitor &visitor) override {
        visitor.visitOutputNode(this);
    }
};
