#include "Type.h"
#include <sstream>

IntType TypeSystem::commonInt = IntType(32);
IntType TypeSystem::commonBool = IntType(1);
VoidType TypeSystem::commonVoid = VoidType();
FloatType TypeSystem::commonFloat=FloatType(32);

Type* TypeSystem::floatType=&commonFloat;
Type* TypeSystem::intType = &commonInt;
Type* TypeSystem::voidType = &commonVoid;
Type* TypeSystem::boolType = &commonBool;


std::string IntType::toStr()
{
    std::ostringstream buffer;
    buffer << "i" << size;
    return buffer.str();
}

std::string VoidType::toStr()
{
    return "void";
}
std::string FloatType::toStr()
{
    return "float";
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

std::string PointerType::toStr()
{
    std::ostringstream buffer;
    buffer << valueType->toStr() << "*";
    return buffer.str();
}

