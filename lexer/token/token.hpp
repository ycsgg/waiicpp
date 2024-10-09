#pragma once

#include <iostream>
#include <map>
#include <string>

namespace token {

enum TokenType {
    ILLEGAL,
    END,

    IDENT,
    INT,
    DOUBLE,

    // Operators
    ASSIGN,
    PLUS,
    MINUS,
    BANG,
    ASTERISK,
    SLASH,

    LT,
    GT,

    LE,
    GE,

    EQ,
    NOT_EQ,

    // Delimiters
    COMMA,
    SEMICOLON,

    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,

    // Keywords
    FUNCTION,
    LET,
    TRUE,
    FALSE,
    IF,
    ELSE,
    RETURN,
    OR,
    AND,
    NOT,
    FOR,
    IN,
    WHILE
};

std::string TypeToSymbol(TokenType type) {
    switch (type) {
    case ILLEGAL:
        return "ILLEGAL";
    case END:
        return "END";
    case IDENT:
        return "IDENT";
    case ASSIGN:
        return "=";
    case PLUS:
        return "+";
    case MINUS:
        return "-";
    case BANG:
        return "!";
    case ASTERISK:
        return "*";
    case SLASH:
        return "/";
    case EQ:
        return "==";
    case NOT_EQ:
        return "!=";
    case COMMA:
        return ",";
    case SEMICOLON:
        return ";";
    case LPAREN:
        return "(";
    case RPAREN:
        return ")";
    case LBRACE:
        return "{";
    case RBRACE:
        return "}";
    case FUNCTION:
        return "Function";
    case LET:
        return "Let";
    case TRUE:
        return "true";
    case FALSE:
        return "false";
    case IF:
        return "If";
    case ELSE:
        return "Else";
    case RETURN:
        return "Return";
    case INT:
        return "Int";
    case DOUBLE:
        return "Double";
    case LE:
        return "<=";
    case GE:
        return ">=";
    case LT:
        return "<";
    case GT:
        return ">";
    case OR:
        return "or";
    case AND:
        return "and";
    case NOT:
        return "not";
    case FOR:
        return "For";
    case IN:
        return "In";
    case WHILE:
        return "While";
    default:
        return "None";
    }
}

std::string TypeToName(TokenType type) {
    switch (type) {
    case ILLEGAL:
        return "ILLEGAL";
    case END:
        return "END";
    case IDENT:
        return "IDENT";
    case ASSIGN:
        return "ASSIGN";
    case PLUS:
        return "PLUS";
    case MINUS:
        return "MINUS";
    case BANG:
        return "BANG";
    case ASTERISK:
        return "ASTERISK";
    case SLASH:
        return "SLASH";
    case EQ:
        return "EQ";
    case NOT_EQ:
        return "NOT_EQ";
    case COMMA:
        return "COMMA";
    case SEMICOLON:
        return "SEMICOLON";
    case LPAREN:
        return "LPAREN";
    case RPAREN:
        return "RPAREN";
    case LBRACE:
        return "LBRACE";
    case RBRACE:
        return "RBRACE";
    case FUNCTION:
        return "FUNCTION";
    case LET:
        return "LET";
    case TRUE:
        return "TRUE";
    case FALSE:
        return "FALSE";
    case IF:
        return "IF";
    case ELSE:
        return "ELSE";
    case RETURN:
        return "RETURN";
    case INT:
        return "INT";
    case DOUBLE:
        return "DOUBLE";
    case LE:
        return "LE";
    case GE:
        return "GE";
    case LT:
        return "LT";
    case GT:
        return "GT";
    case OR:
        return "OR";
    case AND:
        return "AND";
    case NOT:
        return "NOT";
    case FOR:
        return "FOR";
    case WHILE:
        return "WHILE";
    case IN:
        return "IN";
    default:
        return "None";
    }
}

struct Token {
    TokenType Type;
    std::string Literal;
    void Output(std::ostream &out = std::cout) {
        out << TypeToName(Type) << " : " << Literal << std::endl;
    }
};

static std::map<std::string, TokenType> keywords = {
    {"fn", FUNCTION}, {"let", LET},   {"true", TRUE},     {"false", FALSE},
    {"if", IF},       {"else", ELSE}, {"return", RETURN}, {"or", OR},
    {"and", AND},     {"not", NOT},   {"for", FOR},       {"in", IN},
    {"while", WHILE}};

Token newToken(TokenType Type, std::string Literal) {
    Token res;
    res.Type = Type;
    res.Literal = Literal;
    return res;
}

TokenType LookupIdent(std::string str) {
    if (keywords.count(str)) {
        return keywords[str];
    }
    return IDENT;
}

} // namespace token