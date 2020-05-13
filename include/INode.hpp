#pragma once

#include <memory>
#include <utility>
#include "INodeVisitor.hpp"

struct INode {
    std::unique_ptr<INode> next;

    explicit INode() : next(nullptr) {}

    virtual ~INode() = default;

    virtual void accept(INodeVisitor &) = 0;
};
