#pragma once

#include "Visitors/Passes/OptimizationPass.hpp"

class ContractingPass : public OptimizationPass {
    void visitLoopNode(LoopNode *node) override;

    void visitSequenceNode(SequenceNode *node) override;

public:
    void dumpStats() const override;
};
