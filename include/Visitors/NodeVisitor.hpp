#pragma once

struct MoveNode;

struct AddNode;

struct AddMultipleNode;

class InputNode;

class OutputNode;

struct LoopNode;

struct SequenceNode;

struct AssignmentNode;

struct MemsetNode;

struct NodeVisitor {
    virtual void visitMoveNode(MoveNode *) {}

    virtual void visitAddNode(AddNode *) {}

    virtual void visitAddMultipleNode(AddMultipleNode *) {}

    virtual void visitInputNode(InputNode *) {}

    virtual void visitOutputNode(OutputNode *) {}

    virtual void visitLoopNode(LoopNode *) {}

    virtual void visitAssignmentNode(AssignmentNode *) {}

    virtual void visitMemsetNode(MemsetNode *node) {}

    virtual void visitSequenceNode(SequenceNode *node);
};
