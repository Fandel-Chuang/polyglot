#pragma once

#include "lexer.h"
#include "ast.h"
#include <memory>

class Parser {
private:
    std::vector<Token> tokens;
    size_t current;

    Token& peek();
    Token& advance();
    bool isAtEnd();
    bool match(TokenType type);
    void consume(TokenType type, const std::string& message);

public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<Program> parse();
};