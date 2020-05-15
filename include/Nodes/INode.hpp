#pragma once

#include "Visitors/INodeVisitor.hpp"

struct INode {
    char symbol;

    int value;

    INode() : symbol('?'), value(0) {}

    virtual ~INode() = default;

    virtual void accept(INodeVisitor &) = 0;
};
