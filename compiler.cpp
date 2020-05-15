#include <iostream>
#include <cstdio>

#include <parser.hpp>
#include <AstBuilder.hpp>
#include <Visitors/LlvmVisitor.hpp>
#include <Visitors/FoldingVisitor.hpp>
#include <Visitors/PrintingVisitor.hpp>

extern FILE *yyin;
extern unsigned int lineNumber;
extern ASTBuilder builder;

void usage() {
    std::cout << "Usage: compiler <source file>" << std::endl;
}

int main(int argc, char *argv[]) {

    std::cerr << "Parsing source code... ";

    yyin = fopen(argv[1], "r");
    if (yyparse()) {
        return EXIT_FAILURE;
    }

    std::cerr << lineNumber << " lines done\nOptimizing AST... ";

    auto ast = builder.getAST();

    FoldingVisitor folder;
    ast->accept(folder);

//    PrintingVisitor printer;
//    ast->accept(printer);

    std::cerr << " done\nGenerating LLVM IR code... ";

    LLVMVisitor codegen;
    ast->accept(codegen);

    std::cerr << "done\nOptimizing IR... ";

    codegen.finalize();

//    codegen.dumpCode();

    std::cerr << "done\nCompiling to object file... ";

    int retcode = codegen.compile();

    std::cerr << "done\nAll done" << std::endl;

    return retcode;
}
