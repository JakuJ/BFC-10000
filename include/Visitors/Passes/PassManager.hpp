#pragma once

#include <vector>
#include <memory>
#include <Nodes/INode.hpp>
#include "OptimizationPass.hpp"


class PassManager {
    std::vector<std::unique_ptr<OptimizationPass>> passes;

public:
    void addPass(OptimizationPass *);

    void runAll(INode *ast);

    void dumpStats() const;
};
