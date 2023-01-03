#include "SymbolTable.h"
#include <iostream>
#include <sstream>

extern FILE* yyout;
SymbolEntry::SymbolEntry(Type *type, int kind) 
{
    this->type = type;
    this->kind = kind;
}

ConstantSymbolEntry::ConstantSymbolEntry(Type *type, int value) : SymbolEntry(type, SymbolEntry::CONSTANT)
{
    this->value = value;
}

std::string ConstantSymbolEntry::toStr()
{
    std::ostringstream buffer;
    buffer << value;
    return buffer.str();
}

IdentifierSymbolEntry::IdentifierSymbolEntry(Type *type, std::string name, int scope) : SymbolEntry(type, SymbolEntry::VARIABLE), name(name)
{
    this->scope = scope;
    addr = nullptr;
}

std::string IdentifierSymbolEntry::toStr()
{
    if(this->isGlobal())
        return "@" + name;
    else return "%"+name;
}

TemporarySymbolEntry::TemporarySymbolEntry(Type *type, int label) : SymbolEntry(type, SymbolEntry::TEMPORARY)
{
    this->label = label;
}

std::string TemporarySymbolEntry::toStr()
{
    std::ostringstream buffer;
    buffer << "%t" << label;
    return buffer.str();
}

SymbolTable::SymbolTable()
{
    prev = nullptr;
    level = 0;
}

SymbolTable::SymbolTable(SymbolTable *prev)
{
    this->prev = prev;
    this->level = prev->level + 1;
}

 void SymbolTable::installFunc(std::string name, SymbolEntry* entry){
     symbolTable.insert({name, entry});
 }


// void IdentifierSymbolEntry::outputGlbId()
// {
   
//     assert(isGlobal());

//     if(type->isInt()) {
//         if(glbConst){
//             fprintf(yyout, "@%s = global %s %d, align 4 \n", this->name.c_str(), this->type->toStr().c_str(), glbConst->getValue());
//         }else{
//             fprintf(yyout, "@%s = global %s 0, align 4 \n", this->name.c_str(), this->type->toStr().c_str());
//         }
//     }
//     /*
//     else if(type->isFloat()) {
//         if(glbConst){
//             fprintf(yyout, "@%s = global %s %.6e\n",this->name.c_str(), this->type->toStr().c_str(), glbConst->getValueFloat());
//         }else{
//             fprintf(yyout, "@%s = global %s 0.000000e+00 \n",this->name.c_str(), this->type->toStr().c_str());
//         }
//     }
//     */
// }

void IdentifierSymbolEntry::outputSysFunc(){

    assert(this->getType()->isFunc());
    
    
    fprintf(yyout, "declare %s @%s(", 
        dynamic_cast<FunctionType*>(type)->getRetType()->toStr().c_str(), (const char*)name.c_str());
    bool first = true;
    for(auto type : dynamic_cast<FunctionType*>(type)->getParamsType()){
        if(!type->isVoid()){
            if(!first){
                first = false;
                fprintf(yyout, ", ");
            }
            fprintf(yyout,"%s", type->toStr().c_str());
        }
    }
    fprintf(yyout, ")\n");

}


/*
    Description: lookup the symbol entry of an identifier in the symbol table
    Parameters: 
        name: identifier name
    Return: pointer to the symbol entry of the identifier

    hint:
    1. The symbol table is a stack. The top of the stack contains symbol entries in the current scope.
    2. Search the entry in the current symbol table at first.
    3. If it's not in the current table, search it in previous ones(along the 'prev' link).
    4. If you find the entry, return it.
    5. If you can't find it in all symbol tables, return nullptr.
*/
SymbolEntry* SymbolTable::lookup(std::string name)
{
    SymbolTable* temp=this;
    while(temp->level>=0)
    {
        if(temp->symbolTable.count(name)==1)
            return temp->symbolTable[name];
        else
        {
            if(temp->level!=0)
                temp=temp->prev;
            else break;
        }
    }
    // Todo
    return nullptr;
    
}

bool SymbolTable::matchFP(SymbolEntry* se, std::vector<Type*> typeList){




}


// install the entry into current symbol table.
void SymbolTable::install(std::string name, SymbolEntry* entry)
{
    symbolTable[name] = entry;
}

int SymbolTable::counter = 0;
static SymbolTable t;
SymbolTable *identifiers = &t;
SymbolTable *globals = &t;
