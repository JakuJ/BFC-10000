#pragma once

#include <Visitors/INodeVisitor.hpp>

class OptimizationPass : public INodeVisitor {
protected:
    int hits = 0;

public:
    virtual ~OptimizationPass() = default;

    virtual void dumpStats() const = 0;
};
