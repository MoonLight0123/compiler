#ifndef __UNIT_H__
#define __UNIT_H__

#include <vector>
#include "Function.h"
#include "AsmBuilder.h"
class Unit
{
    typedef std::vector<Function *>::iterator iterator;
    typedef std::vector<Function *>::reverse_iterator reverse_iterator;

private:
    std::vector<Function *> func_list;
    std::vector<IdentifierSymbolEntry*> glbIds;
    std::vector<IdentifierSymbolEntry*> sysFuncs;
public:
    Unit() = default;
    ~Unit() ;
    void insertFunc(Function *);
    void removeFunc(Function *);
    void output() const;
    iterator begin() { return func_list.begin(); };
    iterator end() { return func_list.end(); };
    reverse_iterator rbegin() { return func_list.rbegin(); };
    reverse_iterator rend() { return func_list.rend(); };
    
    std::vector<IdentifierSymbolEntry*>& getGlbIds() { return glbIds; }
    std::vector<IdentifierSymbolEntry*>& getSysFuncs() { return sysFuncs; }

    void genMachineCode(MachineUnit* munit);
};

#endif
