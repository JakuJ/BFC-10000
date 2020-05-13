%{

#include "parser.h"

int yylex();

%}

%option noyywrap

%%

\.  { return WRITE; }
\,  { return READ; }
\+  { return PLUS; }
\-  { return MINUS; }
\[  { return OPEN; }
\]  { return CLOSE; }
\<  { return LEFT; }
\>  { return RIGHT; }
\n  { return NEWLINE; }
.   { }

%%