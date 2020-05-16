#pragma once

#include "NodeVisitor.hpp"
#include <string>

class PrintingVisitor : public NodeVisitor {
    std::ostream *out;

public:
    explicit PrintingVisitor(const std::string &);

    virtual ~PrintingVisitor();

    void visitMoveNode(MoveNode *node) override;

    void visitAddNode(AddNode *node) override;

    void visitInputNode(InputNode *node) override;

    void visitOutputNode(OutputNode *node) override;

    void visitLoopNode(LoopNode *node) override;

    void visitSetNode(SetNode *node) override;
};
