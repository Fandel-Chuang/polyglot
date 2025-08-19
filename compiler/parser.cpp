#include "parser.h"
#include "error.h"
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0) {}

Token& Parser::peek() {
    return tokens[current];
}

Token& Parser::advance() {
    if (!isAtEnd()) current++;
    return tokens[current - 1];
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::EOF_TOKEN;
}

bool Parser::match(TokenType type) {
    if (peek().type == type) {
        advance();
        return true;
    }
    return false;
}

void Parser::consume(TokenType type, const std::string& message) {
    if (peek().type == type) {
        advance();
        return;
    }

    Token& token = peek();
    throw ParserError(message, token.line, token.column);
}

std::unique_ptr<Program> Parser::parse() {
    auto program = std::make_unique<Program>();

    std::cout << "   📋 解析器暂时返回空的AST" << std::endl;
    std::cout << "   📋 Token数量: " << tokens.size() << std::endl;

    // 简单的占位符实现
    // 后续将实现完整的语法解析

    return program;
}