#pragma once

#include <Visitors/NodeVisitor.hpp>

class OptimizationPass : public NodeVisitor {
protected:
    long hits = 0;

public:
    virtual ~OptimizationPass() = default;

    virtual void dumpStats() const = 0;
};
