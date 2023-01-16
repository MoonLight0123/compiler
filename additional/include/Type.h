#ifndef __TYPE_H__
#define __TYPE_H__
#include <vector>
#include <string>

class SymbolEntry;


class Type
{
private:
    int kind;
protected:
    enum {INT, VOID, FUNC, PTR,FLOAT,ARRAY};
public:
    Type(int kind) : kind(kind) {};
    virtual ~Type() {};
    virtual std::string toStr() = 0;
    bool isInt() const {return kind == INT;};
    bool isVoid() const {return kind == VOID;};
    bool isFunc() const {return kind == FUNC;};
    bool isFloat() const {return kind==FLOAT;};
    bool isArray() const {return kind==ARRAY;};
    bool isPointer() const {return kind==PTR;};
    virtual int getSize() {return -1;};
};

class IntType : public Type
{
private:
    int size;
public:
    IntType(int size) : Type(Type::INT), size(size){};
    void setSize(int size){this->size=size;}
    int getSize(){return size;}
    std::string toStr();
    bool isBool(){return size==1;};
    bool isInt(){return size==32;};
};

class VoidType : public Type
{
public:
    VoidType() : Type(Type::VOID){};
    std::string toStr();
};

class FunctionType : public Type
{
public:
    Type *returnType;
    std::vector<Type*> paramsType;
    std::vector<SymbolEntry*> paramsSymbolEntry;
    
    FunctionType(Type* returnType, std::vector<Type*> paramsType) : 
    Type(Type::FUNC), returnType(returnType), paramsType(paramsType){};
    std::vector<Type*> getParamsType() { return paramsType; };
    Type* getRetType() {return returnType;};
    std::string toStr();
};


class ArrayType : public Type
{
public:
    Type *arrayElementType;
    std::vector<int> dimsVal;
    ArrayType(Type *arrayElementType):Type(Type::ARRAY),arrayElementType(arrayElementType){};
    std::string toStr();
    std::vector<int> constArrayInitVal;
    int paramOffset;
};

class FloatType:public Type
{
private:
    int size;
public:
    FloatType(int size) : Type(Type::FLOAT), size(size){};
    std::string toStr();
    int getSize(){return size;};
};

class PointerType : public Type
{
public:
    Type *valueType;
    PointerType(Type* valueType) : Type(Type::PTR) {this->valueType = valueType;};
    std::string toStr();
};

class TypeSystem
{
private:
    static IntType commonInt;
    static IntType commonBool;
    static VoidType commonVoid;
    static FloatType commonFloat;
public:
    static Type *intType;
    static Type *voidType;
    static Type *boolType;
    static Type *floatType;
};

#endif



