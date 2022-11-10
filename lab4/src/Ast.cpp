#include "Ast.h"
#include "SymbolTable.h"
#include <string>
#include "Type.h"

extern FILE *yyout;
int Node::counter = 0;

Node::Node()
{
    seq = counter++;
}

void Ast::output()
{
    fprintf(yyout, "program\n");
    if(root != nullptr)
        root->output(4);
}

void BinaryExpr::output(int level)
{
    std::string op_str;
    switch(op)
    {
        case ADD:
            op_str = "add";
            break;
        case SUB:
            op_str = "sub";
            break;
        case AND:
            op_str = "and";
            break;
        case OR:
            op_str = "or";
            break;
        case EQUAL:
            op_str = "EQUAL";
            break;
        case MUL:
            op_str = "MUL";
            break;
        case DIV:
            op_str = "DIV";
            break;
        case MOD:
            op_str = "MOD";
            break;
        case NOTEQUAL:
            op_str = "NOTEQUAL";
            break;
        case LESS:
            op_str = "less";
            break;
        case GREATER:
            op_str = "GREATER";
            break;
        case LESSEQUAL:
            op_str = "LESSEQUAL";
            break;
        case GREATEREQUAL:
            op_str = "GREATEREQUAL";
            break;
    }
    fprintf(yyout, "%*cBinaryExpr\top: %s\n", level, ' ', op_str.c_str());
    expr1->output(level + 4);
    expr2->output(level + 4);
}




void EmptyStmt::output(int level)
{
    //fprintf(yyout, "%*cEmptyStmt\n", level, ' ');
    

}
void DeclArrayStmt::output(int level)
{
    fprintf(yyout, "%*cArrayDeclStmt\n", level, ' ');
    pointer->output(level+4);
    arrayDim->output(level+4);
}

void FuncFArrayParam::output(int level)
{
    fprintf(yyout, "%*cFunctionArrayParam\n", level, ' ');
    pointer->output(level+4);
    if(arrayDim!=nullptr)
        arrayDim->output(level+4);
}
void ArrayDims::output(int level)
{
    fprintf(yyout, "%*cArrayDims\n", level, ' ');
    dim1->output(level+4);
    dim2->output(level+4);
}

void ArrayDim::output(int level)
{
    fprintf(yyout, "%*cArrayDimVal\n", level, ' ');
    dimVal->output(level+4);
}

void FuncCallStmt::output(int level)
{
    fprintf(yyout, "%*cFuncCallStmt\n", level, ' ');
    exp->output(level + 4);

}


void FuncRParam::output(int level)
{
    fprintf(yyout, "%*cFuncRealParams\n", level, ' ');
    param1->output(level + 4);
    param2->output(level + 4);
}

void FuncCall::output(int level)
{
    fprintf(yyout, "%*cFuncCall\n", level, ' ');
    if(FuncName!=nullptr)
        FuncName->output(level+4);
    else 
        fprintf(yyout, "%*cFunction not declare!\n", level+4, ' ');
    if(FuncRParams!=nullptr)
        FuncRParams->output(level+4);
}

void UnaryExpr::output(int level)
{
    std::string op_str;
    switch(op)
    {
        case ADD:
            op_str = "add";
            break;
        case SUB:
            op_str = "sub";
            break;
        case NOT:
            op_str = "not";
            break;
    }
    fprintf(yyout, "%*cUnaryExpr\top: %s\n", level, ' ', op_str.c_str());
    expr->output(level + 4);
}
void Constant::output(int level)
{
    std::string type, value;
    type = symbolEntry->getType()->toStr();
    value = symbolEntry->toStr();
    fprintf(yyout, "%*cIntegerLiteral\tvalue: %s\ttype: %s\n", level, ' ',
            value.c_str(), type.c_str());
}

void Id::output(int level)
{
    std::string name, type;
    int scope;
    name = symbolEntry->toStr();
    type = symbolEntry->getType()->toStr();
    scope = dynamic_cast<IdentifierSymbolEntry*>(symbolEntry)->getScope();
    fprintf(yyout, "%*cId\tname: %s\tscope: %d\ttype: %s\n", level, ' ',
            name.c_str(), scope, type.c_str());
}

void ArrayElement::output(int level)
{
    std::string name, type;
    int scope;
    name = symbolEntry->toStr();
    type = symbolEntry->getType()->toStr();
    scope = dynamic_cast<IdentifierSymbolEntry*>(symbolEntry)->getScope();
    fprintf(yyout, "%*cArray\tname: %s\tscope: %d\ttype: %s\n", level, ' ',
            name.c_str(), scope, type.c_str());
    dimValList->output(level+4);
}

void FuncFParam::output(int level)
{
    fprintf(yyout, "%*cFuncFparamList\n", level, ' ');
    param1->output(level+4);
    param2->output(level+4);
}

void CompoundStmt::output(int level)
{
    fprintf(yyout, "%*cCompoundStmt\n", level, ' ');
    if(stmt!=nullptr)
    stmt->output(level + 4);
}

void SeqNode::output(int level)
{
    fprintf(yyout, "%*cSequence\n", level, ' ');
    stmt1->output(level + 4);
    stmt2->output(level + 4);
}

void DeclStmt::output(int level)
{
    fprintf(yyout, "%*cDeclStmt\n", level, ' ');
    id->output(level + 4);
}

void DeclInitStmt::output(int level)////////////////////////////////
{
    fprintf(yyout, "%*cDeclInitStmt\n", level, ' ');
    id->output(level + 4);
    initVal->output(level+4);
}/////////////////////////////////////////////////////////////

void ConstDeclInitStmt::output(int level)////////////////////////////////
{
    fprintf(yyout, "%*cConstDeclInitStmt\n", level, ' ');
    id->output(level + 4);
    initVal->output(level+4);
}/////////////////////////////////////////////////////////////

void DeclList::output(int level)
{
    fprintf(yyout, "%*cDeclList\n", level, ' ');
    decl1->output(level+4);
    decl2->output(level+4);
}

void ConstDeclList::output(int level)
{
    fprintf(yyout, "%*cConstDeclList\n", level, ' ');
    decl1->output(level+4);
    decl2->output(level+4);
}

void IfStmt::output(int level)
{
    fprintf(yyout, "%*cIfStmt\n", level, ' ');
    cond->output(level + 4);
    thenStmt->output(level + 4);
}

void IfElseStmt::output(int level)
{
    fprintf(yyout, "%*cIfElseStmt\n", level, ' ');
    cond->output(level + 4);
    thenStmt->output(level + 4);
    elseStmt->output(level + 4);
}

void ReturnStmt::output(int level)
{
    fprintf(yyout, "%*cReturnStmt\n", level, ' ');
    retValue->output(level + 4);
}

void BreakStmt::output(int level)
{
    fprintf(yyout, "%*cBreakStmt\n", level, ' ');
}

void ContinueStmt::output(int level)
{
    fprintf(yyout, "%*cContinueStmt\n", level, ' ');
}

void WhileStmt::output(int level)
{
    fprintf(yyout, "%*cWhileStmt\n", level, ' ');
    cond->output(level + 4);
    whileStmt->output(level + 4);
}
void AssignStmt::output(int level)
{
    fprintf(yyout, "%*cAssignStmt\n", level, ' ');
    lval->output(level + 4);
    expr->output(level + 4);
}

void FunctionDef::output(int level)
{
    std::string name, type;
    name = se->toStr();
    type = se->getType()->toStr();
    fprintf(yyout, "%*cFunctionDefine function name: %s, type: %s\n", level, ' ', 
            name.c_str(), type.c_str());
    if(paraStmt!=nullptr)
        paraStmt->output(level+4);
    blockStmt->output(level + 4);
}

