#pragma once

struct MoveNode;

struct AddNode;

class InputNode;

class OutputNode;

struct LoopNode;

struct SequenceNode;

struct INodeVisitor {
    virtual void visitMoveNode(MoveNode *) {}

    virtual void visitAddNode(AddNode *) {}

    virtual void visitInputNode(InputNode *) {}

    virtual void visitOutputNode(OutputNode *) {}

    virtual void visitLoopNode(LoopNode *) {}

    virtual void visitSequenceNode(SequenceNode *) {}
};
