#include <iostream>
#include <cstdio>

#include <parser.hpp>
#include <AstBuilder.hpp>
#include <Visitors/InterpretingVisitor.hpp>
#include <Visitors/ContractingVisitor.hpp>

extern FILE *yyin;
extern unsigned int lineNumber;
extern ASTBuilder builder;

void usage() {
    std::cout << "Usage: interpreter <source file>" << std::endl;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        usage();
        return EXIT_FAILURE;
    }

    yyin = fopen(argv[1], "r");
    if (yyparse()) {
        return EXIT_FAILURE;
    }

    std::cerr << "Parsed " << lineNumber << " lines of Brainfuck" << std::endl;

    auto ast = builder.getAST();

    ContractingVisitor folder;
    ast->accept(folder);

    InterpretingVisitor interpreter;
    ast->accept(interpreter);

    return 0;
}
