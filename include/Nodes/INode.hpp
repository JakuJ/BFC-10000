#pragma once

#include "Visitors/NodeVisitor.hpp"

struct INode {
    char symbol;

    int value;

    INode() : symbol('?'), value(0) {}

    virtual ~INode() = default;

    virtual void accept(NodeVisitor &) = 0;
};
