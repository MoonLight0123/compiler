
%{
    #include <stdio.h>
    #include <stdlib.h>
    #ifndef YYSTYPE
    #define YYSTYPE double
    #endif
    int yylex ();
    extern int yyparse();
    FILE* yyin ;
    void yyerror(const char* s );
%}

%token NUMBER
%left '+' '-'
%left '*' '/'
%right UMINUS

%%


lines : lines expr ';' { printf("%f\n", $2); }
| lines ';'
|
;

expr : expr '+' expr { $$ = $1 + $3; }
| expr '-' expr { $$ = $1 - $3; }
| expr '*' expr { $$ = $1 * $3; }
| expr '/' expr { $$ = $1 / $3; }
| '(' expr ')' { $$ = $2; }
| '-' expr %prec UMINUS { $$ = -$2; }
| NUMBER { $$ = $1; }
;

%%

// programs section

int yylex()
{
// place your token retrieving code here
    char c;
    while(1)
    {
        c=getchar();
        if('0'<=c&&c<='9')
        {
            yylval=0;
            while('0'<=c&&c<='9')
            {
                yylval=yylval*10+c-'0';
                c=getchar();
            }
            ungetc(c,stdin);
            return NUMBER;
        }
        else if(c!=' '&&c!='\t'&&c!='\n')
            return c;
    }
}

int main(void)
{
    yyin = stdin ;
    do {
    yyparse();
    } 
    while (! feof (yyin));
    return 0;
}
void yyerror(const char* s) {
    fprintf (stderr , "Parse error : %s\n", s );
    exit (1);
}

