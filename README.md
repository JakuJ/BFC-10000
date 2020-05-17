# BFC-10000

An optimizing Brainfuck compiler and interpreter. Uses `flex` and `bison` for lexing and parsing the source code.
LLVM is used as the code generation front-end, using a set of IR optimization passes before compiling to x86-64 object files. 

# Requirements

- CMake
- LLVM
- clang++
- flex
- bison >= 3.6

# Building

Make an out of tree build with CMake.

```sh
mkdir build
cd build
cmake ..
make
```

This creates two executables - the compiler and the interpreter.

# Usage

```sh
compiler [-hvAOS] [-o <output file>] <source file>
	-h	 Print this help
	-v	 Verbose mode, print optimization passes' stats
	-A	 Emit the optimized AST (for debugging purposes)
	-S	 Emit LLVM IR
	-O	 Optimize LLVM IR
```

The interpreter has a simpler interface, taking the source file as its sole argument.<br>

The compiler creates object files which have to be linked in order to create an executable (`clang <object file>` will do).

# Optimizations

A range of optimization are employed in order to simplify the code and significantly reduce the number of operations required:

Contracting adjacent instructions: `+++>>-----++--` becomes:
```c
tape[index] += 3;
index += 2;
tape[index] -= 5;
```

The `[-]` loop, a common idiom used for clearing the cell, is simplified to `tape[index] = 0`.

Unreachable loops (such that occur immediately after another loop) are eliminated.

Multiplication loops are simplified to a few copying operations: `[->++<<<--->>]` becomes:

```c
tape[index + 1] += 2 * tape[index];
tape[index - 2] -= 3 * tape[index];
tape[index] = 0;
```

# Example sources

[A Mandelbrot set viewer](https://raw.githubusercontent.com/sci4me/JBFJit/master/progs/mand.bf) by Erik Bosman

[some brainfuck fluff](http://www.hevanet.com/cristofd/brainfuck/) by Daniel Cristofani <br>
A rich collection of BF programs.

[Towers of Hanoi](http://www.clifford.at/bfcpu/hanoi.html) by Clifford Wolf<br>
This one comes with a nice ASCII visuals, but your terminal must be able to understand vt100 escape codes. Also, use the interpreter, and not the compiler - the compiled version is way too fast for the animation. Ifit's still too fast, you might want to disable some AST optimizations in `interpreter.cpp`.

[Lost Kingdom](https://jonripley.com/i-fiction/games/LostKingdomBF.html) by Jon Ripley<br>
A text adventure game written in nearly 30k lines of BF code. You might want to turn off LLVM IR optimizations for this one (compile without the `-O` flag). Even then the compilation might take a few minutes.
