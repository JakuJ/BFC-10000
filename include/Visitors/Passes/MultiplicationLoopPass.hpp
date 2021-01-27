#pragma once

#include <vector>
#include <optional>

#include <Nodes/INode.hpp>
#include "OptimizationPass.hpp"

class MultiplicationLoopPass : public OptimizationPass {
    static std::optional<std::vector<AddMultipleNode *>> trySimplify(const std::vector<INode *> &);

public:
    void visitLoopNode(LoopNode *node) override;

    void visitSequenceNode(SequenceNode *node) override;

    void dumpStats() const override;
};
