#pragma once

#include "INode.hpp"

class InputNode : public INode {
public:
    InputNode() {
        symbol = ',';
    }

    void accept(NodeVisitor &visitor) override {
        visitor.visitInputNode(this);
    }
};
