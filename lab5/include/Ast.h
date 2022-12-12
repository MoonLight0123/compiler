#ifndef __AST_H__
#define __AST_H__

#include <fstream>
#include "Operand.h"

class SymbolEntry;
class Unit;
class Function;
class BasicBlock;
class Instruction;
class IRBuilder;

class Node
{
private:
    static int counter;
    int seq;
protected:
    std::vector<Instruction*> true_list;
    std::vector<Instruction*> false_list;
    static IRBuilder *builder;
    void backPatch(std::vector<Instruction*> &list, BasicBlock*bb);
    std::vector<Instruction*> merge(std::vector<Instruction*> &list1, std::vector<Instruction*> &list2);

public:
    Node();
    int getSeq() const {return seq;};
    static void setIRBuilder(IRBuilder*ib) {builder = ib;};
    virtual void output(int level) = 0;
    virtual void typeCheck() = 0;
    virtual void genCode() = 0;
    std::vector<Instruction*>& trueList() {return true_list;}
    std::vector<Instruction*>& falseList() {return false_list;}
};

class ExprNode : public Node
{
protected:
    SymbolEntry *symbolEntry;
    Operand *dst;   // The result of the subtree is stored into dst.
public:
    ExprNode(SymbolEntry *symbolEntry) : symbolEntry(symbolEntry){};
    Operand* getOperand() {return dst;};
    SymbolEntry* getSymPtr() {return symbolEntry;};
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
    void typeCheck();
    void genCode();
};

class Constant : public ExprNode
{
public:
    Constant(SymbolEntry *se) : ExprNode(se){dst = new Operand(se);};
    void output(int level);
    void typeCheck();
    void genCode();
};

class Id : public ExprNode
{
public:
    Id(SymbolEntry *se) : ExprNode(se){SymbolEntry *temp = new TemporarySymbolEntry(se->getType(), SymbolTable::getLabel()); dst = new Operand(temp);};
    void output(int level);
    void typeCheck();
    void genCode();
};

class StmtNode : public Node
{};

class CompoundStmt : public StmtNode
{
private:
    StmtNode *stmt;
public:
    CompoundStmt(StmtNode *stmt) : stmt(stmt) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class SeqNode : public StmtNode
{
private:
    StmtNode *stmt1, *stmt2;
public:
    SeqNode(StmtNode *stmt1, StmtNode *stmt2) : stmt1(stmt1), stmt2(stmt2){};
    void output(int level);
    void typeCheck();
    void genCode();
};

class DeclStmt : public StmtNode
{
protected:
    Id *id;
public:
    DeclStmt(Id *id) : id(id){};
    void output(int level);
    void typeCheck();
    void genCode();
};

class IfStmt : public StmtNode
{
private:
    ExprNode *cond;
    StmtNode *thenStmt;
public:
    IfStmt(ExprNode *cond, StmtNode *thenStmt) : cond(cond), thenStmt(thenStmt){};
    void output(int level);
    void typeCheck();
    void genCode();
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
    void typeCheck();
    void genCode();
};

class ReturnStmt : public StmtNode
{
private:
    ExprNode *retValue;
public:
    ReturnStmt(ExprNode*retValue) : retValue(retValue) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class AssignStmt : public StmtNode
{
private:
    ExprNode *lval;
    ExprNode *expr;
public:
    AssignStmt(ExprNode *lval, ExprNode *expr) : lval(lval), expr(expr) {};
    void output(int level);
    void typeCheck();
    void genCode();
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
    void typeCheck();
    void genCode();
};



class WhileStmt : public StmtNode
{
private:
    ExprNode *cond;
    StmtNode *whileStmt;
public:
    WhileStmt(ExprNode* cond,StmtNode* whileStmt):cond(cond),whileStmt(whileStmt){};
    void output(int level);
    void typeCheck();
    void genCode();
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
    void typeCheck();
    void genCode();
};

class ArrayElement : public ExprNode
{
private:
    StmtNode *dimValList;
public:
    ArrayElement(SymbolEntry *se,StmtNode *dimValList) : ExprNode(se),dimValList(dimValList){};
    void output(int level);
    void typeCheck();
    void genCode();
};

class FuncCall : public ExprNode
{
private:
    ExprNode* FuncName;
    ExprNode* FuncRParams;
public:
    FuncCall(SymbolEntry *se, ExprNode* FuncName,ExprNode* FuncRParams):ExprNode(se),FuncName(FuncName),FuncRParams(FuncRParams){};
    void output(int level);
    void typeCheck();
    void genCode();
};

class FuncRParam : public ExprNode
{
private:
    ExprNode *param1,*param2;
public:
    FuncRParam(SymbolEntry *se,ExprNode *param1,ExprNode *param2):ExprNode(se),param1(param1),param2(param2){};
    void output(int level);
    void typeCheck();
    void genCode();
};

class FuncFArrayParam : public StmtNode{
protected:
    Id *pointer;
    StmtNode *arrayDim;
public:
    FuncFArrayParam(Id *pointer,StmtNode *arrayDim) : pointer(pointer),arrayDim(arrayDim){};
    void output(int level);
    void typeCheck();
    void genCode();
};

class EmptyStmt : public StmtNode{

public:
	EmptyStmt(){};
	void output(int level);
    void typeCheck();
    void genCode();
};

class ExprStmt : public StmtNode{
private:
	ExprNode *exp;
public:
	ExprStmt(ExprNode *exp):exp(exp){};
	void output(int level);
    void typeCheck();
    void genCode();
};

class FuncFParam : public StmtNode
{
private:
    StmtNode *param1,*param2;
public:
    FuncFParam(StmtNode* param1,StmtNode* param2): param1(param1),param2(param2){};
    void output(int level);
    void typeCheck();
    void genCode();
};

class DeclArrayStmt : public StmtNode
{
protected:
    Id *pointer;
    StmtNode *arrayDim;
public:
    DeclArrayStmt(Id *pointer,StmtNode *arrayDim) : pointer(pointer),arrayDim(arrayDim){};
    void output(int level);
    void typeCheck();
    void genCode();
};

class ArrayDims : public StmtNode
{
private:
    StmtNode *dim1, *dim2;
public:
    ArrayDims(StmtNode *dim1, StmtNode *dim2) : dim1(dim1), dim2(dim2){};
    void output(int level);
    void typeCheck();
    void genCode();
};

class ArrayDim : public StmtNode
{
private:
    ExprNode *dimVal;
public:
    ArrayDim(ExprNode *dimVal) : dimVal(dimVal){};
    void output(int level);
    void typeCheck();
    void genCode();
};

class DeclInitStmt : public DeclStmt
{
private:
    ExprNode *initVal;
public:
    DeclInitStmt(Id *id,ExprNode *initVal):DeclStmt(id),initVal(initVal){};
    void output(int level);
    void typeCheck();
    void genCode();
};

class ConstDeclInitStmt : public DeclStmt
{
private:
    ExprNode *initVal;
public:
    ConstDeclInitStmt(Id *id,ExprNode *initVal):DeclStmt(id),initVal(initVal){};
    void output(int level);
    void typeCheck();
    void genCode();
};

class DeclList : public StmtNode
{
protected:
    StmtNode *decl1,*decl2;
public:
    DeclList(StmtNode* decl1,StmtNode* decl2):decl1(decl1),decl2(decl2){};
    void output(int level);
    void typeCheck();
    void genCode();
};

class ConstDeclList : public StmtNode
{
protected:
    StmtNode *decl1,*decl2;
public:
    ConstDeclList(StmtNode* decl1,StmtNode* decl2):decl1(decl1),decl2(decl2){};
    void output(int level);
    void typeCheck();
    void genCode();
};

class ContinueStmt : public StmtNode
{
    
public:
    void output(int level);
    void typeCheck();
    void genCode();
};

class BreakStmt : public StmtNode
{
    
public:
    void output(int level);
    void typeCheck();
    void genCode();
};

class Ast
{
private:
    Node* root;
public:
    Ast() {root = nullptr;}
    void setRoot(Node*n) {root = n;}
    void output();
    void typeCheck();
    void genCode(Unit *unit);
};







#endif

