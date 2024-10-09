#pragma once

#include "../ast/ast.cpp"
#include "../lexer/lexer.cpp"
#include <functional>
#include <map>
#include <memory>

namespace parser {

using std::function;
using std::map;
using namespace ast;
using lexer ::Lexer;
using std::shared_ptr;
using std::unique_ptr;
using std::unordered_map;
using token::Token;

enum Priority {
    _,
    LOWEST,      // begin
    ASSIGN,      // =
    LOGIC,       // and or
    EQUALS,      // ==
    LESSGREATER, // < <= > >=
    SUM,         //+ -
    PRODUCT,     //* /
    PREFIX,      //! -
    CALL,        // invoke
    INDEX        // []
};

static unordered_map<token::TokenType, Priority> precedences = {
    {token::EQ, EQUALS},      {token::NOT_EQ, EQUALS},
    {token::LT, LESSGREATER}, {token::GT, LESSGREATER},
    {token::PLUS, SUM},       {token::MINUS, SUM},
    {token::SLASH, PRODUCT},  {token::ASTERISK, PRODUCT},
    {token::GE, LESSGREATER}, {token::LE, LESSGREATER},
    {token::OR, LOGIC},       {token::AND, LOGIC},
    {token::LPAREN, CALL},    {token::ASSIGN, ASSIGN},
    {token::IDENT, PRODUCT},  {token::TRUE, PRODUCT},
    {token::FALSE, PRODUCT},  {token::LBRACKET, INDEX}};

class Parser {
    typedef unique_ptr<Expression> (Parser::*prefixParseFunc)();
    typedef unique_ptr<Expression> (Parser::*infixParseFunc)(
        unique_ptr<Expression>);

    Lexer *L;
    Token curToken;
    Token peekToken;

    public:
    vector<string> errors;
    unordered_map<token::TokenType, prefixParseFunc> prefixParseFuncs;
    unordered_map<token::TokenType, infixParseFunc> infixParseFuncs;

    private:
    void registerPrefixFunc(token::TokenType typ, prefixParseFunc Func) {
        prefixParseFuncs[typ] = Func;
    }
    void registerInfixFunc(token::TokenType typ, infixParseFunc Func) {
        infixParseFuncs[typ] = Func;
    }

    void nextToken() {
        curToken = peekToken;
        peekToken = L->NextToken();
    }
    void registerAll();

    public:
    Parser(Lexer L) : L(&L) {
        nextToken();
        nextToken();
        registerAll();
    }
    Parser(Lexer *L) : L(L) {
        nextToken();
        nextToken();
        registerAll();
    }

    unique_ptr<Program> ParserProgram();
    unique_ptr<Statement> parseStatement();
    unique_ptr<LetStatement> parseLetStatement();
    unique_ptr<BlockStatement> parseBlockStatement();
    unique_ptr<ReturnStatement> parseReturnStatement();
    unique_ptr<ExpressionStatement> parseExpressionStatement();
    unique_ptr<FunctionStatement> parseFunctionStatement();
    unique_ptr<ForStatement> parseForStatement();
    unique_ptr<WhileStatement> parseWhileStatement();
    unique_ptr<Expression> parseExpression(Priority pri);
    unique_ptr<Expression> parseIdentifier();
    unique_ptr<Expression> parseIntegerLiteral();
    unique_ptr<Expression> parseBooleanLiteral();
    unique_ptr<Expression> parseDoubleLiteral();
    unique_ptr<Expression> parseStringLiteral();
    unique_ptr<Expression> parsePrefixExpression();
    unique_ptr<Expression> parseArrayLiteral();
    vector<unique_ptr<Expression>> parseExpressionList(token::TokenType end);
    unique_ptr<Expression> parseInfixExpression(unique_ptr<Expression> left);
    unique_ptr<Expression> parseIndexExpression(unique_ptr<Expression> left);
    unique_ptr<Expression>
    parseIdentInfixExpression(unique_ptr<Expression> left);
    unique_ptr<Expression> parseGroupedExpression();
    unique_ptr<Expression> parseIfExpression();
    unique_ptr<Expression> parseFunctionLiteral();
    unique_ptr<Expression> parseCallExpression(unique_ptr<Expression> func);
    vector<shared_ptr<Identifier>> parseParameters();
    // vector<unique_ptr<Expression>> parseCallArguments();

    private:
    bool curTokenIs(token::TokenType typ) {
        return curToken.Type == typ;
    }
    bool peekTokenIs(token::TokenType typ) {
        return peekToken.Type == typ;
    }
    bool expectToken(token::TokenType typ) {
        if (peekTokenIs(typ)) {
            nextToken();
            return true;
        } else {
            peekError(typ);
            return false;
        }
    }
    Priority peekPrecedence() {
        if (precedences.count(peekToken.Type)) {
            return precedences[peekToken.Type];
        }
        return LOWEST;
    }
    Priority curPrecedence() {
        if (precedences.count(curToken.Type)) {
            return precedences[curToken.Type];
        }
        return LOWEST;
    }
    void peekError(token::TokenType typ) {
        // std::cerr << "???";
        errors.push_back(format("expected next token to be {}, got {} instead.",
                                token::TypeToName(typ),
                                token::TypeToName(peekToken.Type)));
    }
    void noPrefixParseFnError(token::TokenType typ) {
        errors.push_back(format("no prefix parse function for {} found.",
                                token::TypeToName(typ)));
    }
};

static shared_ptr<BooleanLiteral> _TRUE =
    std::make_shared<BooleanLiteral>(Token{token::TRUE, "true"}, true);
static shared_ptr<BooleanLiteral> _FALSE =
    std::make_shared<BooleanLiteral>(Token{token::FALSE, "false"}, false);

} // namespace parser
