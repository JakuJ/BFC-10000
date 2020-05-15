#pragma once

#include "INode.hpp"

class InputNode : public INode {
public:
    InputNode() {
        symbol = ',';
    }

    void accept(INodeVisitor &visitor) override {
        visitor.visitInputNode(this);
    }
};
