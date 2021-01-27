#pragma once

#include "Visitors/NodeVisitor.hpp"

// Abstract base class for all AST nodes
struct INode {
    char symbol;

    int value;

    INode() : symbol('?'), value(0) {}

    virtual ~INode() = default;

    virtual void accept(NodeVisitor &) = 0;
};
