#pragma once

#include "OptimizationPass.hpp"

class UnreachableLoopPass : public OptimizationPass {
public:
    void visitLoopNode(LoopNode *node) override;

    void visitSequenceNode(SequenceNode *node) override;

    void dumpStats() const override;
};
