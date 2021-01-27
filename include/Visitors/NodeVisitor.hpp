#pragma once

struct MoveNode;

struct AddNode;

struct AddMultipleNode;

class InputNode;

class OutputNode;

struct LoopNode;

struct SequenceNode;

struct NodeVisitor {
    virtual void visitMoveNode(MoveNode *) {}

    virtual void visitAddNode(AddNode *) {}

    virtual void visitSetNode(AddMultipleNode *) {}

    virtual void visitInputNode(InputNode *) {}

    virtual void visitOutputNode(OutputNode *) {}

    virtual void visitLoopNode(LoopNode *) {}

    virtual void visitSequenceNode(SequenceNode *node);
};
