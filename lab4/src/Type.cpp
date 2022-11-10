#include "Type.h"
#include <sstream>

IntType TypeSystem::commonInt = IntType(4);
VoidType TypeSystem::commonVoid = VoidType();

Type* TypeSystem::intType = &commonInt;
Type* TypeSystem::voidType = &commonVoid;

std::string IntType::toStr()
{
    return "int";
}

std::string VoidType::toStr()
{
    return "void";
}

std::string FunctionType::toStr()
{
    std::ostringstream buffer;
    buffer << returnType->toStr() << "() ParamType: ";
    /*
    int j=paramsType.size();
    for(int i=0;i<j;i++)
        buffer<<paramsType[i]<<" ";
    */
   int j=paramsType.size();
    for(int i=0;i<j;i++){

        if(paramsType[i]->isInt()){
            buffer <<"int"<<" ";
        }

    }
    return buffer.str();
}

