#pragma once

class MoveNode;

class AddNode;

class InputNode;

class OutputNode;

class LoopNode;

class SequenceNode;

struct INodeVisitor {
    virtual void visitMoveNode(MoveNode *) {}

    virtual void visitAddNode(AddNode *) {}

    virtual void visitInputNode(InputNode *) {}

    virtual void visitOutputNode(OutputNode *) {}

    virtual void visitLoopNode(LoopNode *) {}

    virtual void visitSequenceNode(SequenceNode *) {}
};
