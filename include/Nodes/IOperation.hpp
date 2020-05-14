#pragma once

#include "INode.hpp"

struct IOperation : public INode {
    char symbol;

    explicit IOperation(char symbol) : symbol(symbol) {}

    ~IOperation() override = default;
};
