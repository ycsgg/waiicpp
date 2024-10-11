#pragma once

#include <format>
#include <memory>
#include <string>

namespace object {

using std::format;
using std::shared_ptr;
using std::string;
enum Type {
    Null_Obj,
    Int_Obj,
    Float_Obj,
    Bool_Obj,
    Return_Obj,
    Error_Obj,
    Function_Obj,
    Str_Obj,
    Builtin_Obj,
    Array_Obj,
    Hash_Obj
};

string TypeToString(Type t) {
    switch (t) {
    case Null_Obj:
        return "null";
    case Int_Obj:
        return "int";
    case Float_Obj:
        return "float";
    case Bool_Obj:
        return "bool";
    case Str_Obj:
        return "str";
    case Return_Obj:
        return "return";
    case Error_Obj:
        return "error";
    case Function_Obj:
        return "function";
    case Builtin_Obj:
        return "builtin";
    case Array_Obj:
        return "array";
    case Hash_Obj:
        return "hash";
    default:
        return "unknown";
    }
}

class Object {
    public:
    virtual Type ObjectType() = 0;
    virtual string Inspect() = 0;
};

class Hasher {
    public:
    virtual size_t hash() = 0;
};

typedef shared_ptr<Object> obj_ptr;

} // namespace object