#pragma once

class MoveNode;

class AddNode;

class InputNode;

class OutputNode;

class LoopNode;

struct INodeVisitor {
    virtual void visitMoveNode(MoveNode *) {}

    virtual void visitAddNode(AddNode *) {}

    virtual void visitInputNode(InputNode *) {}

    virtual void visitOutputNode(OutputNode *) {}

    virtual void visitLoopNode(LoopNode *) {}
};
