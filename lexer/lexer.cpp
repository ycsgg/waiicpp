#pragma once

#include "./token/token.hpp"

using std::string;

namespace lexer {

using token::Token;
using token::TokenType;

class Lexer {
    private:
    string input;
    int position;
    int readPostition;
    char ch;
    void readChar() {
        if (readPostition >= input.length()) {
            ch = 0;
        } else {
            ch = input[readPostition];
        }
        position = readPostition;
        readPostition++;
    }
    void skipWhitespace() {
        while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {
            readChar();
        }
    }
    bool isLetter() {
        return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') ||
               (ch == '_');
    }
    bool isDigit() {
        return ('0' <= ch && ch <= '9');
    }

    public:
    Lexer() {
    }
    Lexer(string input) : input(input), position(0), readPostition(0), ch(0) {
        readChar();
    }
    char peekChar() {
        if (readPostition >= input.length()) {
            return 0;
        } else {
            return input[readPostition];
        }
    }
    std::pair<string, TokenType> readNumber() {
        auto start = position;
        bool double_flag = false;
        while (isDigit() || (ch == '.' && (!double_flag))) {
            double_flag |= (ch == '.');
            readChar();
        }
        return std::make_pair(input.substr(start, position - start),
                              double_flag ? TokenType::DOUBLE : TokenType::INT);
    }
    string readIdentifier() {
        auto start = position;
        while (isLetter() || isDigit()) {
            readChar();
        }
        // std::cerr << start << " " << position << std::endl;
        return input.substr(start, position - start);
    }
    string readString() {
        auto start = position + 1;
        readChar();
        while (ch != '\"' && ch != 0) {
            readChar();
        }
        return input.substr(start, position - start);
    }
    Token NextToken() {
        Token res;
        skipWhitespace();
        auto setToken = [&res](TokenType type, string literal) -> void {
            res.Type = type;
            res.Literal = literal;
        };
        switch (ch) {
        case '=':
            if (peekChar() == '=') {
                readChar();
                setToken(TokenType::EQ, "==");
            } else {
                setToken(TokenType::ASSIGN, "=");
            }
            break;
        case '+':
            setToken(TokenType::PLUS, "+");
            break;
        case '-':
            setToken(TokenType::MINUS, "-");
            break;
        case '*':
            setToken(TokenType::ASTERISK, "*");
            break;
        case '/':
            setToken(TokenType::SLASH, "/");
            break;
        case '<':
            if (peekChar() == '=') {
                readChar();
                setToken(TokenType::LE, "<=");
            } else {
                setToken(TokenType::LT, "<");
            }
            break;
        case '>':
            if (peekChar() == '=') {
                readChar();
                setToken(TokenType::GE, ">=");
            } else {
                setToken(TokenType::GT, ">");
            }
            break;
        case ';':
            setToken(TokenType::SEMICOLON, ";");
            break;
        case ',':
            setToken(TokenType::COMMA, ",");
            break;
        case '{':
            setToken(TokenType::LBRACE, "{");
            break;
        case '}':
            setToken(TokenType::RBRACE, "}");
            break;
        case '(':
            setToken(TokenType::LPAREN, "(");
            break;
        case ')':
            setToken(TokenType::RPAREN, ")");
            break;
        case '!':
            if (peekChar() == '=') {
                readChar();
                setToken(TokenType::NOT_EQ, "!=");
            } else {
                setToken(TokenType::BANG, "!");
            }
            break;
        case '\"':
            setToken(TokenType::STRING, readString());
            break;
        case '[':
            setToken(TokenType::LBRACKET, "[");
            break;
        case ']':
            setToken(TokenType::RBRACKET, "]");
            break;
        case 0:
            setToken(TokenType::END, "");
            break;
        default:
            if (isLetter()) {
                auto literal = readIdentifier();
                setToken(token::LookupIdent(literal), literal);
                return res;
            } else if (isDigit()) {
                auto [Lit, Typ] = readNumber();
                setToken(Typ, Lit);
                return res;
            } else {
                setToken(TokenType::ILLEGAL, string() + ch);
            }
            break;
        }
        readChar();
        return res;
    }
};

} // namespace lexer