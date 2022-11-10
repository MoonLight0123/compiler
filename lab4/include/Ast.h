#ifndef __AST_H__
#define __AST_H__

#include <fstream>

class SymbolEntry;
class Type;


class Node
{
private:
    static int counter;
    int seq;
public:
    Node();
    int getSeq() const {return seq;};
    virtual void output(int level) = 0;
};
class StmtNode : public Node
{};
class ExprNode : public Node
{
protected:
    SymbolEntry *symbolEntry;
public:
    ExprNode(SymbolEntry *symbolEntry) : symbolEntry(symbolEntry){};
};

class BinaryExpr : public ExprNode
{
private:
    int op;
    ExprNode *expr1, *expr2;
public:
    enum {ADD,EQUAL, SUB ,MUL, DIV, MOD, OR, AND, NOTEQUAL, LESS, GREATER, LESSEQUAL, GREATEREQUAL};
    BinaryExpr(SymbolEntry *se, int op, ExprNode*expr1, ExprNode*expr2) : ExprNode(se), op(op), expr1(expr1), expr2(expr2){};
    void output(int level);
};
class UnaryExpr : public ExprNode
{
private:
    int op;
    ExprNode *expr;
public:
    enum {ADD,SUB,NOT};
    UnaryExpr(SymbolEntry *se, int op, ExprNode*expr1) : ExprNode(se), op(op), expr(expr1){};
    void output(int level);
};

class Constant : public ExprNode
{
public:
    Constant(SymbolEntry *se) : ExprNode(se){};
    void output(int level);
};

class Id : public ExprNode
{
public:
    Id(SymbolEntry *se) : ExprNode(se){};
    void output(int level);
};

class ArrayElement : public ExprNode
{
private:
    StmtNode *dimValList;
public:
    ArrayElement(SymbolEntry *se,StmtNode *dimValList) : ExprNode(se),dimValList(dimValList){};
    void output(int level);
};
//函数调用
class FuncCall : public ExprNode
{
private:
    ExprNode* FuncName;
    ExprNode* FuncRParams;
public:
    FuncCall(SymbolEntry *se, ExprNode* FuncName,ExprNode* FuncRParams):ExprNode(se),FuncName(FuncName),FuncRParams(FuncRParams){};
    void output(int level);
};

//实参
class FuncRParam : public ExprNode
{
private:
    ExprNode *param1,*param2;
public:
    FuncRParam(SymbolEntry *se,ExprNode *param1,ExprNode *param2):ExprNode(se),param1(param1),param2(param2){};
    void output(int level);
};

class FuncFArrayParam : public StmtNode{
protected:
    Id *pointer;
    StmtNode *arrayDim;
public:
    FuncFArrayParam(Id *pointer,StmtNode *arrayDim) : pointer(pointer),arrayDim(arrayDim){};
    void output(int level);
};
class EmptyStmt : public StmtNode{

public:
	EmptyStmt(){};
	void output(int level);
};


class FuncCallStmt : public StmtNode{
private:

	ExprNode *exp;
public:
	FuncCallStmt(ExprNode *exp):exp(exp){};
	void output(int level);
};


class FuncFParam : public StmtNode
{
private:
    StmtNode *param1,*param2;
public:
    FuncFParam(StmtNode* param1,StmtNode* param2): param1(param1),param2(param2){};
    void output(int level);
};

class CompoundStmt : public StmtNode
{
private:
    StmtNode *stmt;
public:
    
    CompoundStmt(StmtNode *stmt) : stmt(stmt) {};
    void output(int level);
};

class SeqNode : public StmtNode
{
private:
    StmtNode *stmt1, *stmt2;
public:
    SeqNode(StmtNode *stmt1, StmtNode *stmt2) : stmt1(stmt1), stmt2(stmt2){};
    void output(int level);
};
class DeclArrayStmt : public StmtNode
{
protected:
    Id *pointer;
    StmtNode *arrayDim;
public:
    DeclArrayStmt(Id *pointer,StmtNode *arrayDim) : pointer(pointer),arrayDim(arrayDim){};
    void output(int level);
};
class ArrayDims : public StmtNode
{
private:
    StmtNode *dim1, *dim2;
public:
    ArrayDims(StmtNode *dim1, StmtNode *dim2) : dim1(dim1), dim2(dim2){};
    void output(int level);
};

class ArrayDim : public StmtNode
{
private:
    ExprNode *dimVal;
public:
    ArrayDim(ExprNode *dimVal) : dimVal(dimVal){};
    void output(int level);
};

class DeclStmt : public StmtNode
{
protected:
    Id *id;
public:
    DeclStmt(Id *id) : id(id){};
    void output(int level);
};

class DeclInitStmt : public DeclStmt//////////////////////////////////////////////////
{
private:
    ExprNode *initVal;
public:
    DeclInitStmt(Id *id,ExprNode *initVal):DeclStmt(id),initVal(initVal){};
    void output(int level);
};/////////////////////////////////////////////////////

class ConstDeclInitStmt : public DeclStmt//////////////////////////////////////////////////
{
private:
    ExprNode *initVal;
public:
    ConstDeclInitStmt(Id *id,ExprNode *initVal):DeclStmt(id),initVal(initVal){};
    void output(int level);
};/////////////////////////////////////////////////////

class DeclList : public StmtNode///////
{
protected:
    StmtNode *decl1,*decl2;
public:
    DeclList(StmtNode* decl1,StmtNode* decl2):decl1(decl1),decl2(decl2){};
    void output(int level);
};///////

class ConstDeclList : public StmtNode///////
{
protected:
    StmtNode *decl1,*decl2;
public:
    ConstDeclList(StmtNode* decl1,StmtNode* decl2):decl1(decl1),decl2(decl2){};
    void output(int level);
};///////

class IfStmt : public StmtNode
{
private:
    ExprNode *cond;
    StmtNode *thenStmt;
public:
    IfStmt(ExprNode *cond, StmtNode *thenStmt) : cond(cond), thenStmt(thenStmt){};
    void output(int level);
};

class IfElseStmt : public StmtNode
{
private:
    ExprNode *cond;
    StmtNode *thenStmt;
    StmtNode *elseStmt;
public:
    IfElseStmt(ExprNode *cond, StmtNode *thenStmt, StmtNode *elseStmt) : cond(cond), thenStmt(thenStmt), elseStmt(elseStmt) {};
    void output(int level);
};

class ReturnStmt : public StmtNode
{
private:
    ExprNode *retValue;
public:
    ReturnStmt(ExprNode*retValue) : retValue(retValue) {};
    void output(int level);
};

class ContinueStmt : public StmtNode
{
    
public:
    void output(int level);
};

class BreakStmt : public StmtNode
{
    
public:
    void output(int level);
};


class WhileStmt : public StmtNode
{
private:
    ExprNode *cond;
    StmtNode *whileStmt;
public:
    WhileStmt(ExprNode* cond,StmtNode* whileStmt):cond(cond),whileStmt(whileStmt){};
    void output(int level);
};
class AssignStmt : public StmtNode
{
private:
    ExprNode *lval;
    ExprNode *expr;
public:
    AssignStmt(ExprNode *lval, ExprNode *expr) : lval(lval), expr(expr) {};
    void output(int level);
};

class FunctionDef : public StmtNode
{
private:
    SymbolEntry *se;
    StmtNode *blockStmt;
    StmtNode *paraStmt;
public:
    FunctionDef(SymbolEntry *se, StmtNode *blockStmt,StmtNode *paraStmt) : se(se), blockStmt(blockStmt),paraStmt(paraStmt){};
    void output(int level);
};

class Ast
{
private:
    Node* root;
public:
    Ast() {root = nullptr;}
    void setRoot(Node*n) {root = n;}
    void output();
};

#endif


