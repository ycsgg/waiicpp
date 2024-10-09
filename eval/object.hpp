#pragma once

#include <format>
#include <memory>
#include <string>

namespace object {

using std::format;
using std::shared_ptr;
using std::string;
enum Type { _Null, Int, Float, Bool, Return, Error, Function, Str, Builtin };

string TypeToString(Type t) {
    switch (t) {
    case _Null:
        return "null";
    case Int:
        return "int";
    case Float:
        return "float";
    case Bool:
        return "bool";
    case Str:
        return "str";
    case Return:
        return "return";
    case Error:
        return "error";
    case Function:
        return "function";
    case Builtin:
        return "builtin";
    default:
        return "unknown";
    }
}

class Object {
    public:
    virtual Type ObjectType() = 0;
    virtual string Inspect() = 0;
};

typedef shared_ptr<Object> obj_ptr;
} // namespace object