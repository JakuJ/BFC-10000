%{
#include <iostream>

#include <ASTBuilder.hpp>
#include <Nodes/MoveNode.hpp>
#include <Nodes/AddNode.hpp>
#include <Nodes/InputNode.hpp>
#include <Nodes/OutputNode.hpp>

extern "C" int yylex();

unsigned int lineNumber = 1;

void yyerror (char const *s) {
   std::cerr << "Parsing error at line " << lineNumber << ": " << s << std::endl;
}

ASTBuilder builder;

%}

%define parse.error verbose

%token NEWLINE
%token LEFT RIGHT
%token PLUS MINUS
%token OPEN CLOSE
%token READ WRITE

%%

ROOT    : STRING ;

STRING  : STRING OP | %empty ;

OP      : NEWLINE   { ++lineNumber; }
        | LEFT      { builder.addOperation(new MoveNode(-1)); }
        | RIGHT     { builder.addOperation(new MoveNode(1)); }
        | PLUS      { builder.addOperation(new AddNode(1)); }
        | MINUS     { builder.addOperation(new AddNode(-1)); }
        | READ      { builder.addOperation(new InputNode()); }
        | WRITE     { builder.addOperation(new OutputNode()); }
        | OPEN      { builder.beginLoop(); }
        | CLOSE     { builder.endLoop(); }
        ;

%%

