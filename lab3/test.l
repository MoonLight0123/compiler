%option noyywrap
%{
    /*
    * You will need to comment this line in lab5.
    */
    #define ONLY_FOR_LEX
    
    #ifdef ONLY_FOR_LEX
    #else
    #define YYSTYPE void *
    #include "parser.h"
    #endif

    #define YY_NO_UNPUT
    #define YY_NO_INPUT
    #include <string.h>
    #define FileName yyout

    #ifdef ONLY_FOR_LEX
    //#include <ostream.h>
    //#include <fstream.h>
    //using namespace std;
    extern FILE *yyin; 
    extern FILE *yyout;


    int charCount=0;
    
    struct symbolTable
    {
        char name[50];
        int scope;
    }table[50];


    int idCount=-1;
    int curScope;
    int finishedScope;
    int leftBraceScope[100];
    int leftBraceScopePointer;
    int leftBraceNum;

/*
    void DEBUG_FOR_LAB4(std::string s){
        std::string DEBUG_INFO = "\t" + s + "\n";
        //sprintf(DEBUG_INFO,"\t %s \t %d",s,yylineno);
        fputs(DEBUG_INFO.c_str(), yyout);
    }
    */
    #endif
%}



FP ([0-9]+)"."([0-9]+)
NUMBER ([1-9][0-9]*|0)


HEXADECMIAL 0x([1-9][0-9]*|0)
ID [[:alpha:]_][[:alpha:][:digit:]_]*
EOL (\r\n|\n|\r)
WHITE [\t ]


commentBegin "/*"
commentElement .|\n
commentEnd "*/"
%x COMMENT

commentLinebegin "//"
commentLineelement .
commentLineend	 \n
%x COMMENTLINE

%%
"const" {
    #ifdef ONLY_FOR_LEX
        fprintf(FileName,"CONST\tconst");
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;
        fprintf(FileName,"\n");
    #else
        return CONST;
    #endif
}

"int" {
    #ifdef ONLY_FOR_LEX
        fprintf(FileName,"INT\tint");
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;
        fprintf(FileName,"\n");
    #else
        return INT;
    #endif
}
"float" {
    #ifdef ONLY_FOR_LEX
        fprintf(FileName,"FLOAT\tfloat");
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;
        fprintf(FileName,"\n");
    #else
        return FLOAT;
    #endif
}
"double" {
    #ifdef ONLY_FOR_LEX
        fprintf(FileName,"DOUBLE\tdouble");
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;
        fprintf(FileName,"\n");
    #else
        return DOUBLE;
    #endif
}
"void" {
    #ifdef ONLY_FOR_LEX
        fprintf(FileName,"VOID\tvoid");
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;
        fprintf(FileName,"\n");
    #else
        return VOID;
    #endif 
}



"if" {
    #ifdef ONLY_FOR_LEX
        fprintf(FileName,"IF\tif");
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;
        fprintf(FileName,"\n");
    #else
        return IF;
    #endif
};
"else" {
    #ifdef ONLY_FOR_LEX
        fprintf(FileName,"ELSE\telse");
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;
        fprintf(FileName,"\n");
    #else
        return ELSE;
    #endif
};
"while" {
    #ifdef ONLY_FOR_LEX
        fprintf(FileName,"WHILE\twhile");
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;
        fprintf(FileName,"\n");
    #else
        return WHILE;
    #endif
};


"break" {
    #ifdef ONLY_FOR_LEX
        fprintf(FileName,"BREAK\tbreak");
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;
        fprintf(FileName,"\n");
    #else
        return BREAK;
    #endif
};

"continue" {
    #ifdef ONLY_FOR_LEX
        fprintf(FileName,"CONTINUE\tcontinue");
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;
        fprintf(FileName,"\n");
    #else
        return CONTINUE;
    #endif
};



"return" {
    #ifdef ONLY_FOR_LEX
        fprintf(FileName,"RETURN\treturn");
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;
        fprintf(FileName,"\n");
    #else
        return RETURN;
    #endif
}



"=" {
    #ifdef ONLY_FOR_LEX
        fprintf(FileName,"ASSIGN\t=");
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;
        fprintf(FileName,"\n");
    #else
        return ASSIGN;
    #endif
}



"<" {
    #ifdef ONLY_FOR_LEX
        fprintf(FileName,"LESS\t<");
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;
        fprintf(FileName,"\n");
    #else
        return LESS;
    #endif
}

">" {
    #ifdef ONLY_FOR_LEX
        fprintf(FileName,"GREAT\t>");
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;
        fprintf(FileName,"\n");
    #else
        return GREAT;
    #endif
}

"<=" {
    #ifdef ONLY_FOR_LEX
        fprintf(FileName,"LE\t<=");
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;
        fprintf(FileName,"\n");
    #else
        return LE;
    #endif
}

">=" {
    #ifdef ONLY_FOR_LEX
        fprintf(FileName,"GE\t>=");
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;
        fprintf(FileName,"\n");
    #else
        return GE;
    #endif
}

"==" {
    #ifdef ONLY_FOR_LEX
        fprintf(FileName,"EQUAL\t==");
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;
        fprintf(FileName,"\n");
    #else
        return EQUAL;
    #endif
}

"!=" {
    #ifdef ONLY_FOR_LEX
        fprintf(FileName,"UEQUAL\t!=");
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;
        fprintf(FileName,"\n");
    #else
        return UEQUAL;
    #endif
}




"+" {
    #ifdef ONLY_FOR_LEX
        fprintf(FileName,"ADD\t+");
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;
        fprintf(FileName,"\n");
    #else
        return ADD;
    #endif
}


"-" {
    #ifdef ONLY_FOR_LEX
        fprintf(FileName,"SUB\t-");
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;
        fprintf(FileName,"\n");
    #else
        return SUB;
    #endif
}
"*" {
    #ifdef ONLY_FOR_LEX
        fprintf(FileName,"MUL\t*");
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;
        fprintf(FileName,"\n");
    #else
        return MUL;
    #endif
}
"/" {
    #ifdef ONLY_FOR_LEX
        fprintf(FileName,"DIV\t+");
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;
        fprintf(FileName,"\n");
    #else
        return DIV;
    #endif
}
"%" {
    #ifdef ONLY_FOR_LEX
        fprintf(FileName,"MOD\t+");
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;
        fprintf(FileName,"\n");
    #else
        return MOD;
    #endif
}





"&&" {
    #ifdef ONLY_FOR_LEX
        fprintf(FileName,"ANDAND\t&&");
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;
        fprintf(FileName,"\n");
    #else
        return ANDAND;
    #endif
}
"||" {
    #ifdef ONLY_FOR_LEX
        fprintf(FileName,"OROR\t||");
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;
        fprintf(FileName,"\n");
    #else
        return OROR;
    #endif
}
"!" {
    #ifdef ONLY_FOR_LEX
        fprintf(FileName,"NOR\t!");
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;
        fprintf(FileName,"\n");
    #else
        return NOR;
    #endif
}



";" {
    #ifdef ONLY_FOR_LEX
        fprintf(FileName,"SEM\t;");
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;
        fprintf(FileName,"\n");
    #else
        return SEM;
    #endif
}
"(" {
    #ifdef ONLY_FOR_LEX
        fprintf(FileName,"LPAREN\t(");
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;
        fprintf(FileName,"\n");
    #else
        return LPAREN;
    #endif
}
")" {
    #ifdef ONLY_FOR_LEX
        fprintf(FileName,"RPAREN\t)");
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;
        fprintf(FileName,"\n");
    #else
    return RPAREN;
    #endif
}


"{" {
    #ifdef ONLY_FOR_LEX
    	leftBraceNum++;
        leftBraceScopePointer++;
        curScope=leftBraceScope[leftBraceScopePointer]=leftBraceNum;
        
        fprintf(FileName,"LBRACE\t{");
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;
        fprintf(FileName,"\n");
    #else
        return LBRACE;
    #endif
}
"}" {
    #ifdef ONLY_FOR_LEX
        leftBraceScopePointer--;
        curScope=leftBraceScope[leftBraceScopePointer];

        fprintf(FileName,"RBRACE\t}");
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;
        fprintf(FileName,"\n");
    #else
        return RBRACE;
    #endif
}

"[" {
    #ifdef ONLY_FOR_LEX
        fprintf(FileName,"LBRACK\t[");
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;
        fprintf(FileName,"\n");
    #else
        return LBRACK;
    #endif
}
"]" {
    #ifdef ONLY_FOR_LEX

        fprintf(FileName,"RBRACK\t]");
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;
        fprintf(FileName,"\n");
    #else
        return RBRACK;
    #endif
}


{commentBegin} {BEGIN COMMENT;fprintf(FileName,"commentBegin\t");}
<COMMENT>{commentElement} {fprintf(FileName,"%s",yytext);}
<COMMENT>{commentEnd} {BEGIN INITIAL;fprintf(FileName,"commentEnd\n");}

{commentLinebegin} {BEGIN COMMENTLINE;fprintf(FileName,"commentLineBegin\t");}
<COMMENTLINE>{commentLineelement} {fprintf(FileName,"%s",yytext);}
<COMMENTLINE>{commentLineend} {BEGIN INITIAL;fprintf(FileName,"\tcommentLineEnd\n");}


{ID} {
    #ifdef ONLY_FOR_LEX
        fprintf(FileName,"ID\t%s",yytext);
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;

        int flag=0,i;
        for(i=idCount;i>=0;i--)
        {
            
            if(strcmp(yytext,table[i].name)==0&&table[i].scope==curScope)
            {
                flag=1;
                break;
            }
        }

        if(flag==1)
        {
            fprintf(FileName,"\t%d",table[i].scope);
        }
        else
        {
            idCount++;
            strcpy(table[idCount].name,yytext);
            table[idCount].scope=curScope; 
            fprintf(FileName,"\t%d",table[idCount].scope);
            
        }
        fprintf(FileName,"\n");
    #else
        return ID;
    #endif
}


{HEXADECMIAL} {
    #ifdef ONLY_FOR_LEX
        fprintf(FileName,"HEXADECMIAL\t%s",yytext);
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;
        fprintf(FileName,"\n");
    #else
        return HEXADECMIAL;
    #endif
}



{FP} {
    #ifdef ONLY_FOR_LEX
        fprintf(FileName,"FP\t%s",yytext);
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;
        fprintf(FileName,"\n");
    #else
        return FP;
    #endif
}

{NUMBER} {
    #ifdef ONLY_FOR_LEX
        fprintf(FileName,"NUMBER\t%s",yytext);
        fprintf(FileName,"\t%d\t%d",yylineno,charCount);
        charCount+=yyleng;
        fprintf(FileName,"\n");
    #else
        return NUMBER;
    #endif
}

{EOL} {yylineno++;charCount=0;}
{WHITE}
%%

#ifdef ONLY_FOR_LEX
int main(int argc, char **argv){
    if(argc != 3){
        fprintf(stderr, "Argument Not Enough");
        exit(EXIT_FAILURE);
    }

    if(!(yyin = fopen(argv[1], "r"))){
        fprintf(stderr, "No such file or directory: %s", argv[1]);
        exit(EXIT_FAILURE);
    }

    if(!(yyout = fopen(argv[2], "w"))){
        fprintf(stderr, "No such file or directory: %s", argv[3]);
        exit(EXIT_FAILURE);
    }
    else
    {
    	fprintf(FileName,"token\tlexeme\tlineno\toffsett\tscopeno\n");
    }

    yylex();
    return 0;
}
#endif
