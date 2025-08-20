#pragma once

#include "ast.h"
#include <iostream>
#include <memory>
#include <map>
#include <any>
#include <string>

namespace polyglot {

// AST 值类型
class ASTValue {
public:
    enum Type { INT, FLOAT, STRING, BOOL, VOID, OBJECT };

private:
    Type type;
    std::any value;

public:
    ASTValue() : type(VOID) {}
    ASTValue(int v) : type(INT), value(v) {}
    ASTValue(double v) : type(FLOAT), value(v) {}
    ASTValue(const std::string& v) : type(STRING), value(v) {}
    ASTValue(bool v) : type(BOOL), value(v) {}

    Type getType() const { return type; }

    template<typename T>
    T get() const { return std::any_cast<T>(value); }

    std::string toString() const {
        switch(type) {
            case INT: return std::to_string(std::any_cast<int>(value));
            case FLOAT: return std::to_string(std::any_cast<double>(value));
            case STRING: return std::any_cast<std::string>(value);
            case BOOL: return std::any_cast<bool>(value) ? "true" : "false";
            case VOID: return "void";
            default: return "unknown";
        }
    }
};

// AST 环境（作用域）
class Environment {
private:
    std::map<std::string, ASTValue> variables;
    std::shared_ptr<Environment> parent;

public:
    Environment() : parent(nullptr) {}
    Environment(std::shared_ptr<Environment> p) : parent(p) {}

    void define(const std::string& name, const ASTValue& value) {
        variables[name] = value;
    }

    ASTValue get(const std::string& name) {
        if (variables.find(name) != variables.end()) {
            return variables[name];
        }
        if (parent) {
            return parent->get(name);
        }
        throw std::runtime_error("Undefined variable: " + name);
    }

    void assign(const std::string& name, const ASTValue& value) {
        if (variables.find(name) != variables.end()) {
            variables[name] = value;
            return;
        }
        if (parent) {
            parent->assign(name, value);
            return;
        }
        throw std::runtime_error("Undefined variable: " + name);
    }
};

// AST 解释器
class ASTInterpreter {
private:
    std::shared_ptr<Environment> environment;
    std::map<std::string, std::unique_ptr<FunctionDecl>> functions;

public:
    ASTInterpreter() {
        environment = std::make_shared<Environment>();
        // 添加内置函数
        setupBuiltins();
    }

    // 解释执行AST
    ASTValue interpret(std::unique_ptr<Program>& program);

    // 访问者模式方法
    ASTValue visit(ASTNode* node);
    ASTValue visitImport(ImportDecl* node);
    ASTValue visitVariableDecl(VariableDecl* node);
    ASTValue visitFunctionDecl(FunctionDecl* node);
    ASTValue visitStructDecl(StructDecl* node);
    ASTValue visitBlock(Block* node);
    ASTValue visitReturnStmt(ReturnStmt* node);
    ASTValue visitExpressionStmt(ExpressionStmt* node);
    ASTValue visitIdentifier(Identifier* node);
    ASTValue visitLiteral(Literal* node);
    ASTValue visitBinaryOp(BinaryOp* node);
    ASTValue visitFunctionCall(FunctionCall* node);

private:
    void setupBuiltins();
    ASTValue callBuiltinFunction(const std::string& name,
                               const std::vector<ASTValue>& args);
};

// AST 可视化器
class ASTVisualizer {
public:
    static void printAST(ASTNode* node, int depth = 0);
    static void exportToJSON(ASTNode* node, const std::string& filename);
    static void exportToDot(ASTNode* node, const std::string& filename);

private:
    static void printIndent(int depth);
    static std::string nodeToString(ASTNode* node);
};

// AST 分析器
class ASTAnalyzer {
public:
    struct AnalysisResult {
        int totalNodes = 0;
        int functionCount = 0;
        int variableCount = 0;
        int maxDepth = 0;
        std::vector<std::string> usedSymbols;
        std::vector<std::string> warnings;
        std::vector<std::string> errors;
    };

    static AnalysisResult analyze(Program* program);

private:
    static void analyzeNode(ASTNode* node, AnalysisResult& result, int depth = 0);
};

}