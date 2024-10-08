#pragma once

#include "../ast/ast.cpp"
#include "./env.cpp"
#include "./object.cpp"
#include <format>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace eval {
using std::format;
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::tuple;
using std::unique_ptr;
using std::vector;
using token::TokenType;

using namespace environment;
using namespace object;

typedef shared_ptr<Object> obj_ptr;

template <typename T>
struct _NodeTypeCast {
    bool vaild;
    T *res;
    operator bool() {
        return vaild;
    }
};

template <typename T>
_NodeTypeCast<T> _isType(ast::Node *node) {
    _NodeTypeCast<T> _res;
    _res.vaild = false;
    if (typeid(*node) == typeid(T)) {
        _res.vaild = true;
        _res.res = node->cast<T>();
    }
    return _res;
}

obj_ptr evalStatements(const vector<unique_ptr<ast::Statement>> &statements,
                       env_ptr env);
obj_ptr evalPrograms(const vector<unique_ptr<ast::Statement>> &statements,
                     env_ptr env);

obj_ptr evalPrefixExpression(token::TokenType, obj_ptr obj);

obj_ptr evalBangOperatorExpression(obj_ptr obj);
obj_ptr evalMinusPrefixExpression(obj_ptr obj);

obj_ptr evalInfixExpression(token::TokenType typ, obj_ptr left, obj_ptr right);

obj_ptr evalCalcExpression(token::TokenType typ, obj_ptr left, obj_ptr right);

obj_ptr evalLogicExpression(token::TokenType typ, obj_ptr left, obj_ptr right);

obj_ptr evalIfExpression(ast::IfExpression *ifexpr, env_ptr env);

obj_ptr evalIdentifer(ast::Identifier *ident, env_ptr env);

obj_ptr Eval(ast::Node *node, env_ptr env) {

#define isType(typ) auto _t = _isType<typ>(node)

    if (isType(ast::Program)) {
        return evalPrograms(_t.res->statements(), env);
    }

    if (isType(ast::ExpressionStatement)) {
        return Eval(_t.res->expression(), env);
    }

    if (isType(ast::IntegerLiteral)) {
        return make_shared<Integer>(_t.res->value);
    }

    if (isType(ast::DoubleLiteral)) {
        return make_shared<Double>(_t.res->value);
    }

    if (isType(ast::BooleanLiteral)) {
        return _t.res->value ? _TRUE : _FALSE;
    }

    if (isType(ast::PrefixExpression)) {
        auto right = Eval(_t.res->right(), env);
        return evalPrefixExpression(_t.res->TokenType(), right);
    }

    if (isType(ast::InfixExpression)) {
        auto left = Eval(_t.res->left(), env);
        auto right = Eval(_t.res->right(), env);
        return evalInfixExpression(_t.res->TokenType(), left, right);
    }
    if (isType(ast::IfExpression)) {
        return evalIfExpression(_t.res, env);
    }
    if (isType(ast::BlockStatement)) {
        return evalStatements(_t.res->statements(), env);
    }
    if (isType(ast::ReturnStatement)) {
        auto val = Eval(_t.res->returnValue(), env);
        return make_shared<ReturnValue>(val);
    }
    if (isType(ast::LetStatement)) {
        auto val = Eval(_t.res->value(), env);
        env->set(_t.res->name()->value, val);
        return nullptr;
    }
    if (isType(ast::Identifier)) {
        return evalIdentifer(_t.res, env);
    }

    return _NULL;

#undef isType
}

obj_ptr evalIdentifer(ast::Identifier *ident, env_ptr env) {
    auto [ok, val] = env->get(ident->value);
    if (ok) {
        return val;
    }
    throw newError("identifier not found: {}", ident->value);
}

obj_ptr evalPrefixExpression(token::TokenType typ, obj_ptr obj) {
    switch (typ) {
    case token::BANG:
    case token::NOT:
        return evalBangOperatorExpression(obj);
    case token::MINUS:
        return evalMinusPrefixExpression(obj);
    default:
        throw newError("unknown operator: {}{}", token::TypeToSymbol(typ),
                       obj->Inspect());
    }
    return _NULL;
}

obj_ptr evalPrograms(const vector<unique_ptr<ast::Statement>> &statements,
                     env_ptr env) {
    obj_ptr res;
    for (auto &stmt : statements) {
        res = Eval(stmt.get(), env);
        if (res != nullptr) {
            if (type(res) == Return) {
                return dynamic_cast<ReturnValue *>(res.get())->Value;
            }
            if (type(res) == Error) {
                return res;
            }
        }
    }

    return res;
}

obj_ptr evalStatements(const vector<unique_ptr<ast::Statement>> &statements,
                       env_ptr env) {
    obj_ptr res;
    for (auto &stmt : statements) {
        res = Eval(stmt.get(), env);
        if (res != nullptr) {
            if (type(res) == Return) {
                return res;
            }
            if (type(res) == Error) {
                return res;
            }
        }
    }

    return res;
}

obj_ptr evalBangOperatorExpression(obj_ptr obj) {
    if (obj == _TRUE) {
        return _FALSE;
    }
    if (obj == _FALSE) {
        return _TRUE;
    }
    if (obj == _NULL) {
        return _TRUE;
    }
    if (_isType<Integer>(obj) && getValue<Integer>(obj) == 0) {
        return _TRUE;
    }
    return _FALSE;
}

obj_ptr evalMinusPrefixExpression(obj_ptr obj) {
    if (_isType<Integer>(obj)) {
        return make_shared<Integer>(-getValue<Integer>(obj));
    }
    if (_isType<Double>(obj)) {
        return make_shared<Double>(-getValue<Double>(obj));
    }
    throw newError("unknown operator: -{}", obj->Inspect());
}

obj_ptr evalInfixExpression(token::TokenType typ, obj_ptr left, obj_ptr right) {
    switch (typ) {
    case token::PLUS:
    case token::MINUS:
    case token::ASTERISK:
    case token::SLASH:
        return evalCalcExpression(typ, left, right);
    case token::AND:
    case token::EQ:
    case token::NOT_EQ:
    case token::LE:
    case token::GE:
    case token::GT:
    case token::LT:
    case token::OR:
        return evalLogicExpression(typ, left, right);
    default:
        throw newError("unknown operator: {} {} {}", left->Inspect(),
                       token::TypeToSymbol(typ), right->Inspect());
    }
}

template <typename T, typename E>
auto _calcFunction(token::TokenType typ, T A, E B) -> decltype(A + B) {
    switch (typ) {
    case token::PLUS:
        return A + B;
    case token::MINUS:
        return A - B;
    case token::ASTERISK:
        return A * B;
    case token::SLASH:
        return A / B;
    default:
        return 0;
    }
}

template <typename T, typename E>
bool _logicFunction(token::TokenType typ, T A, E B) {
    switch (typ) {
    case token::EQ:
        return (A == B);
    case token::NOT_EQ:
        return (A != B);
    case token::LE:
        return (A <= B);
    case token::GE:
        return (A >= B);
    case token::LT:
        return (A < B);
    case token::GT:
        return (A > B);
    case token::OR:
        return (A || B);
    case token::AND:
        return (A && B);
    default:
        return false;
    }
}

obj_ptr evalCalcExpression(token::TokenType typ, obj_ptr left, obj_ptr right) {
    if (isCalcType(left) && isCalcType(right)) {
        auto typeLeft = type(left);
        auto typeRight = type(right);
        if (typeLeft == Float) {
            auto valLeft = getValue<Double>(left);
            if (typeRight == Float) {
                auto valRight = getValue<Double>(right);
                return make_shared<Double>(
                    _calcFunction(typ, valLeft, valRight));
            }
            if (typeRight == Int) {
                auto valRight = getValue<Integer>(right);
                return make_shared<Double>(
                    _calcFunction(typ, valLeft, valRight));
            }
        }
        if (typeLeft == Int) {
            auto valLeft = getValue<Integer>(left);
            if (typeRight == Float) {
                auto valRight = getValue<Double>(right);
                return make_shared<Double>(
                    _calcFunction(typ, valLeft, valRight));
            }
            if (typeRight == Int) {
                auto valRight = getValue<Integer>(right);
                return make_shared<Integer>(
                    _calcFunction(typ, valLeft, valRight));
            }
        }
    }
    throw newError("type mismatch: {} {} {}", left->Inspect(),
                   token::TypeToSymbol(typ), right->Inspect());
}

obj_ptr evalLogicExpression(token::TokenType typ, obj_ptr left, obj_ptr right) {
    auto typLeft = type(left);
    auto typRight = type(right);
    auto func = [&](auto valLeft) -> obj_ptr {
        if (typRight == Float) {
            auto valRight = getValue<Double>(right);
            return make_shared<Boolean>(_logicFunction(typ, valLeft, valRight));
        }
        if (typRight == Int) {
            auto valRight = getValue<Integer>(right);
            return make_shared<Boolean>(_logicFunction(typ, valLeft, valRight));
        }
        if (typRight == Bool) {
            auto valRight = getValue<Boolean>(right);
            return make_shared<Boolean>(_logicFunction(typ, valLeft, valRight));
        }
        throw newError("type mismatch: {} {} {}", left->Inspect(),
                       token::TypeToSymbol(typ), right->Inspect());
    };
    if (typLeft == Float) {
        auto valLeft = getValue<Double>(left);
        return func(valLeft);
    }
    if (typLeft == Int) {
        auto valLeft = getValue<Integer>(left);
        return func(valLeft);
    }
    if (typLeft == Bool) {
        auto valLeft = getValue<Boolean>(left);
        return func(valLeft);
    }
    throw newError("type mismatch: {} {} {}", left->Inspect(),
                   token::TypeToSymbol(typ), right->Inspect());
}

bool isTrue(obj_ptr obj) {
    switch (type(obj)) {
    case _Null:
        return false;
    case Bool:
        return getValue<Boolean>(obj);
    case Int:
        return getValue<Integer>(obj);
    case Float:
        return getValue<Double>(obj);
    default:
        return false;
    }
}

obj_ptr evalIfExpression(ast::IfExpression *ifexpr, env_ptr env) {
    auto condition_res = Eval(ifexpr->condition(), env);
    if (isTrue(condition_res)) {
        return Eval(ifexpr->consequence(), env);
    } else if (ifexpr->Alternative != nullptr) {
        return Eval(ifexpr->alternative(), env);
    } else {
        return _NULL;
    }
}

} // namespace eval
