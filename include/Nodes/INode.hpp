#pragma once

#include "Visitors/INodeVisitor.hpp"

struct INode {
    char symbol;

    explicit INode(char symbol) : symbol(symbol) {}

    virtual ~INode() = default;

    virtual void accept(INodeVisitor &) = 0;
};
