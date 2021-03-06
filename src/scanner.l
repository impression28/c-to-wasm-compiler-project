%option outfile="scanner.cpp"
%option noyywrap
%option nounput
%option noinput
%option yylineno

%{
#include <cstdio>
#include "parsing.hpp"
#include "strtable.hpp"
#include "symtable.hpp"

void yyerror (char const* s) {
    fprintf(stderr, "SYNTAX ERROR (%d): %s\n", yylineno, s);
    exit(EXIT_FAILURE);
}

// Stores current column
int yycolumn = 1;
// Stores line number of the last token
int last_line = 1;

/* Handle token positions */
#define YY_USER_ACTION \
    { \
        yylloc.first_line = last_line; \
        yylloc.last_line = last_line = yylineno; \
        yylloc.first_column = yycolumn; \
        yycolumn += yyleng; \
        yylloc.last_column = yycolumn; \
    }
%}

digit        0|([1-9]+[0-9]*)
alpha        [a-zA-Z]+
alphanum     [a-zA-Z0-9]+
real         (0|([1-9]+[0-9]*))"."[0-9]+

%%

    /* Trata espacos em branco */
\n+          { yycolumn = 1; }
[ \t\r]+     { }

    /* Ignora comentarios */
"//".*(\\(\r\n|\n).*)*\n    { yycolumn = 1; }
"/*"[^*/]*"*/"              { }

    /* Ignora diretivas para o preprocessador */
^[ \t\n\r]*"#".*\n        { yycolumn = 1; }

auto                    { HANDLE_TOKEN(AUTO, {}); }
break                   { HANDLE_TOKEN(BREAK, {}); }
case                    { HANDLE_TOKEN(CASE, {}); }
char                    { HANDLE_TOKEN(CHAR, {}); }
const                   { HANDLE_TOKEN(CONST, {}); }
continue                { HANDLE_TOKEN(CONTINUE, {}); }
default                 { HANDLE_TOKEN(DEFAULT, {}); }
do                      { HANDLE_TOKEN(DO, {}); }
double                  { HANDLE_TOKEN(DOUBLE, {}); }
else                    { HANDLE_TOKEN(ELSE, {}); }
enum                    { HANDLE_TOKEN(ENUM, {}); }
extern                  { HANDLE_TOKEN(EXTERN, {}); }
float                   { HANDLE_TOKEN(FLOAT, {}); }
for                     { HANDLE_TOKEN(FOR, {}); }
goto                    { HANDLE_TOKEN(GOTO, {}); }
if                      { HANDLE_TOKEN(IF, {}); }
inline                  { HANDLE_TOKEN(INLINE, {}); }
int                     { HANDLE_TOKEN(INT, {}); }
long                    { HANDLE_TOKEN(LONG, {}); }
register                { HANDLE_TOKEN(REGISTER, {}); }
return                  { HANDLE_TOKEN(RETURN, {}); }
short                   { HANDLE_TOKEN(SHORT, {}); }
signed                  { HANDLE_TOKEN(SIGNED, {}); }
sizeof                  { HANDLE_TOKEN(SIZEOF, {}); }
static                  { HANDLE_TOKEN(STATIC, {}); }
struct                  { HANDLE_TOKEN(STRUCT, {}); }
switch                  { HANDLE_TOKEN(SWITCH, {}); }
typedef                 { HANDLE_TOKEN(TYPEDEF, {}); }
union                   { HANDLE_TOKEN(UNION, {}); }
unsigned                { HANDLE_TOKEN(UNSIGNED, {}); }
void                    { HANDLE_TOKEN(VOID, {}); }
volatile                { HANDLE_TOKEN(VOLATILE, {}); }
while                   { HANDLE_TOKEN(WHILE, {}); }

"+"                     { HANDLE_TOKEN(PLUS, {}); }
"-"                     { HANDLE_TOKEN(MINUS, {}); }
"*"                     { HANDLE_TOKEN(STAR, {}); }
"/"                     { HANDLE_TOKEN(OVER, {}); }
"%"                     { HANDLE_TOKEN(PERC, {}); }
"++"                    { HANDLE_TOKEN(PLUSPLUS, {}); }
"--"                    { HANDLE_TOKEN(MINUSMINUS, {}); }
"=="                    { HANDLE_TOKEN(EQ, {}); }
"!="                    { HANDLE_TOKEN(NEQ, {}); }
"<"                     { HANDLE_TOKEN(LT, {}); }
">"                     { HANDLE_TOKEN(GT, {}); }
"<="                    { HANDLE_TOKEN(LET, {}); }
">="                    { HANDLE_TOKEN(GET, {}); }
"&&"                    { HANDLE_TOKEN(AND, {}); }
"||"                    { HANDLE_TOKEN(OR, {}); }
"&"                     { HANDLE_TOKEN(AMPER, {}); }
"|"                     { HANDLE_TOKEN(BTOR, {}); }
"^"                     { HANDLE_TOKEN(BTXOR, {}); }
"!"                     { HANDLE_TOKEN(NOT, {}); }
"~"                     { HANDLE_TOKEN(BTNOT, {}); }
"<<"                    { HANDLE_TOKEN(LEFT, {}); }
">>"                    { HANDLE_TOKEN(RIGHT, {}); }
"("                     { HANDLE_TOKEN(LPAR, {}); }
")"                     { HANDLE_TOKEN(RPAR, {}); }
"["                     { HANDLE_TOKEN(LB, {}); }
"]"                     { HANDLE_TOKEN(RB, {}); }
"{"                     { HANDLE_TOKEN(LCB, {}); }
"}"                     { HANDLE_TOKEN(RCB, {}); }
"?"                     { HANDLE_TOKEN(QUEST, {}); }
";"                     { HANDLE_TOKEN(SEMI, {}); }
":"                     { HANDLE_TOKEN(COLON, {}); }
","                     { HANDLE_TOKEN(COMMA, {}); }
"."                     { HANDLE_TOKEN(DOT, {}); }
"..."                   { HANDLE_TOKEN(ELLIPSIS, {}); }
"->"                    { HANDLE_TOKEN(ARROW, {}); }
"="                     { HANDLE_TOKEN(ASSIGN, {}); }
"*="                    { HANDLE_TOKEN(STARASS, {}); }
"/="                    { HANDLE_TOKEN(OVERASS, {}); }
"%="                    { HANDLE_TOKEN(MODASS, {}); }
"+="                    { HANDLE_TOKEN(PLUSASS, {}); }
"-="                    { HANDLE_TOKEN(MINASS, {}); }
"<<="                   { HANDLE_TOKEN(SLASS, {}); }
">>="                   { HANDLE_TOKEN(SRASS, {}); }
"&="                    { HANDLE_TOKEN(ANDASS, {}); }
"^="                    { HANDLE_TOKEN(XORASS, {}); }
"|="                    { HANDLE_TOKEN(ORASS, {}); }

{digit}                 { HANDLE_TOKEN(INT_VAL, { yylval.INT_VAL = atoll(yytext); }); }
{real}                  { HANDLE_TOKEN(REAL_VAL, { yylval.REAL_VAL = atof(yytext); }); }
    /* TODO corrigir e adicionar outras representações */

\'([^\r\n\']|"\\\'")*\'     { HANDLE_TOKEN(CHAR_VAL, {yylval.CHAR_VAL = (int)yytext[1]; }); }
\"([^\r\n\"]|"\\\"")*\"     { HANDLE_TOKEN(STR_VAL, { yylval.STR_VAL = strtb::add(yytext); }); }

[a-zA-Z_][0-9a-zA-Z_]*  { HANDLE_TOKEN_ID({}); }

.                       {   fprintf(stderr, "LEXICAL ERROR: Unknown symbol %s\n", yytext);
                            exit(EXIT_FAILURE); }
