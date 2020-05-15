#include <getopt.h>
#include <iostream>
#include <cstdio>

#include <parser.hpp>
#include <AstBuilder.hpp>
#include <Visitors/PrintingVisitor.hpp>
#include <Visitors/LlvmVisitor.hpp>
#include <Visitors/Passes/ContractingPass.hpp>
#include <Visitors/Passes/SetZeroPass.hpp>
#include <Visitors/Passes/UnreachableLoopPass.hpp>
#include <Visitors/Passes/PassManager.hpp>

extern FILE *yyin;
extern unsigned int lineNumber;
extern ASTBuilder builder;

void usage() {
    std::cerr << "Usage: compiler [-hvbOS] [-o <output file>] <source file>" << std::endl;
    std::cerr << "\t-h\t Print this help" << std::endl;
    std::cerr << "\t-v\t Verbose output about optimization passes" << std::endl;
    std::cerr << "\t-b\t Emit optimized Brainfuck IR" << std::endl;
    std::cerr << "\t-S\t Emit LLVM IR" << std::endl;
    std::cerr << "\t-O\t Optimize LLVM IR" << std::endl;
}

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
    bool emit_llvm = false, emit_bf_ir = false, optimize = false, verbose = false;

    std::string output_file;
    bool custom_of = false;

    int opt;

    while ((opt = getopt(argc, argv, "hvbOSo:")) != -1) {
        switch (opt) {
            case 'b':
                emit_bf_ir = true;
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

    const char *source_file;
    if (optind < argc) {
        source_file = argv[optind];
        if (!custom_of) {
            output_file = remove_extension(source_file);
        }
    } else {
        std::cerr << "No input files specified" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cerr << "Parsing source code... ";

    yyin = fopen(source_file, "r");
    if (yyparse()) {
        return EXIT_FAILURE;
    }

    std::cerr << lineNumber << " lines done\nOptimizing AST... ";

    auto ast = builder.getAST();

    PassManager passManager;

    passManager.addPass(new ContractingPass());
    passManager.addPass(new UnreachableLoopPass());
    passManager.addPass(new SetZeroPass());

    passManager.runAll(ast.get());

    if (verbose) {
        std::cerr << std::endl;
        passManager.dumpStats();
    }

    if (emit_bf_ir) {
        std::cerr << "done\nDumping BF IR... ";

        if (!custom_of) {
            output_file += ".txt";
        }

        PrintingVisitor printer(output_file);
        ast->accept(printer);

        std::cerr << "done\nAll done" << std::endl;
        return 0;
    }

    std::cerr << "done\nGenerating LLVM IR code... ";

    LLVMVisitor codegen;
    ast->accept(codegen);

    codegen.finalize();

    if (optimize) {
        std::cerr << "done\nOptimizing IR... ";
        codegen.optimize();
    }

    if (emit_llvm) {
        std::cerr << "done\nDumping LLVM IR... ";
        if (!custom_of) {
            output_file += ".ll";
        }
        codegen.dumpCode(output_file);
        std::cerr << "done\nAll done" << std::endl;
        return 0;
    }

    std::cerr << "done\nCompiling to object file... ";

    if (!custom_of) {
        output_file += ".o";
    }
    int ret_code = codegen.compile(output_file);

    std::cerr << "done\nAll done" << std::endl;

    return ret_code;
}
