#pragma once

#include "Visitors/Passes/OptimizationPass.hpp"

class FoldingPass : public OptimizationPass {
    long symbols = 0;

    void visitLoopNode(LoopNode *node) override;

    void visitSequenceNode(SequenceNode *node) override;

public:
    void dumpStats() const override;
};
