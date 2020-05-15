#pragma once

#include "INode.hpp"

class InputNode : public INode {
public:
    InputNode() {
        symbol = ',';
    }

    ~InputNode() override = default;

    void accept(INodeVisitor &visitor) override {
        visitor.visitInputNode(this);
    }
};
