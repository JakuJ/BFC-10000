#pragma once

#include <Nodes/INode.hpp>

class OutputNode : public INode {
public:
    OutputNode() {
        symbol = '.';
    }

    ~OutputNode() override = default;

    void accept(INodeVisitor &visitor) override {
        visitor.visitOutputNode(this);
    }
};
