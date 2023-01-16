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
class ArrayValDim;

class Node
{
private:
    static int counter;
    int seq;
protected:
    std::vector<BasicBlock**> true_list;
    std::vector<BasicBlock**> false_list;
    static IRBuilder *builder;
    void backPatch(std::vector<BasicBlock**> &list, BasicBlock*target);
    std::vector<BasicBlock**> merge(std::vector<BasicBlock**> &list1, std::vector<BasicBlock**> &list2);

public:
    Node();
    int getSeq() const {return seq;};
    static void setIRBuilder(IRBuilder*ib) {builder = ib;};
    virtual void output(int level) = 0;
    virtual void typeCheck() = 0;
    virtual void genCode() = 0;
    std::vector<BasicBlock**>& trueList() {return true_list;}
    std::vector<BasicBlock**>& falseList() {return false_list;}
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
    virtual bool isConstantVal(int &val){return false;};
    virtual void getFuncRparamsOperand(std::vector<Operand*> &paramsVector){
        //if(dst!=nullptr)
        paramsVector.push_back(dst);};
    virtual void getArrayDimVal(std::vector<int> &a){};//用于声明时获取数组维度

    virtual void getArrayDimValOperand(std::vector<Operand*> &a){a.push_back(dst);};//用于非声明时使用数组维度
    virtual void getArrayInitValNode(std::vector<ArrayValDim*> &b);//{b.back()->initVals.push_back(this);};
    virtual int getArrayInitValType(){return 0;};//用于数组初始化赋值,区分arrayvaldim与arrayvaldimlist
};
class BinaryExpr : public ExprNode
{
private:
    int op;
    ExprNode *expr1, *expr2;
public:
    enum {OR,AND,ADD, SUB ,MUL, DIV, MOD,EQUAL,LESS, NOTEQUAL, GREATER, LESSEQUAL, GREATEREQUAL};
    BinaryExpr(SymbolEntry *se, int op, ExprNode*expr1, ExprNode*expr2) : ExprNode(se), op(op), expr1(expr1), expr2(expr2){dst=new Operand(se);};
    void output(int level);
    void typeCheck();
    void genCode();
    bool isConstantVal(int &val);
    
};

class Constant : public ExprNode
{
public:
    Constant(SymbolEntry *se) : ExprNode(se){dst = new Operand(se);};
    void output(int level);
    void typeCheck();
    void genCode();
    bool isConstantVal(int &val);
};

class Id : public ExprNode
{
public:
    Id(SymbolEntry *se) : ExprNode(se)
    {
        SymbolEntry *temp = new TemporarySymbolEntry(se->getType(), SymbolTable::getLabel());
        dst = new Operand(temp);
     };
    void output(int level);
    void typeCheck();
    void genCode();
    bool isConstantVal(int &val);
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




class Extend : public ExprNode
{
private:
    ExprNode *originNode;
public:
    Extend(SymbolEntry *se,ExprNode *originNode):ExprNode(se),originNode(originNode){dst = new Operand(se);};
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
    UnaryExpr(SymbolEntry *se, int op, ExprNode*expr1) : ExprNode(se), op(op), expr(expr1){dst=new Operand(se);};
    void output(int level);
    void typeCheck();
    void genCode();
    bool isConstantVal(int &val);
};

class ArrayElement : public ExprNode
{
private:
    ExprNode *dimValList;
    //SymbolEntry *arrayId;
public:
    ArrayElement(SymbolEntry *se,ExprNode *dimValList) : ExprNode(se),dimValList(dimValList)
    {
        Type* t=((ArrayType*)se->getType())->arrayElementType;
        SymbolEntry *temp = new TemporarySymbolEntry(t, SymbolTable::getLabel()); 
        dst = new Operand(temp);
    };
    void output(int level);
    void typeCheck();
    void genCode();
    virtual void getArrayDimValOperand(std::vector<Operand*> &a){dimValList->getArrayDimValOperand(a);};

    Operand* address;//存着要访问元素的指针
};

class FuncCall : public ExprNode
{
private:
    ExprNode* FuncName;
    ExprNode* FuncRParams;
public:
    FuncCall(SymbolEntry *se, ExprNode* FuncName,ExprNode* FuncRParams):ExprNode(se),FuncName(FuncName),FuncRParams(FuncRParams){dst=new Operand(se);};
    void output(int level);
    void typeCheck();
    void genCode();
};

class FuncRParam : public ExprNode
{
private:
    ExprNode *param1,*param2;
public:
    FuncRParam(SymbolEntry *se,ExprNode *param1,ExprNode *param2):ExprNode(nullptr),param1(param1),param2(param2){};
    void output(int level);
    void typeCheck();
    void genCode();
    void getFuncRparamsOperand(std::vector<Operand*> &paramsVector)
    {
        param1->getFuncRparamsOperand(paramsVector);
        param2->getFuncRparamsOperand(paramsVector);
    }
};

class FuncFArrayParam : public StmtNode{
protected:
    Id *arrayId;
    ExprNode *arrayDim;
public:
    FuncFArrayParam(Id *arrayId,ExprNode *arrayDim) : arrayId(arrayId),arrayDim(arrayDim){};
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
    Id *arrayId;
    ExprNode *arrayDim;
public:
    DeclArrayStmt(Id *arrayId,ExprNode *arrayDim) : arrayId(arrayId),arrayDim(arrayDim){};
    void output(int level);
    void typeCheck();
    void genCode();
    void getArrayDimVal(std::vector<int> &a){arrayDim->getArrayDimVal(a);};
};

class DeclArrayInitStmt : public DeclArrayStmt
{
protected:
    ExprNode* initList;
public:
    DeclArrayInitStmt(Id *arrayId,ExprNode *arrayDim,ExprNode *initList) : DeclArrayStmt(arrayId,arrayDim),initList(initList){};
    void output(int level);
    void typeCheck();
    void genCode();
    void getArrayDimVal(std::vector<int> &a){arrayDim->getArrayDimVal(a);};
    std::vector<Operand*> arrayInitValOperands;
    std::vector<ExprNode*> constArrayInitNode;
};
class ArrayValDim : public ExprNode//仅用于数组初始化
{
public:
    ExprNode *dim;
    ArrayValDim(SymbolEntry *se,ExprNode *dim) : ExprNode(se),dim(dim) {};
    ArrayValDim():ExprNode(nullptr){};
    void output(int level);
    void typeCheck();
    void genCode();

    void getArrayInitValNode(std::vector<ArrayValDim*> &b);
    // {
        //b.back()->initVals.push_back(this);
    //     b.push_back(this);
    //     dim->getArrayInitValNode(b);
    //     b.pop_back();
    // }
    int getArrayInitValType(){return 1;};
    std::vector<ExprNode*> initVals;
};
class ArrayValList : public ExprNode//仅用于数组初始化
{
protected:
    ExprNode *val1,*val2;
public:
    ArrayValList(SymbolEntry *se,ExprNode *val1,ExprNode *val2) : ExprNode(se),val1(val1),val2(val2) {};
    void output(int level);
    void typeCheck();
    void genCode();

    void getArrayInitValNode(std::vector<ArrayValDim*> &b)
    {
        //b.back()->initVals.push_back(val1);
        val1->getArrayInitValNode(b);
        //b.back()->initVals.push_back(val2);
        val2->getArrayInitValNode(b);
    }
    int getArrayInitValType(){return 2;};
};

class ArrayDims : public ExprNode
{
private:
    ExprNode *dim1, *dim2;
public:
    ArrayDims(SymbolEntry *se,ExprNode *dim1, ExprNode *dim2) :ExprNode(se), dim1(dim1), dim2(dim2){};
    void output(int level);
    void typeCheck();
    void genCode();
    virtual void getArrayDimValOperand(std::vector<Operand*> &a)
    {
        dim1->getArrayDimValOperand(a);
        dim2->getArrayDimValOperand(a);
    };
};

class ConstArrayDims : public ExprNode
{
private:
    ExprNode *dim1, *dim2;
public:
    ConstArrayDims(SymbolEntry *se,ExprNode *dim1, ExprNode *dim2) :ExprNode(se), dim1(dim1), dim2(dim2){};
    void output(int level);
    void typeCheck();
    void genCode();
    void getArrayDimVal(std::vector<int> &a){dim1->getArrayDimVal(a);dim2->getArrayDimVal(a);};
};

class ConstDimVal : public ExprNode//用于数组声明
{
private:
    ExprNode *value;
public:
    ConstDimVal(SymbolEntry *se,ExprNode *value) :ExprNode(se),value(value) {};
    void output(int level);
    void typeCheck();
    void genCode();
    void getArrayDimVal(std::vector<int> &a)
    {
        a.push_back(((ConstantSymbolEntry*)symbolEntry)->getValue());
    };
};
class DimVal : public ExprNode//用于数组元素使用
{
private:
    ExprNode *value;
public:
    DimVal(SymbolEntry *se,ExprNode *value) :ExprNode(se),value(value) {dst=new Operand(se);};
    void output(int level);
    void typeCheck();
    void genCode();
    virtual void getArrayDimValOperand(std::vector<Operand*> &a){
        //a.push_back(dst);
        value->getArrayDimValOperand(a);
    };
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


