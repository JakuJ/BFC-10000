#pragma once

#include <memory>
#include <utility>

#include <INode.hpp>

struct LoopNode : public INode {
    std::unique_ptr<INode> inside;

    explicit LoopNode(std::unique_ptr<INode> inside) : inside(std::move(inside)) {}

    ~LoopNode() override = default;

    void accept(INodeVisitor &visitor) override {
        visitor.visitLoopNode(this);
    }
};
