%code top{
    #include <iostream>
    #include <assert.h>
    #include <vector>
    #include "parser.h"
    extern Ast ast;
    int yylex();
    int yyerror( char const * );
    Type *basicDeclType;
    Type *funcDeclType;
    std::vector<Type*> FuncFParamsTypeVector;
}

%code requires {
    #include "Ast.h"
    #include "SymbolTable.h"
    #include "Type.h"
}

%union {
    int itype;
    float ftype;
    char* strtype;
    StmtNode* stmttype;
    ExprNode* exprtype;
    Type* type;
}

%start Program
%token <strtype> ID 
%token <itype> INTEGER
%token <ftype> FLOATNUM
%token IF ELSE WHILE CONTINUE BREAK
%token INT VOID FLOAT
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON COMMA LBRACKET RBRACKET
%token ADD ASSIGN EQUAL NOT SUB MUL DIV MOD OR AND NOTEQUAL LESS GREATER LESSEQUAL GREATEREQUAL
%token RETURN CONST

%nterm <stmttype> Stmts Stmt AssignStmt BlockStmt IfStmt ReturnStmt ContinueStmt BreakStmt
%nterm <stmttype> DeclStmt FuncDef IdDeclLists IdDeclList ConstDeclLists ConstDeclList WhileStmt FuncFParam FuncFParams EmptyStmt ExprStmt VarDeclStmt ConstDeclStmt ConstArrayDims ArrayDims FuncFArrayParamDims 
%nterm <exprtype> Exp AddExp Cond LOrExp PrimaryExp LVal RelExp LAndExp InitVal MulExp UnaryExp EqExp FuncRParams ConstExp 
%nterm <type> BType


%precedence THEN
%precedence ELSE
%%
Program
    : Stmts {
        ast.setRoot($1);
    }
    ;
Stmts
    : Stmt {$$=$1;}
    | Stmts Stmt{
        $$ = new SeqNode($1, $2);
    }

    ;
//语句
Stmt
    : AssignStmt {$$=$1;}
    | BlockStmt {$$=$1;}
    | IfStmt {$$=$1;}
    | ReturnStmt {$$=$1;}
    | BreakStmt {$$=$1;}
    | ContinueStmt {$$=$1;}
    | DeclStmt {$$=$1;}
    | FuncDef {$$=$1;}
    | WhileStmt {$$=$1;}
    | ExprStmt {$$=$1;}
    | EmptyStmt {$$=$1;}
    ;

//左值表达式 LVal → Ident {'[' Exp ']'}
LVal
    : ID {
        SymbolEntry *se;
        se = identifiers->lookup($1);
        if(se == nullptr)
        {
            fprintf(stderr, "identifier \"%s\" is undefined\n", (char*)$1);
            delete [](char*)$1;
            assert(se != nullptr);
        }
        $$ = new Id(se);
        delete []$1;
    }
    |
    ID ArrayDims {
        SymbolEntry *se;
        se = identifiers->lookup($1);
        if(se == nullptr)
        {
            fprintf(stderr, "identifier \"%s\" is undefined\n", (char*)$1);
            delete [](char*)$1;
            assert(se != nullptr);
        }
        $$=new ArrayElement(se,$2);
        delete []$1;
    }
    ;

EmptyStmt
    :
     SEMICOLON{
        $$ = new EmptyStmt();
    }
    ;

ExprStmt 
    :
    Exp SEMICOLON {
        $$=new ExprStmt($1);
    }
    ;
// FuncCallStmt 
//     :
//     Cond SEMICOLON{
//         $$ = new FuncCallStmt($1);
//     }
//     ;


AssignStmt
    :
    LVal ASSIGN Exp SEMICOLON {
        $$ = new AssignStmt($1, $3);
    }
    ;

//语句块 
BlockStmt
    :   LBRACE 
        {identifiers = new SymbolTable(identifiers);} 
        Stmts RBRACE 
        {
            $$ = new CompoundStmt($3);
            SymbolTable *top = identifiers;
            identifiers = identifiers->getPrev();
            delete top;
        }
     
    |   LBRACE RBRACE {
    
            $$ = new CompoundStmt(nullptr);
            //$$ = new EmptyStmt();
       }

    ;
IfStmt
    : IF LPAREN Cond RPAREN Stmt %prec THEN {
        $$ = new IfStmt($3, $5);
    }
    | IF LPAREN Cond RPAREN Stmt ELSE Stmt {
        $$ = new IfElseStmt($3, $5, $7);
    }
    

    ;
ReturnStmt
    :
    RETURN Exp SEMICOLON{
        $$ = new ReturnStmt($2);
    }
    ;
WhileStmt
    :
    WHILE LPAREN Cond RPAREN Stmt {
        $$=new WhileStmt($3,$5);
    }
    ;

ContinueStmt
    :
    CONTINUE SEMICOLON{
        $$=new ContinueStmt();
    }
    ;

BreakStmt
    :
    BREAK SEMICOLON{
        $$=new BreakStmt();
    }
    ;
    
//表达式 Exp → AddExp    注：SysY 表达式是 int/float 型表达式
Exp
    :
    AddExp {$$ = $1;}
    ;
ConstExp
    :
    AddExp {$$=$1;}
    ;
//条件表达式  Cond → LOrExp
Cond
    :
    LOrExp {$$ = $1;}
    ;

//基本表达式 PrimaryExp → '(' Exp ')' | LVal | Number
PrimaryExp
    :
    LVal {$$ = $1;}
    | 
    INTEGER {
        SymbolEntry *se = new ConstantSymbolEntry(TypeSystem::intType, $1);
        $$ = new Constant(se);
    }
    |
    LPAREN Exp RPAREN {$$=$2;}
    ;

//乘除模表达式 MulExp → UnaryExp | MulExp ('*' | '/' | '%') UnaryExp
MulExp
    :
    UnaryExp {$$=$1;}
    |
    MulExp MUL UnaryExp {
        SymbolEntry *se=new TemporarySymbolEntry(TypeSystem::intType,SymbolTable::getLabel());
        $$=new BinaryExpr(se,BinaryExpr::MUL,$1,$3);
    }
    |
    MulExp DIV UnaryExp {
        SymbolEntry *se=new TemporarySymbolEntry(TypeSystem::intType,SymbolTable::getLabel());
        $$=new BinaryExpr(se,BinaryExpr::DIV,$1,$3);
    }
    |
    MulExp MOD UnaryExp {
        SymbolEntry *se=new TemporarySymbolEntry(TypeSystem::intType,SymbolTable::getLabel());
        $$=new BinaryExpr(se,BinaryExpr::MOD,$1,$3);
    }
    ;

//一元表达式 UnaryExp → PrimaryExp | Ident '(' [FuncRParams] ')' | （+，-，！） UnaryExp
UnaryExp
    :
    PrimaryExp {$$=$1;}
    |
    ADD UnaryExp {
        SymbolEntry *se=new TemporarySymbolEntry(TypeSystem::intType,SymbolTable::getLabel());
        $$=new UnaryExpr(se,UnaryExpr::ADD,$2);
    }
    |
    SUB UnaryExp {
        SymbolEntry *se=new TemporarySymbolEntry(TypeSystem::intType,SymbolTable::getLabel());
        $$=new UnaryExpr(se,UnaryExpr::SUB,$2);
    }
    |
    NOT UnaryExp {
        SymbolEntry *se=new TemporarySymbolEntry(TypeSystem::intType,SymbolTable::getLabel());
        $$=new UnaryExpr(se,UnaryExpr::NOT,$2);
    }
    |
    ID LPAREN FuncRParams RPAREN {
        SymbolEntry *see=new TemporarySymbolEntry(TypeSystem::intType,SymbolTable::getLabel());
        SymbolEntry *se;
        se = identifiers->lookup($1);
        if(se == nullptr)
        {
            fprintf(stderr, "Function \"%s\" is undefined\n", (char*)$1);
            delete [](char*)$1;
            //assert(se != nullptr);
        }
        Id *funcName=new Id(se);
        $$=new FuncCall(see,funcName,$3);
        delete []$1;
    }
    ;

//函数实参表 FuncRParams → Exp { ',' Exp }

FuncRParams
    :
    %empty {$$=nullptr;}
    |
    Exp {$$=$1;}
    |
    FuncRParams COMMA Exp {
        SymbolEntry *se=new TemporarySymbolEntry(TypeSystem::intType,SymbolTable::getLabel());
        $$=new FuncRParam(se,$1,$3);
    }
    
//加减表达式 AddExp → MulExp | AddExp ('+' | '−') MulExp
AddExp
    :
    MulExp {$$ = $1;}
    |
    AddExp ADD MulExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::ADD, $1, $3);
    }
    |
    AddExp SUB MulExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::SUB, $1, $3);
    }
    ;

//关系表达式 RelExp → AddExp | RelExp ('<' | '>' | '<=' | '>=') AddExp 
RelExp
    :
    AddExp {$$ = $1;}
    |
    RelExp LESS AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LESS, $1, $3);
    }
    |
    RelExp GREATER AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::GREATER, $1, $3);
    }
    |
    RelExp LESSEQUAL AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LESSEQUAL, $1, $3);
    }
    |
    RelExp GREATEREQUAL AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::GREATEREQUAL, $1, $3);
    }
    ;

//逻辑与表达式 LAndExp → EqExp | LAndExp '&&' EqExp
LAndExp
    :
    EqExp {$$ = $1;}
    |
    LAndExp AND EqExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::AND, $1, $3);
    }
    ;

//逻辑或表达式 LOrExp → LAndExp | LOrExp '||' LAndExp
LOrExp
    :
    LAndExp {$$ = $1;}
    |
    LOrExp OR LAndExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::OR, $1, $3);
    }
    ;

//相等性表达式 EqExp → RelExp | EqExp ('==' | '!=') RelExp
EqExp
    :
    RelExp {$$=$1;}
    |
    EqExp EQUAL RelExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::EQUAL, $1, $3);
    }
    |
    EqExp NOTEQUAL RelExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::NOTEQUAL, $1, $3);
    }
    ;

//类型

// FuncType 
//     : BType {
//         $$=$1;
//         funcDeclType=basicDeclType;
//     }
//     | VOID {
//         $$ = TypeSystem::voidType;
//         funcDeclType=TypeSystem::voidType;
//     }
//     ;

BType
    : INT {
        $$ = TypeSystem::intType;
        basicDeclType=TypeSystem::intType;
    }
    | FLOAT {
        $$ = TypeSystem::floatType;
        basicDeclType=TypeSystem::floatType;
    }
    | VOID {
        $$ = TypeSystem::voidType;
        basicDeclType=TypeSystem::voidType;
    }
    ;
//声明，变量和常量

DeclStmt
    :
    VarDeclStmt {$$=$1;}
    |
    ConstDeclStmt {$$=$1;}
    ;

VarDeclStmt
    :
    BType IdDeclLists SEMICOLON {$$=$2;}
    ;

ConstDeclStmt
    :
    CONST BType ConstDeclLists SEMICOLON {$$=$3;}
    ;


IdDeclLists 
    :
    IdDeclLists COMMA IdDeclList {$$=new DeclList($1,$3);}
    | IdDeclList {$$=$1;}
    ;

IdDeclList
    :
    ID ASSIGN InitVal {
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(basicDeclType, $1, identifiers->getLevel());
        identifiers->install($1, se);
        $$ = new DeclInitStmt(new Id(se),$3);
        delete []$1;
    }
    | ID {
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(basicDeclType, $1, identifiers->getLevel());
        identifiers->install($1, se);
        $$ = new DeclStmt(new Id(se));
        delete []$1;
    }
    | ID ConstArrayDims {
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(basicDeclType, $1, identifiers->getLevel());
        identifiers->install($1, se);
        $$=new DeclArrayStmt(new Id(se),$2);
        delete []$1;
    }
    | ID ConstArrayDims ASSIGN InitVal {

    }
    ;

ConstArrayDims
:
    ConstArrayDims LBRACKET ConstExp RBRACKET {
        $$=new ArrayDims($1,new ArrayDim($3));
    }
    |
    LBRACKET ConstExp RBRACKET {$$=new ArrayDim($2);}
;

ArrayDims
:
    ArrayDims LBRACKET Exp RBRACKET {
        $$=new ArrayDims($1,new ArrayDim($3));
    }
    |
    LBRACKET Exp RBRACKET {$$=new ArrayDim($2);}
;

ConstDeclLists 
    :
    ConstDeclLists COMMA ConstDeclList {$$=new ConstDeclList($1,$3);}
    | ConstDeclList {$$=$1;}
    ;

ConstDeclList
    :
    ID ASSIGN ConstExp {
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(basicDeclType, $1, identifiers->getLevel());
        identifiers->install($1, se);
        $$ = new ConstDeclInitStmt(new Id(se),$3);
        delete []$1;
    }
    ;


//函数定义     FuncDef → FuncType Ident '(' [FuncFParams] ')' Block
FuncDef
    :
    BType ID {
        // Type *funcType;
        // funcType = new FunctionType($1,{});
        SymbolEntry *se = new IdentifierSymbolEntry(nullptr, $2, identifiers->getLevel());
        identifiers->install($2, se);
        identifiers = new SymbolTable(identifiers);
    }
    LPAREN FuncFParams RPAREN
    {
        FunctionType *funcType;
        funcType=new FunctionType($1,{});
        FuncFParamsTypeVector.swap(funcType->paramsType);

        SymbolEntry *se;
        se = identifiers->lookup($2);
        //IdentifierSymbolEntry* ss=(IdentifierSymbolEntry*)se;
        //ss->setFuncType(funcType);
        se->setType((Type*)funcType);
    }
    BlockStmt
    {
        SymbolEntry *se;
        se = identifiers->lookup($2);
        assert(se != nullptr);
        $$ = new FunctionDef(se, $8,$5);
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
        delete []$2;
    }
    ;

//函数形参表  FuncFParams → FuncFParam { ',' FuncFParam }
FuncFParams
    :
    %empty {$$=nullptr;}
    | 
    FuncFParam {$$=$1;}
    |
    FuncFParams COMMA FuncFParam {
        $$=new FuncFParam($1,$3);
    }
    ;

//函数形参 FuncFParam → BType Ident ['[' ']' { '[' Exp ']' }]
FuncFParam
    :
    BType ID {
        SymbolEntry *se;
        se = new IdentifierSymbolEntry($1, $2, identifiers->getLevel());
        identifiers->install($2, se);
        $$ = new DeclStmt(new Id(se));
        delete []$2;
        FuncFParamsTypeVector.push_back($1);
    }
    |
    BType ID LBRACKET RBRACKET FuncFArrayParamDims {
        SymbolEntry *se;
        se = new IdentifierSymbolEntry($1, $2, identifiers->getLevel());
        identifiers->install($2, se);
        $$=new FuncFArrayParam(new Id(se),$5);
        delete []$2;
        FuncFParamsTypeVector.push_back($1);
    }
    ;
FuncFArrayParamDims
:
    FuncFArrayParamDims LBRACKET Exp RBRACKET {
        $$=new ArrayDims($1,new ArrayDim($3));
    }
    | %empty {$$=nullptr;}
;
//变量初值 InitVal → Exp | '{' [ InitVal { ',' InitVal } ] '}'
InitVal
    :
    Exp {$$=$1;}
    ;
%%

int yyerror(char const* message)
{
    std::cerr<<message<<std::endl;
    return -1;
}

