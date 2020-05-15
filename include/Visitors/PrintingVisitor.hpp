#pragma once

#include "INodeVisitor.hpp"
#include <string>

class PrintingVisitor : public INodeVisitor {
    std::ostream *out;

public:
    explicit PrintingVisitor(const std::string &);

    virtual ~PrintingVisitor();

    void visitMoveNode(MoveNode *node) override;

    void visitAddNode(AddNode *node) override;

    void visitInputNode(InputNode *node) override;

    void visitOutputNode(OutputNode *node) override;

    void visitLoopNode(LoopNode *node) override;

    void visitSequenceNode(SequenceNode *node) override;

    void visitSetNode(SetNode *node) override;
};
