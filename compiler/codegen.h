#pragma once

#include "ast.h"
#include <string>

class CodeGenerator {
private:
    std::string output;

    void generateStatement(ASTNode* node);
    void generateFunction(FunctionDecl* funcDecl);
    void generateStruct(StructDecl* structDecl);
    void generateImplBlock(ImplBlock* implBlock);
    void generateVariableDecl(VariableDecl* varDecl);
    void generateReturnStmt(ReturnStmt* returnStmt);
    void generateExpressionStmt(ExpressionStmt* exprStmt);
    void generateBlock(Block* block);
    void generateExpression(ASTNode* expr);
    void generateFunctionCall(FunctionCall* funcCall);
    void generateLiteral(Literal* literal);
    void generateBinaryOp(BinaryOp* binaryOp);

    std::string convertType(const std::string& polyglotType);
    int countLines(const std::string& code);

public:
    std::string generate(const std::unique_ptr<Program>& program);
};
