#include <iostream>
#include <string.h>
#include <unistd.h>
#include "Ast.h"
#include "Unit.h"
#include "MachineCode.h"
#include "LinearScan.h"
using namespace std;

Ast ast;
Unit unit;
MachineUnit mUnit;
extern FILE *yyin;
extern FILE *yyout;

int yyparse();

char outfile[256] = "a.out";
bool dump_tokens;
bool dump_ast;
bool dump_ir;
bool dump_asm;

int main(int argc, char *argv[])
{
printf("-1");
    int opt;
    while ((opt = getopt(argc, argv, "Siato:")) != -1)
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
        case 'S':
            dump_asm = true;
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
    tempParaType.push_back(TypeSystem::voidType);
    auto funcType = new FunctionType(TypeSystem::intType, tempParaType);
    auto id = new IdentifierSymbolEntry(funcType, "getint", 0);
    identifiers->installFunc("getint", id);
    //在全局区添加系统函数声明和全局变量
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
    printf("-1");
    if(dump_ast)
        ast.output();
    ast.typeCheck();
    printf("-2");
    ast.genCode(&unit);
    printf("-3");
    if(dump_ir)
        unit.output();
    printf("1");
    unit.genMachineCode(&mUnit);
    printf("2");
    LinearScan linearScan(&mUnit);
    printf("3");
    linearScan.allocateRegisters();
    printf("4");
    if(dump_asm)
        mUnit.output();
    printf("5");
    return 0;
}
