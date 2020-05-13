%{

#include <stdio.h>

extern int yylex();

unsigned int lineNumber = 1;

void yyerror (char const *s) {
   // std::cerr << "Parsing error at line " << lineNumber << ": " << s << std::endl;
   printf("Parsing error at line %d: %s\n", lineNumber, s);
}

%}

%define parse.error verbose

%token NEWLINE
%token LEFT RIGHT
%token PLUS MINUS
%token OPEN CLOSE
%token READ WRITE

%%

ROOT    : TEXT ;

TEXT    : TEXT STRING | %empty ;

STRING  : UNOS | PARENS ;

UNOS    : UNOS UNO | %empty ;

PARENS  : OPEN INSIDE ;

INSIDE  : TEXT CLOSE
        | CLOSE { yyerror("Potential infinite loop []"); }
        ;

UNO     : NEWLINE { ++lineNumber; }
        | LEFT
        | RIGHT
        | PLUS
        | MINUS
        | READ
        | WRITE
        ;

%%

