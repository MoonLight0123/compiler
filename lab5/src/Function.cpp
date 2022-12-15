#include "Function.h"
#include "Unit.h"
#include "Type.h"
#include <list>

extern FILE* yyout;

Function::Function(Unit *u, SymbolEntry *s)
{
    u->insertFunc(this);
    entry = new BasicBlock(this);
    exit=nullptr;
    sym_ptr = s;
    parent = u;
}

Function::~Function()
{
    auto delete_list = block_list;
    for (auto &i : delete_list)
        delete i;
    parent->removeFunc(this);
}

// remove the basicblock bb from its block_list.
void Function::remove(BasicBlock *bb)
{
    block_list.erase(std::find(block_list.begin(), block_list.end(), bb));
}

void Function::output() const
{
    FunctionType* funcType = dynamic_cast<FunctionType*>(sym_ptr->getType());
    Type *retType = funcType->getRetType();
    std::vector<SymbolEntry*> &paramsSymbolEntry=funcType->paramsSymbolEntry;
    if(paramsSymbolEntry.empty())
        fprintf(yyout, "define %s %s() {\n", retType->toStr().c_str(), sym_ptr->toStr().c_str());
    else
    {
        std::string name, type;
        name = paramsSymbolEntry[0]->toStr();
        type = paramsSymbolEntry[0]->getType()->toStr();
        fprintf(yyout, "define %s %s(%s %s", retType->toStr().c_str(), sym_ptr->toStr().c_str(), type.c_str(), name.c_str());
        for (size_t i = 1; i < paramsSymbolEntry.size(); i++)
        {
            name = paramsSymbolEntry[i]->toStr();
            type = paramsSymbolEntry[i]->getType()->toStr();
            fprintf(yyout, ", %s %s", type.c_str(), name.c_str());
        }
        fprintf(yyout, ") {\n");
    }
    std::set<BasicBlock *> v;
    std::list<BasicBlock *> q;
    q.push_back(entry);
    v.insert(entry);
    while (!q.empty())
    {
        auto bb = q.front();
        q.pop_front();
        bb->output();
        for (auto succ = bb->succ_begin(); succ != bb->succ_end(); succ++)
        {
            if (v.find(*succ) == v.end())
            {
                v.insert(*succ);
                q.push_back(*succ);
            }
        }
    }
    fprintf(yyout, "}\n");
}

