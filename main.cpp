#include <iostream>
#include <cstdio>

#include <MoveNode.hpp>
#include <PrintingVisitor.hpp>

extern "C"
{
extern int yyparse(void);
extern FILE *yyin;
extern unsigned int lineNumber;
}

int main() {
    INode *node1 = new MoveNode(-3);
    INode *node2 = new MoveNode(5);
    node1->next = std::unique_ptr<INode>(node2);

    PrintingVisitor visitor;

    node1->accept(visitor);

    yyin = fopen("../test/hello_world.bf", "r");
    if (!yyparse()) {
        std::cout << "Parsed " << lineNumber << " lines of Brainfuck" << std::endl;
        return 0;
    } else {
        return 1;
    }
}
