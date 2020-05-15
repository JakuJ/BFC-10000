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

    std::cerr << "Parsing source code... ";

    yyin = fopen(argv[1], "r");
    if (yyparse()) {
        return EXIT_FAILURE;
    }

    std::cerr << lineNumber << " lines done\nGenerating code... ";

    auto ast = builder.getAST();

    LLVMVisitor codegen;
    ast->accept(codegen);

    std::cerr << "done\nOptimizing... ";

    codegen.finalize();

    std::cerr << "done\nCompiling to object file... ";

    int retcode = codegen.compile();

    std::cerr << "done\nAll done" << std::endl;

    return retcode;
}
