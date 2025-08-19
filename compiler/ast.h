#pragma once

#include <memory>
#include <vector>
#include <string>

// 前向声明
struct ASTNode;

// AST 节点基类
struct ASTNode {
    virtual ~ASTNode() = default;
    int line = 0;
    int column = 0;
};

// 导入声明
struct ImportDecl : public ASTNode {
    std::string moduleName;

    ImportDecl(const std::string& module) : moduleName(module) {}
};

// 程序根节点
struct Program : public ASTNode {
    std::vector<std::unique_ptr<ASTNode>> statements;
};

// 语句基类 - 需要提前定义
struct Statement : public ASTNode {
    // 语句基类
};

// 类型定义
struct TypeNode : public ASTNode {
    std::string name;

    TypeNode(const std::string& typeName) : name(typeName) {}
};

// 变量声明 - 继承自Statement，因为它也可以作为语句
struct VariableDecl : public Statement {
    std::string name;
    std::unique_ptr<TypeNode> type;
    std::unique_ptr<ASTNode> initializer;
    bool isConst = false;
};

// 函数声明
struct FunctionDecl : public ASTNode {
    std::string name;
    std::vector<std::unique_ptr<VariableDecl>> parameters;
    std::unique_ptr<TypeNode> returnType;
    std::unique_ptr<ASTNode> body;
};

// 结构体定义
struct StructDecl : public ASTNode {
    std::string name;
    std::vector<std::unique_ptr<VariableDecl>> fields;
};

// 实现块
struct ImplBlock : public ASTNode {
    std::string structName;
    std::vector<std::unique_ptr<FunctionDecl>> methods;
};

// 表达式基类
struct Expression : public ASTNode {
    // 表达式基类
};

// 标识符表达式
struct Identifier : public Expression {
    std::string name;

    Identifier(const std::string& n) : name(n) {}
};

// 字面值表达式
struct Literal : public Expression {
    std::string value;
    std::string type; // "int", "float", "string", "bool", "char"

    Literal(const std::string& v, const std::string& t) : value(v), type(t) {}
};

// 二元运算表达式
struct BinaryOp : public Expression {
    std::unique_ptr<Expression> left;
    std::string operator_;
    std::unique_ptr<Expression> right;
};

// 块语句
struct Block : public Statement {
    std::vector<std::unique_ptr<Statement>> statements;
};

// 返回语句
struct ReturnStmt : public Statement {
    std::unique_ptr<Expression> value;
};

// 表达式语句
struct ExpressionStmt : public Statement {
    std::unique_ptr<Expression> expression;
};
