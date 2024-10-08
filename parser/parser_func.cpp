#pragma once

#include "./parser.cpp"
#include <algorithm>

namespace parser {

using std::make_unique;
using std::move;
using std::shared_ptr;
using std::unique_ptr;

void Parser::registerAll() {
    // Prefix
    registerPrefixFunc(token::IDENT, parseIdentifier);
    registerPrefixFunc(token::INT, parseIntegerLiteral);
    registerPrefixFunc(token::DOUBLE, parseDoubleLiteral);
    registerPrefixFunc(token::MINUS, parsePrefixExpression);
    registerPrefixFunc(token::BANG, parsePrefixExpression);
    registerPrefixFunc(token::NOT, parsePrefixExpression);
    registerPrefixFunc(token::TRUE, parseBooleanLiteral);
    registerPrefixFunc(token::FALSE, parseBooleanLiteral);
    registerPrefixFunc(token::LPAREN, parseGroupedExpression);
    registerPrefixFunc(token::IF, parseIfExpression);
    // registerPrefixFunc(token::LBRACE, parseGroupedExpression);
    registerPrefixFunc(token::FUNCTION, parseFunctionLiteral);

    // Infix
    registerInfixFunc(token::AND, parseInfixExpression);
    registerInfixFunc(token::OR, parseInfixExpression);
    registerInfixFunc(token::PLUS, parseInfixExpression);
    registerInfixFunc(token::MINUS, parseInfixExpression);
    registerInfixFunc(token::ASTERISK, parseInfixExpression);
    registerInfixFunc(token::SLASH, parseInfixExpression);
    registerInfixFunc(token::EQ, parseInfixExpression);
    registerInfixFunc(token::NOT_EQ, parseInfixExpression);
    registerInfixFunc(token::LE, parseInfixExpression);
    registerInfixFunc(token::GE, parseInfixExpression);
    registerInfixFunc(token::LT, parseInfixExpression);
    registerInfixFunc(token::GT, parseInfixExpression);
    registerInfixFunc(token::LPAREN, parseCallExpression);
    registerInfixFunc(token::ASSIGN, parseInfixExpression);
    registerInfixFunc(token::IDENT, parseIdentInfixExpression);
    registerInfixFunc(token::TRUE, parseIdentInfixExpression);
    registerInfixFunc(token::FALSE, parseIdentInfixExpression);
}

unique_ptr<Expression> Parser::parseIdentifier() {
    return make_unique<Identifier>(curToken, curToken.Literal);
}

unique_ptr<Expression> Parser::parseIntegerLiteral() {
    auto res = make_unique<IntegerLiteral>();
    int val = 0;
    try {
        val = std::stoi(curToken.Literal);
    } catch (std::invalid_argument &e) {
        errors.push_back(
            format("could not parse {} as integer", curToken.Literal));
        return nullptr;
    }

    res->value = val;

    return res;
}

unique_ptr<Expression> Parser::parseDoubleLiteral() {
    auto res = make_unique<DoubleLiteral>();
    double val = 0;
    try {
        val = std::stod(curToken.Literal);
    } catch (std::invalid_argument &e) {
        errors.push_back(
            format("could not parse {} as double", curToken.Literal));
        return nullptr;
    }

    res->value = val;

    return res;
}

unique_ptr<Expression> Parser::parseBooleanLiteral() {
    auto res = make_unique<BooleanLiteral>();
    res->token = curToken;
    res->value = curTokenIs(token::TRUE);
    return res;
}

unique_ptr<Expression> Parser::parsePrefixExpression() {
    auto res = make_unique<PrefixExpression>();
    res->token = curToken;
    res->oper = curToken.Literal;
    nextToken();
    res->Right = parseExpression(PREFIX);
    return res;
}

unique_ptr<Expression>
Parser::parseInfixExpression(unique_ptr<Expression> left) {
    auto res = make_unique<InfixExpression>();
    res->Left = move(left);
    res->oper = curToken.Literal;
    res->token = curToken;

    Priority precedence = curPrecedence();

    nextToken();

    if (res->token.Type == token::ASSIGN) { // 给Assign开个特判....以后再大改
        res->Right = parseExpression(LOWEST);
    } else {
        res->Right = parseExpression(precedence);
    }

    return res;
}

unique_ptr<Expression>
Parser::parseIdentInfixExpression(unique_ptr<Expression> left) {
    auto res = make_unique<InfixExpression>();
    res->Left = move(left);
    res->oper = "*";
    res->token = token::Token{token::ASTERISK, "*"};

    Priority precedence = curPrecedence();

    res->Right = parseExpression(precedence);

    return res;
}

unique_ptr<Expression>
Parser::parseCallExpression(unique_ptr<Expression> func) {
    auto exp = make_unique<CallExpression>();
    exp->token = curToken;
    exp->Function = move(func);
    exp->Arguments = parseCallArguments();

    return exp;
}

vector<unique_ptr<Expression>> Parser::parseCallArguments() {
    vector<unique_ptr<Expression>> args;
    if (peekTokenIs(token::RPAREN)) {
        nextToken();
        return args;
    }

    nextToken();

    args.push_back(parseExpression(LOWEST));

    while (peekTokenIs(token::COMMA)) {
        nextToken();
        nextToken();
        args.push_back(parseExpression(LOWEST));
    }

    if (!expectToken(token::RPAREN)) {
        return args;
    }

    return args;
}

unique_ptr<Expression> Parser::parseGroupedExpression() {
    nextToken();
    auto exp = parseExpression(LOWEST);

    if (!expectToken(token::RPAREN)) {
        return nullptr;
    }
    return exp;
}

unique_ptr<Expression> Parser::parseIfExpression() {
    auto res = make_unique<IfExpression>();
    res->token = curToken;

    if (!expectToken(token::LPAREN)) {
        return nullptr;
    }

    nextToken();

    res->Condition = parseExpression(LOWEST);

    if (!expectToken(token::RPAREN)) {
        return nullptr;
    }

    if (!expectToken(token::LBRACE)) {
        return nullptr;
    }

    res->Consequence = parseBlockStatement();

    unique_ptr<IfExpression> Alter;

    if (peekTokenIs(token::ELSE)) {
        nextToken();
        if (peekTokenIs(token::IF)) {
            nextToken();
            auto _tmp = parseIfExpression().release()->cast<IfExpression>();
            Alter = unique_ptr<IfExpression>(_tmp);
        } else {
            Alter = make_unique<IfExpression>();
            Alter->token = curToken;
            if (!expectToken(token::LBRACE)) {
                return nullptr;
            }
            Alter->Condition = _TRUE;
            Alter->Consequence = parseBlockStatement();
        }

        res->Alternative = move(Alter);
    }

    return res;
}

unique_ptr<Expression> Parser::parseFunctionLiteral() {
    auto lit = make_unique<FunctionLiteral>();
    lit->token = curToken;

    if (!expectToken(token::LPAREN)) {
        return nullptr;
    }

    lit->Parameters = parseParameters();

    if (!expectToken(token::LBRACE)) {
        return nullptr;
    }

    lit->Body = parseBlockStatement();

    return lit;
}

vector<shared_ptr<Identifier>> Parser::parseParameters() {
    vector<shared_ptr<Identifier>> paras;
    if (peekTokenIs(token::RPAREN)) {
        nextToken();
        return {};
    }

    nextToken();

    paras.push_back(std::make_shared<Identifier>(curToken, curToken.Literal));

    while (peekTokenIs(token::COMMA)) {
        nextToken();
        nextToken();
        paras.push_back(
            std::make_shared<Identifier>(curToken, curToken.Literal));
    }

    if (!expectToken(token::RPAREN)) {
        return {};
    }
    return paras;
}

unique_ptr<Program> Parser::ParserProgram() {
    auto program = make_unique<Program>();
    while (curToken.Type != token::END) {
        // std::cerr << curToken.Literal << std::endl;
        auto statement = parseStatement();
        // std::cerr << statement->output() << std::endl;
        if (statement != nullptr) {
            program->Statements.push_back(move(statement));
        }
        nextToken();
    }
    return program;
}
unique_ptr<Statement> Parser::parseStatement() {
    switch (curToken.Type) {
    case token::LET:
        return parseLetStatement();
    case token::RETURN:
        return parseReturnStatement();
    case token::FUNCTION:
        return parseFunctionStatement();
    case token::FOR:
        return parseForStatement();
    case token::LBRACE:
        return parseBlockStatement();
    default:
        return parseExpressionStatement();
    }
}

unique_ptr<BlockStatement> Parser::parseBlockStatement() {
    auto statement = make_unique<BlockStatement>();
    statement->token = curToken;
    nextToken();

    while (!curTokenIs(token::RBRACE) && !curTokenIs(token::END)) {
        auto stmt = parseStatement();
        if (stmt != nullptr) {
            statement->Statements.push_back(move(stmt));
        }
        nextToken();
    }
    return statement;
}

unique_ptr<LetStatement> Parser::parseLetStatement() {
    auto statement = make_unique<LetStatement>();
    statement->token = curToken;
    if (!expectToken(token::IDENT)) {
        return nullptr;
    }

    statement->Name = make_unique<Identifier>(curToken, curToken.Literal);

    if (!expectToken(token::ASSIGN)) {
        return nullptr;
    }

    nextToken();

    statement->Value = parseExpression(LOWEST);

    while (!curTokenIs(token::SEMICOLON) && !curTokenIs(token::END)) {
        nextToken();
    }

    return statement;
}

unique_ptr<ReturnStatement> Parser::parseReturnStatement() {
    auto statement = make_unique<ReturnStatement>();
    statement->token = curToken;
    nextToken();

    statement->ReturnValue = parseExpression(LOWEST);

    while (!curTokenIs(token::SEMICOLON) && !curTokenIs(token::END)) {
        nextToken();
    }
    return statement;
}

unique_ptr<ForStatement> Parser::parseForStatement() {
    auto statement = make_unique<ForStatement>();
    statement->token = curToken;
    if (!expectToken(token::LPAREN)) {
        return nullptr;
    }
    nextToken();

    if (!curTokenIs(token::SEMICOLON)) {
        statement->Init = parseExpression(LOWEST);
        if (!expectToken(token::SEMICOLON)) {
            return nullptr;
        }
    }
    nextToken();

    if (curTokenIs(token::SEMICOLON)) {
        statement->Condition = _FALSE;
    } else {
        statement->Condition = parseExpression(LOWEST);
        if (!expectToken(token::SEMICOLON)) {
            return nullptr;
        }
    }

    if (!peekTokenIs(token::RPAREN)) {
        nextToken();
        statement->PostAction = parseExpression(LOWEST);
    }

    if (!expectToken(token::RPAREN)) {
        return nullptr;
    }
    if (!expectToken(token::LBRACE)) {
        return nullptr;
    }
    statement->Body = parseBlockStatement();

    return statement;
}

unique_ptr<Expression> Parser::parseExpression(Priority precedence) {
    if (!prefixParseFuncs.count(curToken.Type)) {
        noPrefixParseFnError(curToken.Type);
        // std::cerr << "123\n";
        return nullptr;
    }
    auto prefixFunc = prefixParseFuncs[curToken.Type];

    auto leftExpr = (this->*prefixFunc)();

    while (!peekTokenIs(token::SEMICOLON) && precedence < peekPrecedence()) {
        if (!infixParseFuncs.count(peekToken.Type)) {
            return leftExpr;
        }

        auto infixFunc = infixParseFuncs[peekToken.Type];

        nextToken();

        leftExpr = (this->*infixFunc)(move(leftExpr));
    }

    return leftExpr;
}

unique_ptr<ExpressionStatement> Parser::parseExpressionStatement() {
    auto statement = make_unique<ExpressionStatement>();
    statement->token = curToken;

    statement->_expression = parseExpression(LOWEST);

    if (peekTokenIs(token::SEMICOLON)) {
        nextToken();
    }

    return statement;
}

unique_ptr<FunctionStatement> Parser::parseFunctionStatement() {
    auto statement = make_unique<FunctionStatement>();
    statement->token = curToken;
    if (!expectToken(token::IDENT)) {
        return nullptr;
    }
    statement->Name = make_unique<Identifier>(curToken, curToken.Literal);

    if (!expectToken(token::LPAREN)) {
        return nullptr;
    }

    statement->Parameters = parseParameters();

    if (!expectToken(token::LBRACE)) {
        return nullptr;
    }

    statement->Body = parseBlockStatement();

    return statement;
}

} // namespace parser
