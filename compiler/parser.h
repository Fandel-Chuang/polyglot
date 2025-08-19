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

    // 解析函数
    std::unique_ptr<ASTNode> parseTopLevelStatement();
    std::unique_ptr<ASTNode> parseImport();
    std::unique_ptr<StructDecl> parseStructDef();
    std::unique_ptr<ImplBlock> parseImplBlock();
    std::unique_ptr<FunctionDecl> parseFunctionDef();
    std::unique_ptr<VariableDecl> parseVariableDecl();

    std::unique_ptr<Block> parseBlock();
    // 解析各种语句类型
    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<Statement> parseVariableDeclStmt();
    std::unique_ptr<ReturnStmt> parseReturnStmt();
    std::unique_ptr<ExpressionStmt> parseExpressionStmt();

    std::unique_ptr<Expression> parseExpression();
    std::unique_ptr<Expression> parseAssignmentExpression();
    std::unique_ptr<Expression> parseLogicalOrExpression();
    std::unique_ptr<Expression> parseLogicalAndExpression();
    std::unique_ptr<Expression> parseEqualityExpression();
    std::unique_ptr<Expression> parseRelationalExpression();
    std::unique_ptr<Expression> parseAdditiveExpression();
    std::unique_ptr<Expression> parseMultiplicativeExpression();
    std::unique_ptr<Expression> parseUnaryExpression();
    std::unique_ptr<Expression> parsePrimaryExpression();

public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<Program> parse();
};