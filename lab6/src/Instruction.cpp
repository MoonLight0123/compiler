#include "Instruction.h"
#include "BasicBlock.h"
#include <iostream>
#include "Function.h"
#include "Type.h"
extern FILE *yyout;

Instruction::Instruction(unsigned instType, BasicBlock *insert_bb)
{
    prev = next = this;
    opcode = -1;
    this->instType = instType;
    if (insert_bb != nullptr)
    {
        insert_bb->insertBack(this);
        parent = insert_bb;
    }
}

Instruction::~Instruction()
{
    parent->remove(this);
}

BasicBlock *Instruction::getParent()
{
    return parent;
}

void Instruction::setParent(BasicBlock *bb)
{
    parent = bb;
}

void Instruction::setNext(Instruction *inst)
{
    next = inst;
}

void Instruction::setPrev(Instruction *inst)
{
    prev = inst;
}

Instruction *Instruction::getNext()
{
    return next;
}

Instruction *Instruction::getPrev()
{
    return prev;
}

BinaryInstruction::BinaryInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2, BasicBlock *insert_bb) : Instruction(BINARY, insert_bb)
{
    this->opcode = opcode;
    operands.push_back(dst);
    operands.push_back(src1);
    operands.push_back(src2);
    dst->setDef(this);
    src1->addUse(this);
    src2->addUse(this);
}

BinaryInstruction::~BinaryInstruction()
{
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
    operands[2]->removeUse(this);
}

void BinaryInstruction::output() const
{
    std::string s1, s2, s3, op, type;
    s1 = operands[0]->toStr();
    s2 = operands[1]->toStr();
    s3 = operands[2]->toStr();
    type = operands[0]->getType()->toStr();
    switch (opcode)
    {
    case ADD:
        op = "add";
        break;
    case SUB:
        op = "sub";
        break;
    case MUL:
        op = "mul";
        break;
    case DIV:
        op = "sdiv";
        break;
    case MOD:
        op = "srem";
        break;
    case XOR:
        op = "xor";
        break;
    default:
        break;
    }
    fprintf(yyout, "  %s = %s %s %s, %s\n", s1.c_str(), op.c_str(), type.c_str(), s2.c_str(), s3.c_str());
}

CmpInstruction::CmpInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2, BasicBlock *insert_bb) : Instruction(CMP, insert_bb)
{
    this->opcode = opcode;
    operands.push_back(dst);
    operands.push_back(src1);
    operands.push_back(src2);
    dst->setDef(this);
    src1->addUse(this);
    src2->addUse(this);
}

CmpInstruction::~CmpInstruction()
{
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
    operands[2]->removeUse(this);
}

void CmpInstruction::output() const
{
    std::string s1, s2, s3, op, type;
    s1 = operands[0]->toStr();
    s2 = operands[1]->toStr();
    s3 = operands[2]->toStr();
    type = operands[1]->getType()->toStr();
    switch (opcode)
    {
    case E:
        op = "eq";
        break;
    case NE:
        op = "ne";
        break;
    case L:
        op = "slt";
        break;
    case LE:
        op = "sle";
        break;
    case G:
        op = "sgt";
        break;
    case GE:
        op = "sge";
        break;
    default:
        op = "";
        break;
    }

    fprintf(yyout, "  %s = icmp %s %s %s, %s\n", s1.c_str(), op.c_str(), type.c_str(), s2.c_str(), s3.c_str());
}

UncondBrInstruction::UncondBrInstruction(BasicBlock *to, BasicBlock *insert_bb) : Instruction(UNCOND, insert_bb)
{
    branch = to;
}

void UncondBrInstruction::output() const
{
    fprintf(yyout, "  br label %%B%d\n", branch->getNo());
}

void UncondBrInstruction::setBranch(BasicBlock *bb)
{
    branch = bb;
}

BasicBlock *UncondBrInstruction::getBranch()
{
    return branch;
}

CondBrInstruction::CondBrInstruction(BasicBlock *true_branch, BasicBlock *false_branch, Operand *cond, BasicBlock *insert_bb) : Instruction(COND, insert_bb)
{
    this->true_branch = true_branch;
    this->false_branch = false_branch;
    cond->addUse(this);
    operands.push_back(cond);
}

CondBrInstruction::~CondBrInstruction()
{
    operands[0]->removeUse(this);
}

void CondBrInstruction::output() const
{
    std::string cond, type;
    cond = operands[0]->toStr();
    type = operands[0]->getType()->toStr();
    int true_label = true_branch->getNo();
    int false_label = false_branch->getNo();
    fprintf(yyout, "  br %s %s, label %%B%d, label %%B%d\n", type.c_str(), cond.c_str(), true_label, false_label);
}

void CondBrInstruction::setFalseBranch(BasicBlock *bb)
{
    false_branch = bb;
}

BasicBlock *CondBrInstruction::getFalseBranch()
{
    return false_branch;
}

void CondBrInstruction::setTrueBranch(BasicBlock *bb)
{
    true_branch = bb;
}

BasicBlock *CondBrInstruction::getTrueBranch()
{
    return true_branch;
}

RetInstruction::RetInstruction(Operand *src, BasicBlock *insert_bb) : Instruction(RET, insert_bb)
{
    if (src != nullptr)
    {
        operands.push_back(src);
        src->addUse(this);
    }
}

RetInstruction::~RetInstruction()
{
    if (!operands.empty())
        operands[0]->removeUse(this);
}

void RetInstruction::output() const
{
    if (operands.empty())
    {
        fprintf(yyout, "  ret void\n");
    }
    else
    {
        std::string ret, type;
        ret = operands[0]->toStr();
        type = operands[0]->getType()->toStr();
        fprintf(yyout, "  ret %s %s\n", type.c_str(), ret.c_str());
    }
}

AllocaInstruction::AllocaInstruction(Operand *dst, SymbolEntry *se, BasicBlock *insert_bb) : Instruction(ALLOCA, insert_bb)
{
    operands.push_back(dst);
    dst->setDef(this);
    this->se = se;
}

AllocaInstruction::~AllocaInstruction()
{
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
}

void AllocaInstruction::output() const
{
    std::string dst, type;
    dst = operands[0]->toStr();
    type = se->getType()->toStr();
    fprintf(yyout, "  %s = alloca %s, align 4\n", dst.c_str(), type.c_str());
}

LoadInstruction::LoadInstruction(Operand *dst, Operand *src_addr, BasicBlock *insert_bb) : Instruction(LOAD, insert_bb)
{
    operands.push_back(dst);
    operands.push_back(src_addr);
    dst->setDef(this);
    src_addr->addUse(this);
}

LoadInstruction::~LoadInstruction()
{
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
}

void LoadInstruction::output() const
{
    std::string dst = operands[0]->toStr();
    std::string src = operands[1]->toStr();
    std::string src_type;
    std::string dst_type;
    dst_type = operands[0]->getType()->toStr();
    src_type = operands[1]->getType()->toStr();
    fprintf(yyout, "  %s = load %s, %s %s, align 4\n", dst.c_str(), dst_type.c_str(), src_type.c_str(), src.c_str());
}

StoreInstruction::StoreInstruction(Operand *dst_addr, Operand *src, BasicBlock *insert_bb) : Instruction(STORE, insert_bb)
{
    operands.push_back(dst_addr);
    operands.push_back(src);
    dst_addr->addUse(this);
    src->addUse(this);
}

StoreInstruction::~StoreInstruction()
{
    operands[0]->removeUse(this);
    operands[1]->removeUse(this);
}

void StoreInstruction::output() const
{
    std::string dst = operands[0]->toStr();
    std::string src = operands[1]->toStr();
    std::string dst_type = operands[0]->getType()->toStr();
    std::string src_type = operands[1]->getType()->toStr();

    fprintf(yyout, "  store %s %s, %s %s, align 4\n", src_type.c_str(), src.c_str(), dst_type.c_str(), dst.c_str());
}

CallInstruction::CallInstruction(Operand *dst, std::vector<Operand *> &params, SymbolEntry *se, BasicBlock *insert_bb) : Instruction(CALL, insert_bb)
{
    this->se = se;
    this->opcode = opcode;
    operands.push_back(dst);
    operands.insert(operands.end(), params.begin(), params.end());
    if (dst != nullptr)
        dst->setDef(this);
    for (auto &use : params)
        use->addUse(this);
}

CallInstruction::~CallInstruction()
{
    if (operands[0] != nullptr)
        operands[0]->setDef(nullptr);
    for (size_t i = 1; i < operands.size(); i++)
    {
        auto operand = operands[i];
        operand->removeUse(this);
    }
}

void CallInstruction::output() const
{
    fprintf(yyout, "  ");
    if (operands[0] != nullptr)
    {
        std::string str = operands[0]->toStr();
        FunctionType *func = (FunctionType *)operands[0]->getType();

        if (func->getRetType() != TypeSystem::voidType)
            fprintf(yyout, "%s = ", str.c_str());
    }
    FunctionType *funcType = dynamic_cast<FunctionType *>(se->getType());
    Type *retType = funcType->getRetType();
    fprintf(yyout, "call %s %s", retType->toStr().c_str(), se->toStr().c_str());
    if (operands.size() == 1)
        fprintf(yyout, "()\n");
    else
    {
        std::string s;
        s = operands[1]->toStr();
        fprintf(yyout, "(%s %s", operands[1]->getType()->toStr().c_str(), s.c_str());
        for (size_t i = 2; i < operands.size(); i++)
        {
            s = operands[i]->toStr();
            fprintf(yyout, ", %s %s", operands[i]->getType()->toStr().c_str(), s.c_str());
        }
        fprintf(yyout, ")\n");
    }
}

ExtInstruction::ExtInstruction(Operand *dst, Operand *src, BasicBlock *insert_bb) : Instruction(EXT, insert_bb)
{
    operands.push_back(dst);
    operands.push_back(src);
    dst->setDef(this);
    src->addUse(this);
}

void ExtInstruction::output() const
{
    std::string dst, src, dst_type, src_type;
    dst = operands[0]->toStr();
    src = operands[1]->toStr();
    dst_type = operands[0]->getType()->toStr();
    src_type = operands[1]->getType()->toStr();
    fprintf(yyout, "  %s = zext %s %s to %s\n", dst.c_str(), src_type.c_str(), src.c_str(), dst_type.c_str());
}

MachineOperand *Instruction::genMachineOperand(Operand *ope)
{
    auto se = ope->getEntry();
    MachineOperand *mope = nullptr;
    if (se->isConstant())
        mope = new MachineOperand(MachineOperand::IMM, dynamic_cast<ConstantSymbolEntry *>(se)->getValue());
    else if (se->isTemporary())
        mope = new MachineOperand(MachineOperand::VREG, dynamic_cast<TemporarySymbolEntry *>(se)->getLabel());
    else if (se->isVariable())
    {
        auto id_se = dynamic_cast<IdentifierSymbolEntry *>(se);
        if (id_se->isGlobal())
            mope = new MachineOperand(id_se->toStr().erase(0, 1).c_str());
        else if (id_se->isParam())
        {
            if (id_se->paramNo < 4)
            {
                mope = genMachineReg(id_se->paramNo);
            }
            else // 其余情况为参数大于4个要从栈中取
            {
            }
        }
        else
            exit(0);
    }
    return mope;
}

MachineOperand *Instruction::genMachineReg(int reg)
{
    return new MachineOperand(MachineOperand::REG, reg);
}

MachineOperand *Instruction::genMachineVReg()
{
    return new MachineOperand(MachineOperand::VREG, SymbolTable::getLabel());
}

MachineOperand *Instruction::genMachineImm(int val)
{
    return new MachineOperand(MachineOperand::IMM, val);
}

MachineOperand *Instruction::genMachineLabel(int block_no)
{
    std::ostringstream buf;
    buf << ".L" << block_no;
    std::string label = buf.str();
    return new MachineOperand(label);
}

void AllocaInstruction::genMachineCode(AsmBuilder *builder)
{
    /* HINT:
     * Allocate stack space for local variabel
     * Store frame offset in symbol entry */
    auto cur_func = builder->getFunction();
    int offset = cur_func->AllocSpace(4);
    dynamic_cast<TemporarySymbolEntry *>(operands[0]->getEntry())->setOffset(-offset);
}

void LoadInstruction::genMachineCode(AsmBuilder *builder)
{
    auto cur_block = builder->getBlock();
    MachineInstruction *cur_inst = nullptr;
    // Load global operand
    if (operands[1]->getEntry()->isVariable() && dynamic_cast<IdentifierSymbolEntry *>(operands[1]->getEntry())->isGlobal())
    { // 全局变量要首先将其地址加载入一个寄存器中，然后再一次ldr加载到目标寄存器中
        auto dst = genMachineOperand(operands[0]);
        auto internal_reg1 = genMachineVReg();
        auto internal_reg2 = new MachineOperand(*internal_reg1);
        auto src = genMachineOperand(operands[1]);
        // example: load r0, addr_a
        cur_inst = new LoadMInstruction(cur_block, internal_reg1, src);
        cur_block->InsertInst(cur_inst);
        // example: load r1, [r0]
        cur_inst = new LoadMInstruction(cur_block, dst, internal_reg2);
        cur_block->InsertInst(cur_inst);
    }
    // Load local operand
    else if (operands[1]->getEntry()->isTemporary() && operands[1]->getDef() && operands[1]->getDef()->isAlloc())
    {
        // example: load r1, [r0, #4]
        auto dst = genMachineOperand(operands[0]);
        auto src1 = genMachineReg(11);
        auto src2 = genMachineImm(dynamic_cast<TemporarySymbolEntry *>(operands[1]->getEntry())->getOffset());
        cur_inst = new LoadMInstruction(cur_block, dst, src1, src2);
        cur_block->InsertInst(cur_inst);
    }
    // Load operand from temporary variable
    else
    {
        // example: load r1, [r0]
        auto dst = genMachineOperand(operands[0]);
        auto src = genMachineOperand(operands[1]);
        cur_inst = new LoadMInstruction(cur_block, dst, src);
        cur_block->InsertInst(cur_inst);
    }
}

void StoreInstruction::genMachineCode(AsmBuilder *builder)
{
    // TODO
    // 3种情况，储存一寄存器中的值到一个全局变量中，储存到栈中的变量中
    auto cur_block = builder->getBlock();
    MachineInstruction *cur_inst = nullptr;
    if (operands[0]->getEntry()->isVariable() && dynamic_cast<IdentifierSymbolEntry *>(operands[0]->getEntry())->isGlobal())
    {
        auto dst = genMachineOperand(operands[0]);
        auto internal_reg1 = genMachineVReg();
        auto internal_reg2 = new MachineOperand(*internal_reg1);
        auto src = genMachineOperand(operands[1]);
        // example: load r0, addr_a
        cur_inst = new LoadMInstruction(cur_block, internal_reg1, dst);
        cur_block->InsertInst(cur_inst);
        // example: sdr r1, [r0]
        if (src->isImm())
        {
            auto internal_reg = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block, internal_reg, src);
            cur_block->InsertInst(cur_inst);
            src = new MachineOperand(*internal_reg);
        }
        cur_inst = new StoreMInstruction(cur_block, src, internal_reg2);
        cur_block->InsertInst(cur_inst);
    }
    // Store local operand
    else if (operands[0]->getEntry()->isTemporary() && operands[0]->getDef() && operands[0]->getDef()->isAlloc())
    {
        // example: store r1, [r0, #4]
        auto src = genMachineOperand(operands[1]); // 这里没有写函数生成operand
        if (src->isImm())
        {
            auto internal_reg = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block, internal_reg, src);
            cur_block->InsertInst(cur_inst);
            src = new MachineOperand(*internal_reg);
        }
        auto src1 = genMachineReg(11);
        auto src2 = genMachineImm(dynamic_cast<TemporarySymbolEntry *>(operands[0]->getEntry())->getOffset()); // dstaddr相对fp的偏移量
        cur_inst = new StoreMInstruction(cur_block, src, src1, src2);
        cur_block->InsertInst(cur_inst);
    }
    // Store operand to temporary variable
    else
    {
        // example: Store r1, [r0]
        auto dst = genMachineOperand(operands[0]);
        auto src = genMachineOperand(operands[1]);
        cur_inst = new StoreMInstruction(cur_block, dst, src);
        cur_block->InsertInst(cur_inst);
    }
}

void BinaryInstruction::genMachineCode(AsmBuilder *builder)
{
    // TODO:
    // complete other instructions
    auto cur_block = builder->getBlock();
    auto dst = genMachineOperand(operands[0]);
    auto src1 = genMachineOperand(operands[1]);
    auto src2 = genMachineOperand(operands[2]);
    /* HINT:
     * The source operands of ADD instruction in ir code both can be immediate num.
     * However, it's not allowed in assembly code.
     * So you need to insert LOAD/MOV instrucrion to load immediate num into register.
     * As to other instructions, such as MUL, CMP, you need to deal with this situation, too.*/
    MachineInstruction *cur_inst = nullptr;
    if (src1->isImm())
    {
        auto internal_reg = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block, internal_reg, src1);
        cur_block->InsertInst(cur_inst);
        src1 = new MachineOperand(*internal_reg);
    }
    if (src2->isImm())
    {
        auto internal_reg = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block, internal_reg, src2);
        cur_block->InsertInst(cur_inst);
        src2 = new MachineOperand(*internal_reg);
    }
    switch (opcode)
    {
    case ADD:
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, dst, src1, src2);
        break;
    case SUB:
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::SUB, dst, src1, src2);
        break;
    case MUL:
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::MUL, dst, src1, src2);
        break;
    case DIV:
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::DIV, dst, src1, src2);
        break;
    // case MOD:
    //     cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::MOD, dst, src1, src2);
    //     break;
    // a % b = a - a / b * b
    case MOD:
    {
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::DIV, dst, src1, src2);
        MachineOperand *dst1 = new MachineOperand(*dst);
        src1 = new MachineOperand(*src1);
        src2 = new MachineOperand(*src2);
        cur_block->InsertInst(cur_inst);
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::MUL, dst1, dst, src2);
        cur_block->InsertInst(cur_inst);
        dst = new MachineOperand(*dst1);
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::SUB, dst, src1, dst1);
    }
    break;
    case AND:
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::AND, dst, src1, src2);
        break;
    case OR:
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::OR, dst, src1, src2);
        break;
    case XOR:
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::XOR, dst, src1, src2);
        break;
    default:
        break;
    }
    cur_block->InsertInst(cur_inst);
}

void CmpInstruction::genMachineCode(AsmBuilder *builder)
{
    // TODO
    auto cur_block = builder->getBlock();
    auto src1 = genMachineOperand(operands[1]);
    auto src2 = genMachineOperand(operands[2]);
    MachineInstruction *cur_inst = nullptr;
    if (src1->isImm())
    {
        auto internal_reg = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block, internal_reg, src1);
        cur_block->InsertInst(cur_inst);
        src1 = new MachineOperand(*internal_reg);
    }
    if (src2->isImm())
    {
        auto internal_reg = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block, internal_reg, src2);
        cur_block->InsertInst(cur_inst);
        src2 = new MachineOperand(*internal_reg);
    }
    builder->setCmpOpcode(this->opcode);
    cur_inst = new CmpMInstruction(cur_block, src1, src2);
    cur_block->InsertInst(cur_inst);

    // cur_inst = new CmpMInstruction(cur_block, src1, src2, opcode);
    // cur_block->InsertInst(cur_inst);
    // cur_block->setCurrentBranchCond(opcode);
    // 采用条件存储的方式将1/0存储到dst中
    MachineOperand *dst = genMachineOperand(operands[0]);
    MachineOperand *trueOperand = genMachineImm(1);
    MachineOperand *falseOperand = genMachineImm(0);
    cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, trueOperand, opcode);
    cur_block->InsertInst(cur_inst);
    if (opcode == CmpInstruction::E || opcode == CmpInstruction::NE)
    {
        cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, falseOperand, 1-opcode);
        cur_block->InsertInst(cur_inst);
    
    }

    else
    {
        cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, falseOperand, 7 - opcode);
        cur_block->InsertInst(cur_inst);
    }
    // cur_block->InsertInst(cur_inst);
}

void UncondBrInstruction::genMachineCode(AsmBuilder *builder)
{
    // TODO
    auto cur_block = builder->getBlock();
    MachineInstruction *cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::B, genMachineLabel(branch->getNo()));
    cur_block->InsertInst(cur_inst);
}

void CondBrInstruction::genMachineCode(AsmBuilder *builder)
{
    // TODOenum {E, NE, L, GE, G, LE};
    auto cur_block = builder->getBlock();
    int cmpOpcode = builder->getCmpOpcode();
    MachineInstruction *cur_inst = nullptr;
    switch (cmpOpcode)
    {
    case CmpInstruction::E:
        cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::B, genMachineLabel(true_branch->getNo()), MachineInstruction::EQ);
        break;
    case CmpInstruction::NE:
        cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::B, genMachineLabel(true_branch->getNo()), MachineInstruction::NE);
        break;
    case CmpInstruction::L:
        cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::B, genMachineLabel(true_branch->getNo()), MachineInstruction::LT);
        break;
    case CmpInstruction::GE:
        cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::B, genMachineLabel(true_branch->getNo()), MachineInstruction::GE);
        break;
    case CmpInstruction::LE:
        cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::B, genMachineLabel(true_branch->getNo()), MachineInstruction::LE);
        break;
    case CmpInstruction::G:
        cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::B, genMachineLabel(true_branch->getNo()), MachineInstruction::GT);
        break;
    default:
        break;
    }
    cur_block->InsertInst(cur_inst);
    cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::B, genMachineLabel(false_branch->getNo()));
    cur_block->InsertInst(cur_inst);
}

void RetInstruction::genMachineCode(AsmBuilder *builder)
{
    // TODO
    /* HINT:
     * 1. Generate mov instruction to save return value in r0
     * 2. Restore callee saved registers and sp, fp
     * 3. Generate bx instruction */
    auto cur_block = builder->getBlock();
    MachineInstruction *cur_inst = nullptr;
    if (!operands.empty())
    {
        auto ret = genMachineOperand(operands[0]);
        if (ret->isImm())
        {
            auto internal_reg = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block, internal_reg, ret);
            cur_block->InsertInst(cur_inst);
            ret = new MachineOperand(*internal_reg);
        }
        auto r0 = genMachineReg(0);
        cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, r0, ret);
        cur_block->InsertInst(cur_inst);
    }
    auto fp = genMachineReg(11);
    auto lr = genMachineReg(14);
    auto sp = genMachineReg(13);
    auto offset = genMachineImm(builder->getFunction()->getStackSize());
    cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, sp, sp, offset);
    cur_block->InsertInst(cur_inst);
    cur_inst = builder->getFunction()->funcPopInst;
    cur_inst->setParent(cur_block);
    cur_block->InsertInst(cur_inst);
    cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::BX, lr);
    cur_block->InsertInst(cur_inst);
    // 待更新//if("")//叶与非叶函数不同
}

void CallInstruction::genMachineCode(AsmBuilder *bulider)
{
    auto cur_block = bulider->getBlock();
    MachineInstruction *cur_inst = nullptr;
    int argsNum = operands.size() - 1;
    if (argsNum <= 4)
    {
        for (int i = 0; i < argsNum; i++)
        {
            auto dst = genMachineReg(i);
            // operands[0]函数返回值，参数从下标1开始
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, genMachineOperand(operands[i + 1]));
            cur_block->InsertInst(cur_inst);
        }
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
            auto dst = genMachineReg(i);
            // operands[0]函数返回值，参数从下标1开始
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, genMachineOperand(operands[i + 1]));
            cur_block->InsertInst(cur_inst);
        }
        // 其余参数逆序压栈，参数列表中最右的参数最先压栈
        for (int i = argsNum; i >= 5; i--)
        {
            auto param = genMachineOperand(operands[i]);
            cur_inst = new StackMInstrcuton(cur_block, StackMInstrcuton::PUSH, param);
            cur_block->InsertInst(cur_inst);
        }
    }
    auto funName = ((IdentifierSymbolEntry *)se)->name.c_str();
    cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::BL, new MachineOperand(MachineOperand::FUNC, funName));
    cur_block->InsertInst(cur_inst);

    // 若返回值不是void，则mov ret,r0
    if (((FunctionType *)se->getType())->getRetType() != TypeSystem::voidType)
    {
        auto src = genMachineReg(0);
        auto dst = genMachineOperand(operands[0]);
        cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, src);
        cur_block->InsertInst(cur_inst);
    }

    // 压栈的时候将sp降低，现在要恢复，即sp=sp+(argsNum-4)*4
    if (argsNum > 4)
    {
        auto sp = genMachineReg(13);
        auto offset = genMachineImm((argsNum - 4) * 4);
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, sp, sp, offset);
        cur_block->InsertInst(cur_inst);
    }
}

void ExtInstruction::genMachineCode(AsmBuilder *builder)
{
    MachineBlock *cur_block = builder->getBlock();
    MachineInstruction *cur_inst = nullptr;
    MachineOperand *src = genMachineOperand(operands[1]);
    if (src->isImm())
    {
        auto internal_reg = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block, internal_reg, src);
        cur_block->InsertInst(cur_inst);
        src = new MachineOperand(*internal_reg);
    }
    MachineOperand *dst = genMachineOperand(operands[0]);
    if (dst->isImm())
    {
        auto internal_reg = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block, internal_reg, dst);
        cur_block->InsertInst(cur_inst);
        dst = new MachineOperand(*internal_reg);
    }
    cur_inst = new ExtMInstruction(cur_block, dst, src);
    cur_block->InsertInst(cur_inst);
}
