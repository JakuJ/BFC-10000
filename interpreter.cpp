#include <iostream>
#include <cstdio>

#include <parser.hpp>
#include <AstBuilder.hpp>
#include <Visitors/InterpretingVisitor.hpp>
#include <Visitors/Passes/ContractingPass.hpp>
#include <Visitors/Passes/PassManager.hpp>
#include <Visitors/Passes/UnreachableLoopPass.hpp>
#include <Visitors/Passes/SetZeroPass.hpp>
#include <Visitors/Passes/MultLoopPass.hpp>

// FLEX/BISON externals
extern FILE *yyin;
extern unsigned int lineNumber;
extern ASTBuilder builder;

void usage() {
    std::cout << "Usage: interpreter <source file>" << std::endl;
}

int main(int argc, char *argv[]) {
    // Parse program arguments
    if (argc != 2) {
        usage();
        return EXIT_FAILURE;
    }

    // Parse source file
    yyin = fopen(argv[1], "r");
    if (yyparse()) {
        return EXIT_FAILURE;
    }

    std::cerr << "Parsed " << lineNumber << " lines of Brainfuck" << std::endl;

    // Run optimization passes
    auto ast = builder.getAST();

    PassManager passManager;

    passManager.addPass(new ContractingPass());
    passManager.addPass(new UnreachableLoopPass());
    passManager.addPass(new SetZeroPass());
    passManager.addPass(new MultLoopPass());

    passManager.runAll(ast.get());

    // Interpret the program
    InterpretingVisitor interpreter;
    ast->accept(interpreter);

    return 0;
}
