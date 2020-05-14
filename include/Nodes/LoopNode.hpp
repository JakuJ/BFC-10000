#pragma once

#include <memory>
#include <utility>

#include <Nodes/INode.hpp>
#include <Nodes/SequenceNode.hpp>

struct LoopNode : public INode {
    SequenceNode *inside;

    LoopNode() : inside(new SequenceNode()) {}

    ~LoopNode() override { delete inside; };

    void accept(INodeVisitor &visitor) override {
        visitor.visitLoopNode(this);
    }
};
