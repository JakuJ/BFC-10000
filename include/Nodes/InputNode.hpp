#pragma once

#include "IOperation.hpp"

class InputNode : public IOperation {
public:
    InputNode() : IOperation(',') {}

    ~InputNode() override = default;

    void accept(INodeVisitor &visitor) override {
        visitor.visitInputNode(this);
    }
};
