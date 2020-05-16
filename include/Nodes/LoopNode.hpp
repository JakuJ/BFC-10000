#pragma once

#include <memory>
#include <utility>

#include <Nodes/INode.hpp>
#include <Nodes/SequenceNode.hpp>

struct LoopNode : public INode {
    SequenceNode *inside;

    LoopNode() : inside(new SequenceNode()) {
        symbol = '[';
    }

    ~LoopNode() override { delete inside; };

    void accept(NodeVisitor &visitor) override {
        visitor.visitLoopNode(this);
    }
};
