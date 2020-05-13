#pragma once

#include <INode.hpp>

class OutputNode : public INode {
public:
    ~OutputNode() override = default;

    void accept(INodeVisitor &visitor) override {
        visitor.visitOutputNode(this);
    }
};
