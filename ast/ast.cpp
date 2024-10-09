#pragma once

// #define DEBUG

#include "../lexer/token/token.hpp"
#include <format>
#include <memory>
#include <string>
#include <vector>

namespace ast {

using std::format;
using std::shared_ptr;
using std::string;
using std::unique_ptr;
using std::vector;
using token::Token;
class Node {
    public:
    virtual string TokenLiteral() = 0;
    virtual string output() = 0;
    template <typename T>
    T *cast() {
        return dynamic_cast<T *>(this);
    }
    template <typename T>
    const T *cast() const {
        return dynamic_cast<T *>(this);
    }
};

string SafeOutput(Node *node) {
    if (node == nullptr) {
        return "NULL";
    } else {
        return node->output();
    }
}

class Statement : public Node {
    public:
    virtual string statementNode() {
        return "";
    };
};
class Expression : public Node {
    public:
    virtual string expressionNode() {
        return "";
    };
};
class Program : public Node {
    public:
    vector<unique_ptr<Statement>> Statements;

    public:
    const vector<unique_ptr<Statement>> &statements() const {
        return Statements;
    }
    string TokenLiteral() {
        if (Statements.size()) {
            return Statements[0]->TokenLiteral();
        } else {
            return "";
        }
    }
    string output() {
        string res;
        for (auto &x : Statements) {
            res += x->output() + '\n';
        }
        return res;
    }
};
class Identifier : public Expression {
    public:
    Token token;
    string value;

    public:
    string expressionNode() {
        return "";
    }
    string TokenLiteral() {
        return token.Literal;
    }

    public:
    Identifier(Token token, string val) : token(token), value(val) {
    }
    string output() {
#ifdef DEBUG
        return format("Identifier: {}", TokenLiteral());
#else
        return format("{}", TokenLiteral());
#endif
    }
};
class LetStatement : public Statement {
    public:
    Token token;
    unique_ptr<Identifier> Name;
    unique_ptr<Expression> Value;

    public:
    Expression *value() {
        return Value.get();
    }
    Identifier *name() {
        return Name.get();
    }
    string statementNode() {
        return "";
    }
    string TokenLiteral() {
        return token.Literal;
    }
    string output() {
#ifdef DEUBG
        return format("LetStatement:\n{}\n=\n    {}", SafeOutput(Name),
                      SafeOutput(Value));
#else
        return format("let {} = {};", SafeOutput(Name.get()),
                      SafeOutput(Value.get()));
#endif
    }
};
class ReturnStatement : public Statement {
    public:
    Token token;
    unique_ptr<Expression> ReturnValue;

    public:
    Expression *returnValue() {
        return ReturnValue.get();
    }
    string TokenLiteral() {
        return token.Literal;
    }
    string output() {
#ifdef DEUBG
        return format("Return: \n    {}", SafeOutput(ReturnValue));
#else
        return format("return {};", SafeOutput(ReturnValue.get()));
#endif
    }
};

class ExpressionStatement : public Statement {
    public:
    Token token;
    unique_ptr<Expression> _expression;

    public:
    Expression *expression() {
        return _expression.get();
    }
    string TokenLiteral() {
        return token.Literal;
    }
    string output() {
#ifdef DEBUG
        return format("Expression: \n    {}", SafeOutput(_expression.get()));
#else
        return format("{}", SafeOutput(_expression.get()));
#endif
    }
};

class BlockStatement : public Statement {
    public:
    Token token;
    vector<unique_ptr<Statement>> Statements;

    public:
    vector<unique_ptr<Statement>> &statements() {
        return Statements;
    }
    string TokenLiteral() {
        return token.Literal;
    }
    string output() {
        string res;
        for (auto &x : Statements) {
            res += x->output() + '\n';
        }
        res = res.substr(0, res.length() - 1);
        return res;
    }
};

class FunctionStatement : public Statement {
    public:
    Token token;
    unique_ptr<Identifier> Name;
    vector<shared_ptr<Identifier>> Parameters;
    shared_ptr<BlockStatement> Body;

    public:
    Identifier *name() {
        return Name.get();
    }
    vector<shared_ptr<Identifier>> &parameters() {
        return Parameters;
    }
    shared_ptr<BlockStatement> body() {
        return Body;
    }
    string TokenLiteral() {
        return token.Literal;
    }
    string output() {
        string res = "fn " + Name->TokenLiteral() + "(";
        for (auto &p : Parameters) {
            res += p->TokenLiteral() + ",";
        }
        res = res.substr(0, res.length() - 1);
        res += ")";
#ifdef DEBUG
        return format("FunctionStatement : \n{}:\n    {{{}    }}", res,
                      SafeOutput(Body.get()));
#else
        return format("{}{{{}}}", res, SafeOutput(Body.get()));
#endif
    }
};

class ForStatement : public Statement {
    public:
    Token token;
    unique_ptr<BlockStatement> Body;
    shared_ptr<Identifier> Name;
    shared_ptr<Expression> Range;

    public:
    BlockStatement *body() {
        return Body.get();
    }
    Identifier *name() {
        return Name.get();
    }
    Expression *range() {
        return Range.get();
    }
    string TokenLiteral() {
        return token.Literal;
    }
    string output() {
        return format("for({} in {}){{{}}}", SafeOutput(Name.get()),
                      SafeOutput(Range.get()), SafeOutput(Body.get()));
    }
};

class IntegerLiteral : public Expression {
    public:
    Token token;
    int value;

    public:
    string TokenLiteral() {
        return token.Literal;
    }
    string output() {
#ifdef DEBUG
        return format("Integer:{}", value);
#else
        return format("{}", value);
#endif
    }
};

class DoubleLiteral : public Expression {
    public:
    Token token;
    double value;

    public:
    string TokenLiteral() {
        return token.Literal;
    }
    string output() {
#ifdef DEBUG
        return format("Double:{}", value);
#else
        return format("{}", value);
#endif
    }
};

class BooleanLiteral : public Expression {
    public:
    Token token;
    bool value;

    public:
    BooleanLiteral() {};
    BooleanLiteral(Token token, bool val) : token(token), value(val) {
    }
    string TokenLiteral() {
        return token.Literal;
    }
    string output() {
#ifdef DEBUG
        return format("Boolean:{}", value);
#else
        return format("{}", value);
#endif
    }
};

class StringLiteral : public Expression {
    public:
    Token token;
    string value;

    public:
    string TokenLiteral() {
        return token.Literal;
    }
    string output() {
#ifdef DEBUG
        return format("String:\"{}\"", value);
#else
        return format("\"{}\"", value);
#endif
    }
};

class PrefixExpression : public Expression {
    public:
    Token token;
    string oper;
    unique_ptr<Expression> Right;

    public:
    Expression *right() {
        return Right.get();
    }
    string TokenLiteral() {
        return token.Literal;
    }
    token::TokenType TokenType() {
        return token.Type;
    }
    string output() {
        return format("({} {})", oper, SafeOutput(Right.get()));
    }
};

class InfixExpression : public Expression {
    public:
    Token token;
    unique_ptr<Expression> Left, Right;
    string oper;

    public:
    Expression *left() {
        return Left.get();
    }
    Expression *right() {
        return Right.get();
    }
    string TokenLiteral() {
        return token.Literal;
    }
    token::TokenType TokenType() {
        return token.Type;
    }
    string output() {
        return format("({} {} {})", SafeOutput(Left.get()), oper,
                      SafeOutput(Right.get()));
    }
};

class IfExpression : public Expression {
    public:
    Token token;
    shared_ptr<Expression> Condition;
    unique_ptr<BlockStatement> Consequence;
    unique_ptr<IfExpression> Alternative;

    public:
    Expression *condition() {
        return Condition.get();
    }
    BlockStatement *consequence() {
        return Consequence.get();
    }
    IfExpression *alternative() {
        return Alternative.get();
    }
    string TokenLiteral() {
        return token.Literal;
    }
    string output() {
#ifdef DEBUG
        return format(
            "IF:{}\n    TRUE->{}\n    FALSE->{}", SafeOutput(Condition.get()),
            SafeOutput(Consequence.get()), SafeOutput(Alternative.get()));
#else
        return format("if({}){{{}}}else{{{}}}", SafeOutput(Condition.get()),
                      SafeOutput(Consequence.get()),
                      SafeOutput(Alternative.get()));
#endif
    }
};

class FunctionLiteral : public Expression {
    public:
    Token token;
    vector<shared_ptr<Identifier>> Parameters;
    shared_ptr<BlockStatement> Body;

    public:
    vector<shared_ptr<Identifier>> &parameters() {
        return Parameters;
    }
    shared_ptr<BlockStatement> body() {
        return Body;
    }
    string TokenLiteral() {
        return token.Literal;
    }
    string output() {
        string res = "fn(";
        for (auto p : Parameters) {
            res += p->TokenLiteral() + ",";
        }
        res = res.substr(0, res.length() - 1);
        res += ")";
#ifdef DEBUG
        return format("{}:\n    {{{}    }}", res, SafeOutput(Body.get()));
#else
        return format("{}:{{{}}}", res, SafeOutput(Body.get()));
#endif
    }
};

class ArrayLiteral : public Expression {
    public:
    Token token;
    vector<unique_ptr<Expression>> Elements;

    public:
    vector<unique_ptr<Expression>> &elements() {
        return Elements;
    }
    string TokenLiteral() {
        return token.Literal;
    }
    string output() {
        string res;
        for (auto &p : Elements) {
            res += SafeOutput(p.get()) + ",";
        }
        res = res.substr(0, res.length() - 1);
#ifdef DEBUG
        return format("Array : [{}]", res);
#else
        return format("[{}]", res);
#endif
    }
};

class IndexExpression : public Expression {
    public:
    Token token;
    unique_ptr<Expression> Left, Index;

    public:
    Expression *left() {
        return Left.get();
    }
    Expression *index() {
        return Index.get();
    }
    string TokenLiteral() {
        return token.Literal;
    }
    string output() {
        return format("({}[{}])", SafeOutput(Left.get()),
                      SafeOutput(Index.get()));
    }
};

class CallExpression : public Expression {
    public:
    Token token;
    unique_ptr<Expression> Function;
    vector<unique_ptr<Expression>> Arguments;

    public:
    vector<unique_ptr<Expression>> &arguments() {
        return Arguments;
    }
    Expression *function() {
        return Function.get();
    }
    string TokenLiteral() {
        return token.Literal;
    }
    string output() {
        string res;
        for (auto &p : Arguments) {
            res += SafeOutput(p.get()) + ",";
        }
        res = res.substr(0, res.length() - 1);
#ifdef DEBUG
        return format("Call : {}({})", SafeOutput(Function.get()), res);
#else
        return format("{}({})", SafeOutput(Function.get()), res);
#endif
    }
};

class WhileStatement : public Statement {
    public:
    Token token;
    shared_ptr<Expression> Condition;
    unique_ptr<BlockStatement> Body;

    public:
    Expression *condition() {
        return Condition.get();
    }
    BlockStatement *body() {
        return Body.get();
    }
    string TokenLiteral() {
        return token.Literal;
    }
    string output() {
        return format("while({}){{{}}}", SafeOutput(Condition.get()),
                      SafeOutput(Body.get()));
    }
};
} // namespace ast
