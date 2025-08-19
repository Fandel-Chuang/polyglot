#pragma once

#include "ast.h"
#include "error.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

// 符号信息
struct Symbol {
    std::string name;
    std::string type;
    bool isConst = false;
    int line = 0;
    int column = 0;

    Symbol() = default;
    Symbol(const std::string& n, const std::string& t, bool isC = false)
        : name(n), type(t), isConst(isC) {}
};

// 函数符号信息
struct FunctionSymbol : public Symbol {
    std::vector<std::string> paramTypes;
    std::string returnType;

    FunctionSymbol(const std::string& n, const std::vector<std::string>& params,
                   const std::string& ret)
        : Symbol(n, "function"), paramTypes(params), returnType(ret) {}
};

// 作用域管理
class SymbolTable {
private:
    std::vector<std::unordered_map<std::string, std::unique_ptr<Symbol>>> scopes;

public:
    SymbolTable();
    ~SymbolTable() = default;

    // 作用域管理
    void enterScope();
    void exitScope();

    // 符号操作
    bool declareSymbol(const std::string& name, std::unique_ptr<Symbol> symbol);
    Symbol* lookupSymbol(const std::string& name);
    bool isSymbolInCurrentScope(const std::string& name);

    // 调试输出
    void printCurrentScope();
};

// 简单的语义错误信息结构
struct SemanticErrorInfo {
    std::string message;
    int line;
    int column;

    SemanticErrorInfo(const std::string& msg, int l = 0, int c = 0)
        : message(msg), line(l), column(c) {}
};

class SemanticAnalyzer {
private:
    SymbolTable symbolTable;
    std::vector<SemanticErrorInfo> errors;
    bool hasErrors = false;

    // 内置函数初始化
    void initializeBuiltinFunctions();

    // 内置类型检查
    bool isBuiltinType(const std::string& type);
    std::string getExpressionType(Expression* expr);

    // AST访问方法
    void visitProgram(Program* program);
    void visitImportDecl(ImportDecl* importDecl);
    void visitFunctionDecl(FunctionDecl* funcDecl);
    void visitStructDecl(StructDecl* structDecl);
    void visitImplBlock(ImplBlock* implBlock);
    void visitVariableDecl(VariableDecl* varDecl);
    void visitBlock(Block* block);
    void visitReturnStmt(ReturnStmt* returnStmt);
    void visitExpressionStmt(ExpressionStmt* exprStmt);

    // 表达式类型检查
    std::string visitExpression(Expression* expr);
    std::string visitIdentifier(Identifier* identifier);
    std::string visitLiteral(Literal* literal);
    std::string visitBinaryOp(BinaryOp* binaryOp);

    // 错误处理
    void reportError(const std::string& message, ASTNode* node = nullptr);
    bool isTypeCompatible(const std::string& expected, const std::string& actual);

public:
    SemanticAnalyzer();
    ~SemanticAnalyzer() = default;

    // 主分析入口
    bool analyze(const std::unique_ptr<Program>& program);

    // 错误信息获取
    const std::vector<SemanticErrorInfo>& getErrors() const { return errors; }
    bool hasSemanticErrors() const { return hasErrors; }

    // 调试输出
    void printErrors();
};