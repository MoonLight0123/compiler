#include "MachineCode.h"
#include <list>
#include "Instruction.h"
extern FILE* yyout;

MachineOperand::MachineOperand(int tp, int val)
{
    this->type = tp;
    if(tp == MachineOperand::IMM)
        this->val = val;
    else 
        this->reg_no = val;
}

MachineOperand::MachineOperand(std::string label)
{
    this->type = MachineOperand::LABEL;
    this->label = label;
}

bool MachineOperand::operator==(const MachineOperand&a) const
{
    if (this->type != a.type)
        return false;
    if (this->type == IMM)
        return this->val == a.val;
    return this->reg_no == a.reg_no;
}

bool MachineOperand::operator<(const MachineOperand&a) const
{
    if(this->type == a.type)
    {
        if(this->type == IMM)
            return this->val < a.val;
        return this->reg_no < a.reg_no;
    }
    return this->type < a.type;

    if (this->type != a.type)
        return false;
    if (this->type == IMM)
        return this->val == a.val;
    return this->reg_no == a.reg_no;
}

void MachineOperand::PrintReg()
{
    switch (reg_no)
    {
    case 11:
        fprintf(yyout, "fp");
        break;
    case 13:
        fprintf(yyout, "sp");
        break;
    case 14:
        fprintf(yyout, "lr");
        break;
    case 15:
        fprintf(yyout, "pc");
        break;
    default:
        fprintf(yyout, "r%d", reg_no);
        break;
    }
}

void MachineOperand::output() 
{
    /* HINT：print operand
    * Example:
    * immediate num 1 -> print #1;
    * register 1 -> print r1;
    * lable addr_a -> print addr_a; */
    switch (this->type)
    {
    case IMM:
        fprintf(yyout, "#%d", this->val);
        break;
    case VREG:
        fprintf(yyout, "v%d", this->reg_no);
        break;
    case REG:
        PrintReg();
        break;
    case LABEL:
        if (this->label.substr(0, 2) == ".L")
            fprintf(yyout, "%s", this->label.c_str());
        else
            fprintf(yyout, "addr_%s", this->label.c_str());
        break;
    case FUNC:
        fprintf(yyout, "%s", this->label.c_str());
    default:
        break;
    }
}

void MachineInstruction::PrintCond()
{
    // TODO
    switch (cond)
    {
    case LT:
        fprintf(yyout, "lt");
        break;
     case LE:
        fprintf(yyout, "le");
        break;
    case EQ:
        fprintf(yyout, "eq");
        break;
    case NE:
        fprintf(yyout, "ne");
        break;
    case GT:
        fprintf(yyout, "gt");
        break;
    case GE:
        fprintf(yyout, "ge");
        break;
    default:
        break;
    }
}


BinaryMInstruction::BinaryMInstruction(
    MachineBlock* p, int op, 
    MachineOperand* dst, MachineOperand* src1, MachineOperand* src2, 
    int cond)
{
    this->parent = p;
    this->type = MachineInstruction::BINARY;
    this->op = op;
    this->cond = cond;
    this->def_list.push_back(dst);
    this->use_list.push_back(src1);
    this->use_list.push_back(src2);
    dst->setParent(this);
    src1->setParent(this);
    src2->setParent(this);
}

void BinaryMInstruction::output() 
{
    // TODO: 
    // Complete other instructions
    switch (this->op)
    {
    case BinaryMInstruction::ADD:
        fprintf(yyout, "\tadd ");
        break;
    case BinaryMInstruction::SUB:
        fprintf(yyout, "\tsub ");
        break;
    case BinaryMInstruction::MUL:
        fprintf(yyout, "\tmul ");
        break;
    case BinaryMInstruction::DIV:

        fprintf(yyout, "\tsdiv ");
        break;

    case BinaryMInstruction::AND:
        fprintf(yyout, "\tand ");
        break;
    case BinaryMInstruction::OR:
        fprintf(yyout, "\torr ");
        break;
    case BinaryMInstruction::XOR:
        fprintf(yyout, "\teor ");
        break;
    default:
        break;
    }
   
        this->PrintCond();
        this->def_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[1]->output();
        fprintf(yyout, "\n");
 
}

LoadMInstruction::LoadMInstruction(MachineBlock* p,
    MachineOperand* dst, MachineOperand* src1, MachineOperand* src2,
    int cond)
{
    this->parent = p;
    this->type = MachineInstruction::LOAD;
    this->op = -1;
    this->cond = cond;
    this->def_list.push_back(dst);
    this->use_list.push_back(src1);
    if (src2)
        this->use_list.push_back(src2);
    dst->setParent(this);
    src1->setParent(this);
    if (src2)
        src2->setParent(this);
}

void LoadMInstruction::output()
{
    fprintf(yyout, "\tldr ");
    this->def_list[0]->output();
    fprintf(yyout, ", ");

    // Load immediate num, eg: ldr r1, =8
    if(this->use_list[0]->isImm())
    {
        fprintf(yyout, "=%d\n", this->use_list[0]->getVal());
        return;
    }

    // Load address
    if(this->use_list[0]->isReg()||this->use_list[0]->isVReg())
        fprintf(yyout, "[");

    this->use_list[0]->output();
    if( this->use_list.size() > 1 )
    {
        fprintf(yyout, ", ");
        this->use_list[1]->output();
    }

    if(this->use_list[0]->isReg()||this->use_list[0]->isVReg())
        fprintf(yyout, "]");
    fprintf(yyout, "\n");
}

StoreMInstruction::StoreMInstruction(MachineBlock* p,
    MachineOperand* src1, MachineOperand* src2, MachineOperand* src3, 
    int cond)
{
    // TODO
    this->parent = p;
    this->type = MachineInstruction::STORE;
    this->op = -1;
    this->cond = cond;
    this->use_list.push_back(src1);
    this->use_list.push_back(src2);
    if (src3)
        this->use_list.push_back(src3);
    src2->setParent(this);
    src1->setParent(this);
    if (src3)
        src3->setParent(this);
}

void StoreMInstruction::output()
{
    // TODO
    fprintf(yyout, "\tstr ");
    this->use_list[0]->output();
    fprintf(yyout, ", ");

    // Load address
    if(this->use_list[1]->isReg()||this->use_list[1]->isVReg())
        fprintf(yyout, "[");

    this->use_list[1]->output();
    if( this->use_list.size() > 2 )
    {
        fprintf(yyout, ", ");
        this->use_list[2]->output();
    }

    if(this->use_list[1]->isReg()||this->use_list[1]->isVReg())
        fprintf(yyout, "]");
    fprintf(yyout, "\n");
}



MovMInstruction::MovMInstruction(MachineBlock* p, int op, 
    MachineOperand* dst, MachineOperand* src,
    int cond)
{
    // TODO
    this->parent = p;
    this->type = MachineInstruction::MOV;
    this->op = op;
    this->cond = cond;
    this->def_list.push_back(dst);
    this->use_list.push_back(src);
    dst->setParent(this);
    src->setParent(this);
}


void MovMInstruction::output() 
{
    // TODO
    fprintf(yyout, "\tmov");
    PrintCond();
    fprintf(yyout, " ");
    this->def_list[0]->output();
    fprintf(yyout, ", ");
    this->use_list[0]->output();
    fprintf(yyout, "\n");
}

BranchMInstruction::BranchMInstruction(MachineBlock* p, int op, 
    MachineOperand* dst, 
    int cond)
{
    // TODO
    this->parent = p;
    this->type = MachineInstruction::BRANCH;
    this->op = op;
    this->cond = cond;
    this->def_list.push_back(dst);
    dst->setParent(this);
}

void BranchMInstruction::output()
{
    // TODO
    switch(op){
    case B:
        fprintf(yyout, "\tb");
        PrintCond();
        fprintf(yyout, " ");
        break;
    case BL:
        fprintf(yyout, "\tbl ");
        break;
    case BX://弃用
        fprintf(yyout, "\tbx ");
        break;
    }
    this->def_list[0]->output();
    fprintf(yyout, "\n");
    if(op==BX)fprintf(yyout, "\n");
}

CmpMInstruction::CmpMInstruction(MachineBlock* p, 
    MachineOperand* src1, MachineOperand* src2, 
    int cond)
{
    // TODO
    this->parent = p;
    this->type = MachineInstruction::CMP;
    this->cond = cond;
    this->use_list.push_back(src1);
    this->use_list.push_back(src2);
    src1->setParent(this);
    src2->setParent(this);
}

void CmpMInstruction::output()
{
    // TODO
    // Jsut for reg alloca test
    // delete it after test
    fprintf(yyout, "\tcmp ");
    this->use_list[0]->output();
    fprintf(yyout, ", ");
    this->use_list[1]->output();
    fprintf(yyout, "\n");
}

StackMInstrcuton::StackMInstrcuton(MachineBlock* p, int op, 
    MachineOperand* src,
    int cond)
{
    // TODO
    this->parent = p;
    this->type = MachineInstruction::STACK;
    this->op = op;
    this->cond = cond;
    //this->use_list = src;
    this->use_list.push_back(src);
    for(auto reg : use_list){
        if(reg!=nullptr)
            reg->setParent(this);
    }
}
StackMInstrcuton::StackMInstrcuton(int op, int cond)
{
    // TODO
    this->type = MachineInstruction::STACK;
    this->op = op;
    this->cond = cond;
    //this->use_list = src;
}

void StackMInstrcuton::output()
{
    // TODO
    switch(op){
    case PUSH:
        fprintf(yyout, "\tpush {");
        break;
    case POP:
        fprintf(yyout, "\tpop {");
        break;
    }
    bool is_first = true;
    for(auto reg : use_list){
        if(reg==nullptr)continue;
        if(!is_first)
            fprintf(yyout, ", ");
        else
            is_first = false;
        reg->output();
    }
    fprintf(yyout, "}\n");
}

ExtMInstruction::ExtMInstruction(MachineBlock* p,MachineOperand* dst,
                MachineOperand* src,
                int cond)
{
    this->parent=p;
    this->cond=cond;
    this->use_list.push_back(src);
    this->def_list.push_back(dst);
    src->setParent(this);
    dst->setParent(this);
}

void ExtMInstruction::output()
{
    fprintf(yyout, "\tuxtb ");
    def_list[0]->output();
    fprintf(yyout, ", ");
    use_list[0]->output();
    fprintf(yyout, "\n");
}

MachineFunction::MachineFunction(MachineUnit* p, SymbolEntry* sym_ptr) 
{ 
    this->parent = p; 
    this->sym_ptr = sym_ptr; 
    this->stack_size = 0;
    funcPopInst=new StackMInstrcuton(StackMInstrcuton::POP);
};

void MachineBlock::output()
{
    fprintf(yyout, ".L%d:\n", this->no);
    for(auto iter : inst_list)
        iter->output();
}

void MachineBlock::insertBefore(MachineInstruction* at, MachineInstruction* src)
{
    std::vector<MachineInstruction*>::iterator pos = find(inst_list.begin(), inst_list.end(), at);
    inst_list.insert(pos, src);
}

void MachineBlock::insertAfter(MachineInstruction* at, MachineInstruction* src)
{
    std::vector<MachineInstruction*>::iterator pos = find(inst_list.begin(), inst_list.end(), at);
    ++pos;
    inst_list.insert(pos, src);
}


void MachineFunction::output()
{
    const char *func_name = this->sym_ptr->toStr().c_str() + 1;
    fprintf(yyout, "\t.global %s\n", func_name);
    fprintf(yyout, "\t.type %s , %%function\n", func_name);
    fprintf(yyout, "%s:\n", func_name);
    // TODO
    /* Hint:
    *  1. Save fp
    *  2. fp = sp
    *  3. Save callee saved register
    *  4. Allocate stack space for local variable */
    //1. Save fp
    fprintf(yyout,"\tpush {");
    Instruction* temp=new DummyInstruction();
    //if(funcPopInst->use_list[0]==nullptr)printf("55555555555555555500");
    //funcPopInst->use_list.pop_back();
    //printf("size:%d",funcPopInst->use_list.size());
    auto fp=temp->genMachineReg(11);
    auto lr=temp->genMachineReg(14);
    for(auto &reg:saved_regs)
    {
        auto regOperand=temp->genMachineReg(reg);
        regOperand->output();
        funcPopInst->addUse(regOperand);
        fprintf(yyout,", ");
    }
    fprintf(yyout,"fp, lr}\n");
    funcPopInst->addUse(fp);
    funcPopInst->addUse(lr);
    //printf("size:%d",funcPopInst->use_list.size());
    fprintf(yyout,"\tmov fp, sp\n");

    fprintf(yyout,"\tsub sp ,sp ,#%d\n",stack_size);

    //funcPopInst->use_list.pop_back();

    // Traverse all the block in block_list to print assembly code.
    std::set<MachineBlock *> v;
    std::list<MachineBlock *> q;
    q.push_back(block_list[0]);
    v.insert(block_list[0]);
    while (!q.empty())
    {
        auto bb = q.front();
        q.pop_front();
        bb->output();
        for (auto succ = bb->getSuccs().begin(); succ != bb->getSuccs().end(); succ++)
        {
            if (v.find(*succ) == v.end())//如果succ不在v中
            {
                v.insert(*succ);
                q.push_back(*succ);
            }
        }
    }



}

void MachineUnit::PrintGlobalDecl()
{
    // TODO:
    // You need to print global variable/const declarition code;
    fprintf(yyout,"\t.data\n");
    for(auto &globId:globIds)
    {
        fprintf(yyout,"\t.global %s\n",globId->name.c_str());
        fprintf(yyout,"\t.align 4\n");
        fprintf(yyout,"\t.size %s, %d\n",globId->name.c_str(),globId->getType()->getSize()/8);
        fprintf(yyout,"%s:\n",globId->name.c_str());

        //现在只支持int32的全局变量
        if(globId->getType()==TypeSystem::intType)//全局变量必有初值
            fprintf(yyout,"\t.word %d\n",globId->initVal);

        if(globId->getType()->isArray())
        {
            ArrayType *at=(ArrayType*)globId->getType();
            if(globId->haveInitVal)//声明时的形式为a[10]={11,};
            {
                for(auto i:at->constArrayInitVal)
                    fprintf(yyout,"\t.word %d\n",i);
            }
            else //声明时的形式为a[10];默认初值为0
            {
                int elementNum=1;
                for(auto i:at->dimsVal)
                    elementNum*=i;
                for(int i=0;i<elementNum;i++)
                    fprintf(yyout,"\t.word 0\n");
            }
        }
    }
}

void MachineUnit::printBridge()
{
    for(auto &globId:globIds)
    {
        fprintf(yyout,"addr_%s:\n",globId->name.c_str());
        fprintf(yyout,"\t.word %s\n",globId->name.c_str());
    }
}
void MachineUnit::output()
{
    // TODO
    /* Hint:
    * 1. You need to print global variable/const declarition code;
    * 2. Traverse all the function in func_list to print assembly code;
    * 3. Don't forget print bridge label at the end of assembly code!! */
    fprintf(yyout, "\t.arch armv8-a\n");
    fprintf(yyout, "\t.arch_extension crc\n");
    fprintf(yyout, "\t.arm\n");
    PrintGlobalDecl();
    for(auto iter : func_list)
        iter->output();
    printBridge();
}



