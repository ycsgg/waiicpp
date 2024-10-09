#pragma once

#include "object.hpp"
#include "../ast/ast.cpp"
#include "env.cpp"
#include <format>
#include <functional>
#include <memory>
#include <string>

namespace object {

using std::format;
using std::shared_ptr;
using std::string;

class Integer : public Object {
    public:
    int Value;

    public:
    Integer(int val) : Value(val) {
    }
    Type ObjectType() {
        return Int_Obj;
    }
    string Inspect() {
        return format("{}", Value);
    }
};

class Boolean : public Object {
    public:
    bool Value;

    public:
    ~Boolean() {
    }
    Boolean(bool val) : Value(val) {
    }
    Type ObjectType() {
        return Bool_Obj;
    }
    string Inspect() {
        return format("{}", Value);
    }
};

static shared_ptr<Boolean> _TRUE = std::make_shared<Boolean>(true);
static shared_ptr<Boolean> _FALSE = std::make_shared<Boolean>(false);

class Double : public Object {
    public:
    double Value;

    public:
    Double(double val) : Value(val) {
    }
    Type ObjectType() {
        return Float_Obj;
    }
    string Inspect() {
        return format("{}", Value);
    }
};

class String : public Object {
    public:
    string Value;

    public:
    String(string val) : Value(val) {
    }
    Type ObjectType() {
        return Str_Obj;
    }
    string Inspect() {
        return "\"" + Value + "\"";
    }
};

using BuiltinFunction = std::function<obj_ptr(const std::vector<obj_ptr> &)>;

class BuiltIn : public Object {
    public:
    BuiltinFunction Fn;

    public:
    BuiltIn(BuiltinFunction fn) : Fn(fn) {
    }
    Type ObjectType() {
        return Builtin_Obj;
    }
    string Inspect() {
        return "builtin function";
    }
};

class Null : public Object {
    public:
    Type ObjectType() {
        return Null_Obj;
    }
    string Inspect() {
        return "null";
    }
};

class ReturnValue : public Object {
    public:
    shared_ptr<Object> Value;
    Type ObjectType() {
        return Return_Obj;
    }
    string Inspect() {
        return Value->Inspect();
    }
    ReturnValue(shared_ptr<Object> val) : Value(val) {
    }
};

class ErrorObject : public Object {
    public:
    string Message;
    Type ObjectType() {
        return Error_Obj;
    }
    string Inspect() {
        return "Error: " + Message;
    }
    ErrorObject(string msg) : Message(msg) {
    }
};

class FunctionObject : public Object {
    public:
    std::vector<shared_ptr<ast::Identifier>> Parameters;
    shared_ptr<ast::BlockStatement> Body;
    environment::env_ptr Env;

    Type ObjectType() {
        return Function_Obj;
    }

    string shortInspect() {
        std::string params;
        for (auto para : Parameters) {
            params += para->value + ",";
        }
        if (!params.empty()) {
            params.pop_back();
        }
        return format("fn({})", params);
    }

    string Inspect() {
        std::string params;
        for (auto para : Parameters) {
            params += para->value + ",";
        }
        if (!params.empty()) {
            params.pop_back();
        }
        return format("fn({}) {{{}}}", params, Body->output());
    }

    FunctionObject(std::vector<shared_ptr<ast::Identifier>> params,
                   shared_ptr<ast::BlockStatement> body,
                   environment::env_ptr env) {
        Parameters = params;
        Body = body;
        Env = env;
    }
};

class Array : public Object {
    public:
    std::vector<shared_ptr<Object>> Elements;

    Type ObjectType() {
        return Array_Obj;
    }

    string Inspect() {
        std::string res;
        for (auto &p : Elements) {
            res += p->Inspect() + ",";
        }
        if (!res.empty()) {
            res.pop_back();
        }
        return format("[{}]", res);
    }

    Array(std::vector<shared_ptr<Object>> elements) : Elements(elements) {
    }
};

template <typename... Args>
ErrorObject newError(const string &fmt, Args... args) {
    return ErrorObject(std::vformat(fmt, std::make_format_args(args...)));
}

static shared_ptr<Null> _NULL = std::make_shared<Null>();

template <typename T>
bool _isType(obj_ptr obj) {
    return (typeid(*obj.get()) == typeid(T));
}

Type type(obj_ptr obj) {
    return obj->ObjectType();
}

template <typename T>
auto getValue(shared_ptr<Object> obj) {
    return dynamic_cast<T *>(obj.get())->Value;
}

bool isCalcType(obj_ptr obj) {
    auto typ = type(obj);
    return typ == Int_Obj || typ == Float_Obj;
}

Type resultCalcType(Type Left, Type Right) {
    if (Left == Float_Obj || Right == Float_Obj) {
        return Float_Obj;
    }
    if (Left == Int_Obj || Right == Int_Obj) {
        return Int_Obj;
    }
    return Int_Obj;
}

} // namespace object