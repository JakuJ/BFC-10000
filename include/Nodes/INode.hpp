#pragma once

#include "Visitors/INodeVisitor.hpp"

struct INode {
    virtual ~INode() = default;

    virtual void accept(INodeVisitor &) = 0;
};
