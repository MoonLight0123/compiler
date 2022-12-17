#include <iostream>
#include <string.h>
#include <unistd.h>
#include "Ast.h"
#include "Unit.h"
using namespace std;

Ast ast;
Unit unit;
extern FILE *yyin;
extern FILE *yyout;

int yyparse();

char outfile[256] = "a.out";
bool dump_tokens;
bool dump_ast;
bool dump_ir;

int main(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "iato:")) != -1)
    {
        switch (opt)
        {
        case 'o':
            strcpy(outfile, optarg);
            break;
        case 'a':
            dump_ast = true;
            break;
        case 't':
            dump_tokens = true;
            break;
        case 'i':
            dump_ir = true;
            break;
        default:
            fprintf(stderr, "Usage: %s [-o outfile] infile\n", argv[0]);
            exit(EXIT_FAILURE);
            break;
        }
    }
    if (optind >= argc)
    {
        fprintf(stderr, "no input file\n");
        exit(EXIT_FAILURE);
    }
    if (!(yyin = fopen(argv[optind], "r")))
    {
        fprintf(stderr, "%s: No such file or directory\nno input file\n", argv[optind]);
        exit(EXIT_FAILURE);
    }
    if (!(yyout = fopen(outfile, "w")))
    {
        fprintf(stderr, "%s: fail to open output file\n", outfile);
        exit(EXIT_FAILURE);
    }
    
     vector<Type*> tempParaType;
    //p13修正void返回值的系统函数问题
    tempParaType.push_back(TypeSystem::voidType);
    auto funcType = new FunctionType(TypeSystem::intType, tempParaType);
    auto id = new IdentifierSymbolEntry(funcType, "getint", 0);
    identifiers->installFunc("getint", id);
    //q6在全局区添加系统函数声明和全局变量
    unit.getSysFuncs().push_back(id);

    auto funcType1 = new FunctionType(TypeSystem::intType, tempParaType);
    auto id1 = new IdentifierSymbolEntry(funcType1, "getch", 0);
    identifiers->installFunc("getch", id1);
    unit.getSysFuncs().push_back(id1);


    auto funcType3 = new FunctionType(TypeSystem::voidType, tempParaType);
    auto id3 = new IdentifierSymbolEntry(funcType3, "starttime", 0);
    identifiers->installFunc("starttime", id3);
    unit.getSysFuncs().push_back(id3);

    auto funcType4 = new FunctionType(TypeSystem::voidType, tempParaType);
    auto id4 = new IdentifierSymbolEntry(funcType4, "stoptime", 0);
    identifiers->installFunc("stoptime", id4);
    unit.getSysFuncs().push_back(id4);

    tempParaType[0] = TypeSystem::intType;
    auto funcType7 = new FunctionType(TypeSystem::voidType, tempParaType);
    auto id7 = new IdentifierSymbolEntry(funcType7, "putint", 0);
    identifiers->installFunc("putint", id7);
    unit.getSysFuncs().push_back(id7);

    auto funcType8 = new FunctionType(TypeSystem::voidType, tempParaType);
    auto id8 = new IdentifierSymbolEntry(funcType8, "putch", 0);
    identifiers->installFunc("putch", id8);
    unit.getSysFuncs().push_back(id8);





    yyparse();
    printf("1\n");
    if(dump_ast){
        ast.output();
        printf("1\n");
        }
    ast.typeCheck();
    printf("2\n");
    ast.genCode(&unit);
    printf("3\n");
    if(dump_ir){
        unit.output();
        printf("4\n");
        }
        printf("5\n");
    return 0;
}
