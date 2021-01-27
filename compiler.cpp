#include <getopt.h>
#include <iostream>
#include <cstdio>

#include <parser.hpp>
#include <AstBuilder.hpp>
#include <Visitors/PrintingVisitor.hpp>
#include <Visitors/LlvmVisitor.hpp>
#include <Visitors/Passes/FoldingPass.hpp>
#include <Visitors/Passes/ClearCellPass.hpp>
#include <Visitors/Passes/UnreachableLoopPass.hpp>
#include <Visitors/Passes/PassManager.hpp>
#include <Visitors/Passes/MultiplicationLoopPass.hpp>

// FLEX/BISON externals
extern FILE *yyin;
extern unsigned int lineNumber;
extern ASTBuilder builder;

void usage() {
    std::cerr << "Usage: compiler [-hvAOS] [-o <output file>] <source file>" << std::endl;
    std::cerr << "\t-h\t Print this help" << std::endl;
    std::cerr << "\t-v\t Verbose mode, print optimization passes' stats" << std::endl;
    std::cerr << "\t-A\t Emit the optimized AST (for debugging purposes)" << std::endl;
    std::cerr << "\t-S\t Emit LLVM IR" << std::endl;
    std::cerr << "\t-O\t Optimize LLVM IR" << std::endl;
}

// Removes extension from a file path
std::string remove_extension(const char *path) {
    auto filename = std::string(path);

    size_t last_slash = filename.find_last_of('/');
    if (last_slash != std::string::npos) {
        filename = filename.substr(last_slash + 1);
    }

    size_t lastdot = filename.find_last_of('.');
    if (lastdot != std::string::npos) {
        filename = filename.substr(0, lastdot);
    }
    return filename;
}

int main(int argc, char *argv[]) {
    bool emit_llvm = false, emit_ast = false, optimize = false, verbose = false, custom_of = false;
    std::string output_file;

    // Parse program arguments
    int opt;
    while ((opt = getopt(argc, argv, "hvAOSo:")) != -1) {
        switch (opt) {
            case 'A':
                emit_ast = true;
                break;
            case 'v':
                verbose = true;
                break;
            case 'O':
                optimize = true;
                break;
            case 'S':
                emit_llvm = true;
                break;
            case 'o':
                output_file = std::string(optarg);
                custom_of = true;
                break;
            case ':':
                std::cerr << "Option -" << optopt << " needs a value" << std::endl;
                usage();
                exit(EXIT_FAILURE);
            case '?':
                std::cerr << "Unknown option:" << optopt << std::endl;
            case 'h':
            default:
                usage();
                exit(EXIT_FAILURE);
        }
    }

    // Parse source file path
    const char *source_file;
    if (optind < argc) {
        source_file = argv[optind];

        // If no custom output file specified, save to this name with different extension
        if (!custom_of) {
            output_file = remove_extension(source_file);
        }
    } else {
        std::cerr << "No input files specified" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Parse source
    std::cerr << "Parsing source code... ";

    yyin = fopen(source_file, "r");
    if (yyparse()) {
        return EXIT_FAILURE;
    }

    std::cerr << lineNumber << " lines done\nOptimizing AST... ";

    // Run optimization passes
    auto ast = builder.getAST();

    PassManager passManager;

    passManager.addPass(new FoldingPass());
    passManager.addPass(new UnreachableLoopPass());
    passManager.addPass(new ClearCellPass());
    passManager.addPass(new MultiplicationLoopPass());

    passManager.runAll(ast.get());

    // Optional: dump optimization statistics
    if (verbose) {
        std::cerr << std::endl;
        passManager.dumpStats();
    }

    // Optional: dump Brainfuck IR
    if (emit_ast) {
        std::cerr << "done\nDumping BF IR... ";

        if (!custom_of) {
            output_file += ".txt";
        }

        PrintingVisitor printer(output_file);
        ast->accept(printer);

        std::cerr << "done\nAll done" << std::endl;
        return 0;
    }

    // Compile to LLVM IR
    std::cerr << "done\nGenerating LLVM IR code... ";

    LLVMVisitor codegen;
    ast->accept(codegen);

    codegen.finalize();

    // Optional: Run LLVM optimizations
    if (optimize) {
        std::cerr << "done\nOptimizing IR... ";
        codegen.optimize();
    }

    // Optional: dump LLVM IR
    if (emit_llvm) {
        std::cerr << "done\nDumping LLVM IR... ";
        if (!custom_of) {
            output_file += ".ll";
        }
        codegen.dumpCode(output_file);
        std::cerr << "done\nAll done" << std::endl;
        return 0;
    }

    // Compile to binary
    std::cerr << "done\nCompiling to object file... ";

    if (!custom_of) {
        output_file += ".o";
    }
    int ret_code = codegen.compile(output_file);

    std::cerr << "done\nAll done" << std::endl;

    return ret_code;
}
