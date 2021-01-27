#pragma once

#include "Visitors/Passes/OptimizationPass.hpp"

class AssignmentPass : public OptimizationPass {
    void visitLoopNode(LoopNode *node) override;

    void visitSequenceNode(SequenceNode *node) override;

public:
    void dumpStats() const override;
};
