#include "Ast.h"
#include "SymbolTable.h"
#include "Unit.h"
#include "Instruction.h"
#include "IRBuilder.h"
#include <string>
#include "Type.h"

extern FILE *yyout;
int Node::counter = 0;
IRBuilder* Node::builder = nullptr;

struct s1
{
    bool inWhile=0;
    bool inFunc=0;
    
    Type * funcret;
    
    
}flag;
Node::Node()
{
    seq = counter++;
}

void Node::backPatch(std::vector<BasicBlock**> &list, BasicBlock*target)
{
    for(auto &bb:list)
        *bb = target;
}

std::vector<BasicBlock**> Node::merge(std::vector<BasicBlock**> &list1, std::vector<BasicBlock**> &list2)
{
    std::vector<BasicBlock**> res(list1);
    res.insert(res.end(), list2.begin(), list2.end());
    return res;
}

void Ast::genCode(Unit *unit)
{
    IRBuilder *builder = new IRBuilder(unit);
    Node::setIRBuilder(builder);
    root->genCode();
}

void FunctionDef::genCode()
{
    Unit *unit = builder->getUnit();
    Function *func = new Function(unit, se);
    BasicBlock *entry = func->getEntry();
    BasicBlock *exit=func->getExit();
    // set the insert point to the entry basicblock of this function.
    builder->setInsertBB(entry);


    if(paraStmt!=nullptr)
        paraStmt->genCode();
    blockStmt->genCode();

    /**
     * Construct control flow graph. You need do set successors and predecessors for each basic block.
     * Todo
    */
   std::vector<BasicBlock*> list=func->getBlockList();
   int j=list.size();
   for(int i=0;i<j;i++)
   {
        BasicBlock* &block=list[i];
        Instruction *inst=block->rbegin();
        if(inst->isCond())
        {
            CondBrInstruction *condBr=(CondBrInstruction*)(inst);
            BasicBlock *succ1,*succ2;

            succ1=condBr->getTrueBranch();
            succ2=condBr->getFalseBranch();
            block->addSucc(succ1);
            block->addSucc(succ2);
            succ1->addPred(block);
            succ2->addPred(block);
        }
        else if(inst->isUncond())
        {
            
            UncondBrInstruction *ucondBr=(UncondBrInstruction*)(inst);
            BasicBlock *succ;
            succ=ucondBr->getBranch();

            block->addSucc(succ);
            succ->addPred(block);
        }
   }
   
   exit=new BasicBlock(func);
   //builder->setInsertBB(exit);//end block?
}

void BinaryExpr::genCode()
{
    BasicBlock *bb = builder->getInsertBB();
    Function *func = bb->getParent();
    if (op == AND)
    {
        BasicBlock *trueBB = new BasicBlock(func);  // if the result of lhs is true, jump to the trueBB.
        builder->setIsGenBranch(true);
        expr1->genCode();
        backPatch(expr1->trueList(), trueBB);
        builder->setInsertBB(trueBB);               // set the insert point to the trueBB so that intructions generated by expr2 will be inserted into it.
        builder->setIsGenBranch(true);
        expr2->genCode();
        true_list = expr2->trueList();
        false_list = merge(expr1->falseList(), expr2->falseList());
    }
    else if(op == OR)  
    {
        // Todo
        BasicBlock *falseBB = new BasicBlock(func);
        builder->setIsGenBranch(true);
        expr1->genCode();
        backPatch(expr1->falseList(), falseBB);
        builder->setInsertBB(falseBB);
        builder->setIsGenBranch(true);
        expr2->genCode();
        false_list = expr2->falseList();
        true_list = merge(expr1->trueList(), expr2->trueList());
    }
    else if(op >= EQUAL && op <= GREATEREQUAL)
    {
        // Todo
        bool isGenBranch=builder->getIsGenBranch();
        builder->setIsGenBranch(false);
        expr1->genCode();
        builder->setIsGenBranch(false);
        expr2->genCode();
        Operand *src1=expr1->getOperand();
        Operand *src2=expr2->getOperand();
        int opcode;
        switch(op)
        {
            case LESS:
                opcode = CmpInstruction::L;
                break;
            case GREATER:
                opcode = CmpInstruction::G;
                break;
            case LESSEQUAL:
                opcode = CmpInstruction::LE;
                break;
            case GREATEREQUAL:
                opcode = CmpInstruction::GE;
                break;
            case NOTEQUAL:
                opcode = CmpInstruction::NE;
                break;
            case EQUAL:
                opcode = CmpInstruction::E;
                break; 
        }
        new CmpInstruction(opcode,dst,src1,src2,bb);
        if(isGenBranch)
        {
            CondBrInstruction *cond;
            cond=new CondBrInstruction(nullptr,nullptr,dst,bb);
            true_list.push_back(cond->truePatchBranch());
            false_list.push_back(cond->falsePatchBranch());
        }
    }
    else if(op >= ADD && op <= MOD)
    {
        expr1->genCode();
        expr2->genCode();
        Operand *src1 = expr1->getOperand();
        Operand *src2 = expr2->getOperand();
        int opcode;
        switch (op)
        {
            case ADD:
                opcode = BinaryInstruction::ADD;
                break;
            case SUB:
                opcode = BinaryInstruction::SUB;
                break;
            case MUL:
                opcode = BinaryInstruction::MUL;
                break;
            case DIV:
                opcode = BinaryInstruction::DIV;
                break;
            case MOD:
                opcode = BinaryInstruction::MOD;
        }
        new BinaryInstruction(opcode, dst, src1, src2, bb);
    }
}

void Constant::genCode()
{
    // we don't need to generate code.
}

void Id::genCode()
{
    BasicBlock *bb = builder->getInsertBB();
    Operand *addr = dynamic_cast<IdentifierSymbolEntry*>(symbolEntry)->getAddr();
    new LoadInstruction(dst, addr, bb);
}

void IfStmt::genCode()
{
    Function *func;
    BasicBlock *then_bb, *end_bb;

    func = builder->getInsertBB()->getParent();
    then_bb = new BasicBlock(func);
    end_bb = new BasicBlock(func);

    builder->setIsGenBranch(true);
    cond->genCode();
    builder->setIsGenBranch(false);
    backPatch(cond->trueList(), then_bb);
    backPatch(cond->falseList(), end_bb);

    builder->setInsertBB(then_bb);
    thenStmt->genCode();
    then_bb = builder->getInsertBB();
    new UncondBrInstruction(end_bb, then_bb);

    builder->setInsertBB(end_bb);
}

void IfElseStmt::genCode()
{
    // Todo
    Function *func;
    BasicBlock *thenBB,*elseBB,*endBB;
    
    func=builder->getInsertBB()->getParent();
    thenBB=new BasicBlock(func);
    elseBB=new BasicBlock(func);
    endBB=new BasicBlock(func);

    builder->setIsGenBranch(true);
    cond->genCode();
    builder->setIsGenBranch(false);
    backPatch(cond->trueList(),thenBB);
    backPatch(cond->falseList(),elseBB);

    builder->setInsertBB(thenBB);
    thenStmt->genCode();
    thenBB=builder->getInsertBB();
    new UncondBrInstruction(endBB,thenBB);

    builder->setInsertBB(elseBB);
    elseStmt->genCode();
    elseBB=builder->getInsertBB();
    new UncondBrInstruction(endBB,elseBB);

    builder->setInsertBB(endBB);
}

void CompoundStmt::genCode()
{
    // Todo
    if(stmt!=nullptr)
        stmt->genCode();
}

void SeqNode::genCode()
{
    // Todo
    stmt1->genCode();
    stmt2->genCode();
}

void DeclStmt::genCode()
{
    IdentifierSymbolEntry *se = dynamic_cast<IdentifierSymbolEntry *>(id->getSymPtr());
    if(se->isGlobal())
    {
        Operand *addr;
        SymbolEntry *addr_se;
        addr_se = new IdentifierSymbolEntry(*se);
        addr_se->setType(new PointerType(se->getType()));
        addr = new Operand(addr_se);
        se->setAddr(addr);
        //BasicBlock *bb=builder->getInsertBB();
        //new GlobalInitInstruction(new Operand(se),bb);
        int initVal=se->getInitVal();
        bool haveInitVal=se->haveInitVal;
        if(haveInitVal)
            fprintf(yyout, "@%s = global %s %d, align 4 \n",se->name.c_str(), se->type->toStr().c_str(),initVal);
        else
            fprintf(yyout, "@%s = global %s 0, align 4 \n", se->name.c_str(), se->type->toStr().c_str());
    }
    else if(se->isLocal())
    {
        Function *func = builder->getInsertBB()->getParent();
        BasicBlock *entry = func->getEntry();
        Instruction *alloca;
        Operand *addr;
        SymbolEntry *addr_se;
        Type *type;
        type = new PointerType(se->getType());
        addr_se = new TemporarySymbolEntry(type, SymbolTable::getLabel());
        addr = new Operand(addr_se);
        alloca = new AllocaInstruction(addr, se);                   // allocate space for local id in function stack.
        entry->insertFront(alloca);                             // allocate instructions should be inserted into the begin of the entry block.
        se->setAddr(addr);                                          // set the addr operand in symbol entry so that we can use it in subsequent code generation.
    }
    else if(se->isParam())
    {
        
        BasicBlock *bb=builder->getInsertBB();
        SymbolEntry *i32addr_se;
        Operand *i32addr;
        i32addr_se=new TemporarySymbolEntry(se->getType(),SymbolTable::getLabel());
        i32addr=new Operand(i32addr_se);
        new AllocaInstruction(i32addr,i32addr_se,bb);
        SymbolEntry *ad=new IdentifierSymbolEntry(*se);

        SymbolEntry* i32ptrse=new TemporarySymbolEntry(new PointerType(se->getType()),((TemporarySymbolEntry*)i32addr_se)->getTemporySymbolEntryLabel());
        
        Operand* i32ptr=new Operand(i32ptrse);
        new StoreInstruction(i32ptr,new Operand(ad),bb);
        se->setAddr(i32ptr);
    }

}

void ReturnStmt::genCode()
{
    // Todo
    BasicBlock *bb=builder->getInsertBB();
    Function *func=bb->getParent();
    BasicBlock *nextBB=new BasicBlock(func);
    Operand *src=nullptr;
    if(retValue!=nullptr)
    {
        retValue->genCode();
        src=retValue->getOperand();
    }
    new RetInstruction(src,bb);

    builder->setInsertBB(nextBB);
}

void AssignStmt::genCode()
{
    BasicBlock *bb = builder->getInsertBB();
    expr->genCode();
    Operand *addr = dynamic_cast<IdentifierSymbolEntry*>(lval->getSymPtr())->getAddr();
    Operand *src = expr->getOperand();
    /***
     * We haven't implemented array yet, the lval can only be ID. So we just store the result of the `expr` to the addr of the id.
     * If you want to implement array, you have to caculate the address first and then store the result into it.
     */
    new StoreInstruction(addr, src, bb);
}

/*************************************************************************************/
/***********************************typecheck*****************************************/

void Ast::typeCheck()
{
    if(root != nullptr)
        root->typeCheck();
}

void FunctionDef::typeCheck()
{
    flag.inFunc=1;
    if(paraStmt!=nullptr)
        paraStmt->typeCheck();

    flag.funcret=((FunctionType*)(se->getType()))->getRetType();
    
    blockStmt->typeCheck();

    flag.inFunc=0;
    // Todo
}

void BinaryExpr::typeCheck()
{
    expr1->typeCheck();
    expr2->typeCheck();
    // Todo

    Type *type1=expr1->getSymPtr()->getType();
    Type *type2=expr2->getSymPtr()->getType();
    /*
    if(!type1->isInt()||!type2->isInt()){
        printf("error type!\n");
        exit(-1);
    }
    */
   //printf("type1: %s   type2: %s\n",type1->toStr().c_str(),type2->toStr().c_str());

    if(type1->isFunc()){
        
        FunctionType* fun =(FunctionType*)type1;
        type1=fun->getRetType();

    }
        if(type2->isFunc()){
        
        FunctionType* fun =(FunctionType*)type2;
        type2=fun->getRetType();

    }
    


    if(type1==TypeSystem::voidType ||type1==TypeSystem::voidType){

        printf("binaryexpr exit void \n");
        exit(1);
    }

    IntType *t1=(IntType*)type1;
    IntType *t2=(IntType*)type2;

    if(op>=ADD&&op<=MOD)
    {
            if(!type1->isInt()||!type2->isInt()){
                printf("error type! exit no int\n");
                exit(-1);
            }

        if(type1!=type2)
        {

            IntType *t1=(IntType*)type1;
            IntType *t2=(IntType*)type2;

            printf("type1: %s type2: %s mismatch \n",type1->toStr().c_str(),type2->toStr().c_str());
            if(t1->getSize()>t2->getSize())
            {
                SymbolEntry *se=new TemporarySymbolEntry(type1,SymbolTable::getLabel());
                expr2=new Extend(se,expr2);
                symbolEntry->setType(type1);
            }
            else
            {
                SymbolEntry *se=new TemporarySymbolEntry(type2,SymbolTable::getLabel());
                expr1=new Extend(se,expr1);
                 symbolEntry->setType(type2);
            }
        }

    }
    else if(op>=EQUAL&&op<=GREATEREQUAL)
    {
         if(!type1->isInt()||!type2->isInt()){
                printf("error type! exit no int\n");
                exit(-1);
            }
        if(t1!=t2)
        {
            IntType *t1=(IntType*)type1;
            IntType *t2=(IntType*)type2;

            if(t1->getSize()>t2->getSize())
            {
                SymbolEntry *se=new TemporarySymbolEntry(type1,SymbolTable::getLabel());
                expr2=new Extend(se,expr2);
            }
            else
            {
                SymbolEntry *se=new TemporarySymbolEntry(type2,SymbolTable::getLabel());
                expr1=new Extend(se,expr1);
            }
        }
        symbolEntry->setType(TypeSystem::boolType);
    }
    else if(op==AND||op==OR)
    {
        if(!t1->isBool())
        {
            printf("type is not bool and turn to bool\n");
            SymbolEntry *se=new ConstantSymbolEntry(t1,0);
            Constant *zero=new Constant(se);
            SymbolEntry *sse=new TemporarySymbolEntry(TypeSystem::boolType,SymbolTable::getLabel());
            expr1=new BinaryExpr(sse,BinaryExpr::NOTEQUAL,expr1,zero);
        }
        if(!t2->isBool())
        {
            printf("type is not bool and turn to bool\n");
            SymbolEntry *se=new ConstantSymbolEntry(t2,0);
            Constant *zero=new Constant(se);
            SymbolEntry *sse=new TemporarySymbolEntry(TypeSystem::boolType,SymbolTable::getLabel());
            expr2=new BinaryExpr(sse,BinaryExpr::NOTEQUAL,expr2,zero);
        }
        symbolEntry->setType(TypeSystem::boolType);
    }
}

void Constant::typeCheck()
{
    // Todo
}
bool Constant::isConstantVal(int &val)
{
    ConstantSymbolEntry *sse=(ConstantSymbolEntry*)symbolEntry;
    val=sse->getValue();
    return true;
}

void Id::typeCheck()
{
    // Todo
}

void IfStmt::typeCheck()
{
    // Todo
    cond->typeCheck();
    thenStmt->typeCheck();
    IntType *t=(IntType*)cond->getSymPtr()->getType();
    if(!t->isBool())
    {
        SymbolEntry *se=new ConstantSymbolEntry(t,0);
        Constant *zero=new Constant(se);
        SymbolEntry *sse=new TemporarySymbolEntry(TypeSystem::boolType,SymbolTable::getLabel());
        cond=new BinaryExpr(sse,BinaryExpr::NOTEQUAL,cond,zero);
    }
}

void IfElseStmt::typeCheck()
{
    // Todo
    cond->typeCheck();
    thenStmt->typeCheck();
    elseStmt->typeCheck();
    IntType *t=(IntType*)cond->getSymPtr()->getType();
    if(!t->isBool())
    {
        SymbolEntry *se=new ConstantSymbolEntry(t,0);
        Constant *zero=new Constant(se);
        SymbolEntry *sse=new TemporarySymbolEntry(TypeSystem::boolType,SymbolTable::getLabel());
        cond=new BinaryExpr(sse,BinaryExpr::NOTEQUAL,cond,zero);
    }
}

void CompoundStmt::typeCheck()
{
    // Todo
    if(stmt!=nullptr)
        stmt->typeCheck();
}

void SeqNode::typeCheck()
{
    // Todo
    stmt1->typeCheck();
    stmt2->typeCheck();
}

void DeclStmt::typeCheck()
{
    // Todo
    IdentifierSymbolEntry *idse=(IdentifierSymbolEntry*)id->getSymPtr();

    /*
    SymbolEntry *se=globals->lookup(idse->toStr());
    //std::cout<<idse->toStr();
    if(se!=nullptr){

        std::cout<<idse->toStr()<<"sdsd"<<std::endl;
    }
    //printf("%d\n",identifiers->ifExitInLevel(idse->toStr()));
    if(globals->ifExitInLevel(idse->toStr(),0)){

        printf("exit  this id in the level\n");
    }
    */

    if(idse->isGlobal())
        idse->setInitVal(0);
}

void ReturnStmt::typeCheck()
{
    // Todo
    //retValue->getSymPtr()->getType()
    if(!flag.inFunc){

        printf("return not in func\n");
        exit(1);
    }
    if(flag.funcret== TypeSystem::voidType){
        printf("void return  func have other ret\n");
        exit(1);

    }
    else {

        if(retValue->getSymPtr()->getType()->isFunc()){
        FunctionType* fun =(FunctionType*)retValue->getSymPtr()->getType();
        Type * ret=fun->getRetType();
        if(!retValue ||ret!=flag.funcret){
        printf(" return  mismatch func ret\n");
        //exit(1);
        }
        }
        if(!retValue || retValue->getSymPtr()->getType()!=flag.funcret){
        printf(" return  mismatch\n");
        //exit(1);
        }
    }

    if(retValue!=nullptr)
        retValue->typeCheck();
}

void AssignStmt::typeCheck()
{
    // Todo
    lval->typeCheck();
    expr->typeCheck();

    Type* type1 = this->lval->getSymPtr()->getType();
    Type* type2 = this->expr->getSymPtr()->getType();

    if(type1->isFunc()){
        
        FunctionType* fun =(FunctionType*)type1;
        type1=fun->getRetType();

    }
        if(type2->isFunc()){
        
        FunctionType* fun =(FunctionType*)type2;
        type2=fun->getRetType();

    }
    if(type1 != type2){
        printf(
            "cannot assign object of type \'%s\' with an rvalue "
            "of type \'%s\'\n",
            type1->toStr().c_str(), type2->toStr().c_str());
        exit(EXIT_FAILURE);
    }


}

void WhileStmt::typeCheck()
{
    flag.inWhile=1;
    cond->typeCheck();
    whileStmt->typeCheck();
    flag.inWhile=0;
    IntType *t=(IntType*)cond->getSymPtr()->getType();
    if(!t->isBool())
    {
        printf("while cond is not bool\n");
        
        SymbolEntry *se=new ConstantSymbolEntry(t,0);
        Constant *zero=new Constant(se);
        SymbolEntry *sse=new TemporarySymbolEntry(TypeSystem::boolType,SymbolTable::getLabel());
        cond=new BinaryExpr(sse,BinaryExpr::NOTEQUAL,cond,zero);
        
    }
}

void UnaryExpr::typeCheck()
{
    
    expr->typeCheck();
    IntType *type = (IntType *)expr->getSymPtr()->getType();
    //printf("1111111\n");
    if(op == NOT)
    {
        //printf("sdasdasd\n");
        if(!type->isBool())
        {
            printf("not unary not bool\n");
            SymbolEntry *se = new ConstantSymbolEntry(type, 0);
            Constant *zero = new Constant(se);
            expr = new BinaryExpr(new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel()), BinaryExpr::NOTEQUAL, expr, zero);
            //Type *type = (Type *)expr->getSymPtr()->getType();
            symbolEntry->setType(TypeSystem::boolType);
            //type=TypeSystem::boolType;
            
        }
        symbolEntry->setType(TypeSystem::boolType);

    }
    else if(op == SUB)
    {
        if(!type->isInt())
        {
            printf("sub unary not int\n");
            expr = new Extend(new TemporarySymbolEntry(TypeSystem::intType ,SymbolTable::getLabel()), expr);
            symbolEntry->setType(TypeSystem::intType);
            //Type *type = (Type *)expr->getSymPtr()->getType();
            //type=TypeSystem::intType;
        }
    }
    else if(op == ADD)
    {
        if(!type->isInt())
        {
            printf("sub unary not int\n");
            expr = new Extend(new TemporarySymbolEntry(TypeSystem::intType ,SymbolTable::getLabel()), expr);
            symbolEntry->setType(TypeSystem::intType);
            //Type *type = (Type *)expr->getSymPtr()->getType();
            //type=TypeSystem::intType;
        }
    }
    
}
void ArrayElement::typeCheck()
{
    //dimValList->typeCheck();
}

void FuncCall::typeCheck()
{
  


}
void FuncRParam::typeCheck()
{

}
void FuncFArrayParam::typeCheck()
{
    
}
void EmptyStmt::typeCheck()
{
    //
}
void ExprStmt::typeCheck()
{
    
    exp->typeCheck();
}
void FuncFParam::typeCheck()
{

}
void DeclArrayStmt::typeCheck()
{

}
void ArrayDims::typeCheck()
{

}
void ArrayDim::typeCheck()
{

}
void DeclInitStmt::typeCheck()
{
    IdentifierSymbolEntry *idse=(IdentifierSymbolEntry*)id->getSymPtr();
    if(idse->isGlobal())
    {
        int val;
        if(initVal->isConstantVal(val))
        {
            idse->setInitVal(val);
        }
    }
}
void ConstDeclInitStmt::typeCheck()
{
    IdentifierSymbolEntry *idse=(IdentifierSymbolEntry*)id->getSymPtr();
    int val;
    //if(idse->isGlobal())
        if(!initVal->isConstantVal(val))
        {
            printf("constant with wrong init value!\n");
            exit(-1);
        }
    idse->setInitVal(val);
}
void DeclList::typeCheck()
{
    decl1->typeCheck();
    decl2->typeCheck();
}
void ConstDeclList::typeCheck()
{
    decl1->typeCheck();
    decl2->typeCheck();
}
void ContinueStmt::typeCheck()
{
    //
    if(!flag.inWhile){

        printf("continue not in while\n");
    }
}

void BreakStmt::typeCheck()
{
    //
        if(!flag.inWhile){

        printf("break not in while\n");
    }
}

void Extend::typeCheck()
{
    //
}



/*********************************************************************************************************/




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
bool BinaryExpr::isConstantVal(int &val)
{
    int val1,val2;
    if(!expr1->isConstantVal(val1))
        return false;
    if(!expr2->isConstantVal(val2))
        return false;
    switch(op)
    {
        case ADD:
            val=val1+val2;
            break;
        case SUB:
            val=val1-val2;
            break;
        case MUL:
            val=val1*val2;
            break;
        case DIV:
            val=val1/val2;
            break;
        case MOD:
            val=val1%val2;
            break;
    }
    return true;
}


void Ast::output()
{
    fprintf(yyout, "program\n");
    if(root != nullptr)
        root->output(4);
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
bool Id::isConstantVal(int &val)
{
    IdentifierSymbolEntry *idse=(IdentifierSymbolEntry*)symbolEntry;
    val=idse->getInitVal();
    return idse->isGlobal();//***************888??
}

void CompoundStmt::output(int level)
{
    fprintf(yyout, "%*cCompoundStmt\n", level, ' ');
    if(stmt!=nullptr)
        stmt->output(level + 4);
}

void SeqNode::output(int level)
{
    stmt1->output(level);
    stmt2->output(level);
}

void DeclStmt::output(int level)
{
    fprintf(yyout, "%*cDeclStmt\n", level, ' ');
    id->output(level + 4);
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




void WhileStmt::output(int level)
{
    fprintf(yyout, "%*cWhileStmt\n", level, ' ');
    cond->output(level + 4);
    whileStmt->output(level + 4);
}


void WhileStmt::genCode()
{
        // Todo
    Function *func;
    BasicBlock *condBB,*stmtBB,*endBB,*nowBB;
    
    func=builder->getInsertBB()->getParent();
    condBB=new BasicBlock(func);
    stmtBB=new BasicBlock(func);
    endBB=new BasicBlock(func);
    nowBB=builder->getInsertBB();
    new UncondBrInstruction(condBB,nowBB);

    builder->setInsertBB(condBB);
    builder->setIsGenBranch(true);
    cond->genCode();
    builder->setIsGenBranch(false);
    backPatch(cond->trueList(),stmtBB);
    backPatch(cond->falseList(),endBB);

    builder->setInsertBB(stmtBB);
    whileStmt->genCode();
    nowBB=builder->getInsertBB();
    new UncondBrInstruction(condBB,nowBB);

    builder->setInsertBB(endBB);
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

void UnaryExpr::genCode()
{
    bool isGenBranch=builder->getIsGenBranch();
    expr->genCode();
    if(op==SUB)
    {
        if(isGenBranch)
        {
            true_list=expr->trueList();
            false_list=expr->falseList();
        }
        else
        {
            BasicBlock *bb=builder->getInsertBB();
            SymbolEntry *se=new ConstantSymbolEntry(expr->getSymPtr()->getType(),0);
            Operand *zero=new Operand(se);
            new BinaryInstruction(BinaryInstruction::SUB,dst,zero,expr->getOperand(),bb);
        }
    }
    else if(op==ADD)
    {
        if(isGenBranch)
        {
            true_list=expr->trueList();
            false_list=expr->falseList();
        }
        else
        {
            BasicBlock *bb=builder->getInsertBB();
            SymbolEntry *se=new ConstantSymbolEntry(expr->getSymPtr()->getType(),0);
            Operand *zero=new Operand(se);
            new BinaryInstruction(BinaryInstruction::ADD,dst,zero,expr->getOperand(),bb);
        }
    }

    else if(op==NOT)
    {
        if(isGenBranch)
        {
            true_list=expr->falseList();
            false_list=expr->trueList();
        }
        else
        {
            BasicBlock *bb=builder->getInsertBB();
            SymbolEntry *se=new ConstantSymbolEntry(TypeSystem::boolType,1);
            Operand *one=new Operand(se);
            new BinaryInstruction(BinaryInstruction::XOR,dst,one,expr->getOperand(),bb);
        }
    }
}
bool UnaryExpr::isConstantVal(int &val)
{
    if(!expr->isConstantVal(val))
        return false;
    switch(op)
    {
        case ADD:
            val = val;
            break;
        case SUB:
            val =-val;
            break;
        case NOT:
            val=!val;
            break;
    }
    return true;
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

void ArrayElement::genCode()
{
    
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

void FuncCall::genCode()
{
    if(FuncRParams!=nullptr)
        FuncRParams->genCode();
    std::vector<Operand*> params;
    if(FuncRParams!=nullptr)
        FuncRParams->getFuncRparamsOperand(params);
    BasicBlock *bb=builder->getInsertBB();
    new CallInstruction(dst,params,FuncName->getSymPtr(),bb);
    //    params.push_back(FuncRParams->getOperand());
    //BasicBlock *bb=builder->getInsertBB();
}


void FuncRParam::output(int level)
{
    fprintf(yyout, "%*cFuncRealParams\n", level, ' ');
    param1->output(level + 4);
    param2->output(level + 4);
}

void FuncRParam::genCode()
{
    param1->genCode();
    param2->genCode();
}

void FuncFArrayParam::output(int level)
{
    fprintf(yyout, "%*cFunctionArrayParam\n", level, ' ');
    pointer->output(level+4);
    if(arrayDim!=nullptr)
        arrayDim->output(level+4);
}

void FuncFArrayParam::genCode()
{
    
}

void EmptyStmt::output(int level)
{
    //fprintf(yyout, "%*cEmptyStmt\n", level, ' ');
}

void EmptyStmt::genCode()
{
    //
}

void ExprStmt::output(int level)
{
    fprintf(yyout, "%*cExprStmt\n", level, ' ');
    exp->output(level + 4);

}

void ExprStmt::genCode()
{
    exp->genCode();
}

void FuncFParam::output(int level)
{
    fprintf(yyout, "%*cFuncFparamList\n", level, ' ');
    param1->output(level+4);
    param2->output(level+4);
}

void FuncFParam::genCode()
{
    param1->genCode();
    param2->genCode();
}

void DeclArrayStmt::output(int level)
{
    fprintf(yyout, "%*cArrayDeclStmt\n", level, ' ');
    pointer->output(level+4);
    arrayDim->output(level+4);
}

void DeclArrayStmt::genCode()
{
    
}

void ArrayDims::output(int level)
{
    fprintf(yyout, "%*cArrayDims\n", level, ' ');
    dim1->output(level+4);
    dim2->output(level+4);
}

void ArrayDims::genCode()
{
    
}

void ArrayDim::output(int level)
{
    fprintf(yyout, "%*cArrayDimVal\n", level, ' ');
    dimVal->output(level+4);
}

void ArrayDim::genCode()
{
    
}

void DeclInitStmt::output(int level)
{
    fprintf(yyout, "%*cDeclInitStmt\n", level, ' ');
    id->output(level + 4);
    initVal->output(level+4);
}

void DeclInitStmt::genCode()
{
    this->DeclStmt::genCode();
    IdentifierSymbolEntry *se = dynamic_cast<IdentifierSymbolEntry *>(id->getSymPtr());
    if(se->isLocal())
    {
        BasicBlock *bb=builder->getInsertBB();
        initVal->genCode();
        Operand *src=initVal->getOperand();
        new StoreInstruction(se->getAddr(),src,bb);
    }
}

void ConstDeclInitStmt::output(int level)
{
    fprintf(yyout, "%*cConstDeclInitStmt\n", level, ' ');
    id->output(level + 4);
    initVal->output(level+4);
}

void ConstDeclInitStmt::genCode()
{
    this->DeclStmt::genCode();
    IdentifierSymbolEntry *se = dynamic_cast<IdentifierSymbolEntry *>(id->getSymPtr());
    if(se->isLocal())
    {
        BasicBlock *bb=builder->getInsertBB();
        initVal->genCode();
        Operand *src=initVal->getOperand();
        new StoreInstruction(se->getAddr(),src,bb);
    }
}

void DeclList::output(int level)
{
    fprintf(yyout, "%*cDeclList\n", level, ' ');
    decl1->output(level+4);
    decl2->output(level+4);
}

void DeclList::genCode()
{
    decl1->genCode();
    decl2->genCode();
}

void ConstDeclList::output(int level)
{
    fprintf(yyout, "%*cConstDeclList\n", level, ' ');
    decl1->output(level+4);
    decl2->output(level+4);
}

void ConstDeclList::genCode()
{
    decl1->genCode();
    decl2->genCode();
}

void ContinueStmt::output(int level)
{
    fprintf(yyout, "%*cContinueStmt\n", level, ' ');
}

void ContinueStmt::genCode()
{
    //
}

void BreakStmt::output(int level)
{
    fprintf(yyout, "%*cBreakStmt\n", level, ' ');
}

void BreakStmt::genCode()
{

}

void Extend::output(int level)
{
    fprintf(yyout, "%*cExtendNode\n", level, ' ');
    originNode->output(level+4);
}

void Extend::genCode()
{
    // originNode->genCode();
    // BasicBlock *nowBB=builder->getInsertBB();
    //符号拓展语句？
    originNode->genCode();
    BasicBlock *bb=builder->getInsertBB();
    new ExtInstruction(dst, originNode->getOperand(), bb);
}
