#include <iostream>
#include <cstdio>

#include <parser.hpp>
#include <AstBuilder.hpp>
#include <Visitors/LlvmVisitor.hpp>

extern FILE *yyin;
extern unsigned int lineNumber;
extern ASTBuilder builder;

void usage() {
    std::cout << "Usage: compiler <source file>" << std::endl;
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

    LLVMVisitor codegen;
    ast->accept(codegen);

    codegen.dumpCode();
    return codegen.compile();
}
