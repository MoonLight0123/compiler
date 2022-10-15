
%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #ifndef YYSTYPE
    #define YYSTYPE char*
    #endif
    char idStr[50];
    char numStr[50];
    int yylex ();
    extern int yyparse();
    FILE* yyin ;
    void yyerror(const char* s );
%}

%token NUMBER
%token ID
%left '+' '-'
%left '*' '/'
%right UMINUS

%%


lines : lines expr ';' { printf("%s\n", $2); }
| lines ';'
|
;

expr : expr '+' expr { $$=(char*)malloc(50*sizeof(char));strcpy($$,$1);
                        strcat($$,$3);strcat($$,"+ "); }
| expr '-' expr { $$=(char*)malloc(50*sizeof(char));strcpy($$,$1);
                        strcat($$,$3);strcat($$,"- "); }
| expr '*' expr { $$=(char*)malloc(50*sizeof(char));strcpy($$,$1);
                        strcat($$,$3);strcat($$,"* "); }
| expr '/' expr { $$=(char*)malloc(50*sizeof(char));strcpy($$,$1);
                        strcat($$,$3);strcat($$,"/ "); }
| '(' expr ')' { $$=(char*)malloc(50*sizeof(char));strcpy($$,$2);
                        strcat($$," "); }
| '-' expr %prec UMINUS { $$=(char*)malloc(50*sizeof(char));strcpy($$," -");
                        strcat($$,$2); }
| NUMBER { $$=(char*)malloc(50*sizeof(char));strcpy($$,$1);
            strcat($$," "); }
| ID    { $$=(char*)malloc(50*sizeof(char));strcpy($$,$1);
            strcat($$," "); }
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
            int t=0;
            while('0'<=c&&c<='9')
            {
                numStr[t++]=c;
                c=getchar();
            }
            numStr[t]='\0';
            yylval=numStr;
            ungetc(c,stdin);
            return NUMBER;
        }
        else if((c>='a'&&c<='z')||(c>='A'&&c<='Z')||c=='_')
        {
            int t=0;
            while((c>='a'&&c<='z')||(c>='A'&&c<='Z')||c=='_'||('0'<=c&&c<='9'))
            {
                idStr[t++]=c;
                c=getchar();
            }
            idStr[t]='\0';
            yylval=idStr;
            ungetc(c,stdin);
            return ID;
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

