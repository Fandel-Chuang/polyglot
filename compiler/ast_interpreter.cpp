#include "ast_interpreter.h"
#include <iostream>
#include <fstream>
#include <typeinfo>
#include <sstream>

namespace polyglot {

// AST解释器实现
ASTValue ASTInterpreter::interpret(std::unique_ptr<Program>& program) {
    ASTValue result;

    std::cout << "🚀 开始解释执行AST..." << std::endl;

    for (auto& stmt : program->statements) {
        result = visit(stmt.get());
    }

    std::cout << "✅ AST解释执行完成" << std::endl;
    return result;
}

ASTValue ASTInterpreter::visit(ASTNode* node) {
    // 使用dynamic_cast进行类型判断
    if (auto import = dynamic_cast<ImportDecl*>(node)) {
        return visitImport(import);
    } else if (auto varDecl = dynamic_cast<VariableDecl*>(node)) {
        return visitVariableDecl(varDecl);
    } else if (auto funcDecl = dynamic_cast<FunctionDecl*>(node)) {
        return visitFunctionDecl(funcDecl);
    } else if (auto structDecl = dynamic_cast<StructDecl*>(node)) {
        return visitStructDecl(structDecl);
    } else if (auto block = dynamic_cast<Block*>(node)) {
        return visitBlock(block);
    } else if (auto returnStmt = dynamic_cast<ReturnStmt*>(node)) {
        return visitReturnStmt(returnStmt);
    } else if (auto exprStmt = dynamic_cast<ExpressionStmt*>(node)) {
        return visitExpressionStmt(exprStmt);
    } else if (auto identifier = dynamic_cast<Identifier*>(node)) {
        return visitIdentifier(identifier);
    } else if (auto literal = dynamic_cast<Literal*>(node)) {
        return visitLiteral(literal);
    } else if (auto binaryOp = dynamic_cast<BinaryOp*>(node)) {
        return visitBinaryOp(binaryOp);
    } else if (auto funcCall = dynamic_cast<FunctionCall*>(node)) {
        return visitFunctionCall(funcCall);
    }

    std::cerr << "⚠️ 未识别的AST节点类型" << std::endl;
    return ASTValue();
}

ASTValue ASTInterpreter::visitImport(ImportDecl* node) {
    std::cout << "📦 导入模块: " << node->moduleName << std::endl;
    return ASTValue();
}

ASTValue ASTInterpreter::visitVariableDecl(VariableDecl* node) {
    ASTValue value;
    if (node->initializer) {
        value = visit(node->initializer.get());
    }

    environment->define(node->name, value);
    std::cout << "📝 定义变量: " << node->name << " = " << value.toString() << std::endl;

    return ASTValue();
}

ASTValue ASTInterpreter::visitFunctionDecl(FunctionDecl* node) {
    // 将函数存储起来，供后续调用
    std::cout << "🔧 定义函数: " << node->name << std::endl;
    return ASTValue();
}

ASTValue ASTInterpreter::visitStructDecl(StructDecl* node) {
    std::cout << "🏗️ 定义结构体: " << node->name << std::endl;
    return ASTValue();
}

ASTValue ASTInterpreter::visitBlock(Block* node) {
    // 创建新的作用域
    auto previous = environment;
    environment = std::make_shared<Environment>(environment);

    ASTValue result;
    for (auto& stmt : node->statements) {
        result = visit(stmt.get());
    }

    // 恢复上一层作用域
    environment = previous;
    return result;
}

ASTValue ASTInterpreter::visitReturnStmt(ReturnStmt* node) {
    ASTValue value;
    if (node->value) {
        value = visit(node->value.get());
    }

    std::cout << "↩️ 返回值: " << value.toString() << std::endl;
    return value;
}

ASTValue ASTInterpreter::visitExpressionStmt(ExpressionStmt* node) {
    return visit(node->expression.get());
}

ASTValue ASTInterpreter::visitIdentifier(Identifier* node) {
    try {
        return environment->get(node->name);
    } catch (const std::runtime_error& e) {
        std::cerr << "❌ " << e.what() << std::endl;
        return ASTValue();
    }
}

ASTValue ASTInterpreter::visitLiteral(Literal* node) {
    if (node->type == "int") {
        return ASTValue(std::stoi(node->value));
    } else if (node->type == "float") {
        return ASTValue(std::stod(node->value));
    } else if (node->type == "string") {
        return ASTValue(node->value);
    } else if (node->type == "bool") {
        return ASTValue(node->value == "true");
    }

    return ASTValue();
}

ASTValue ASTInterpreter::visitBinaryOp(BinaryOp* node) {
    ASTValue left = visit(node->left.get());
    ASTValue right = visit(node->right.get());

    if (node->operator_ == "+") {
        if (left.getType() == ASTValue::INT && right.getType() == ASTValue::INT) {
            return ASTValue(left.get<int>() + right.get<int>());
        }
    } else if (node->operator_ == "-") {
        if (left.getType() == ASTValue::INT && right.getType() == ASTValue::INT) {
            return ASTValue(left.get<int>() - right.get<int>());
        }
    } else if (node->operator_ == "*") {
        if (left.getType() == ASTValue::INT && right.getType() == ASTValue::INT) {
            return ASTValue(left.get<int>() * right.get<int>());
        }
    } else if (node->operator_ == "/") {
        if (left.getType() == ASTValue::INT && right.getType() == ASTValue::INT) {
            return ASTValue(left.get<int>() / right.get<int>());
        }
    }

    std::cerr << "⚠️ 不支持的二元运算: " << node->operator_ << std::endl;
    return ASTValue();
}

ASTValue ASTInterpreter::visitFunctionCall(FunctionCall* node) {
    std::vector<ASTValue> args;
    for (auto& arg : node->arguments) {
        args.push_back(visit(arg.get()));
    }

    return callBuiltinFunction(node->name, args);
}

void ASTInterpreter::setupBuiltins() {
    // 内置函数将在运行时处理
}

ASTValue ASTInterpreter::callBuiltinFunction(const std::string& name,
                                           const std::vector<ASTValue>& args) {
    if (name == "print" || name == "打印") {
        std::cout << "📢 输出: ";
        for (size_t i = 0; i < args.size(); ++i) {
            std::cout << args[i].toString();
            if (i < args.size() - 1) std::cout << " ";
        }
        std::cout << std::endl;
        return ASTValue();
    }

    std::cerr << "❌ 未知的内置函数: " << name << std::endl;
    return ASTValue();
}

// AST可视化器实现
void ASTVisualizer::printAST(ASTNode* node, int depth) {
    printIndent(depth);
    std::cout << nodeToString(node) << std::endl;

    // 递归打印子节点
    if (auto program = dynamic_cast<Program*>(node)) {
        for (auto& stmt : program->statements) {
            printAST(stmt.get(), depth + 1);
        }
    } else if (auto block = dynamic_cast<Block*>(node)) {
        for (auto& stmt : block->statements) {
            printAST(stmt.get(), depth + 1);
        }
    } else if (auto funcDecl = dynamic_cast<FunctionDecl*>(node)) {
        if (funcDecl->body) {
            printAST(funcDecl->body.get(), depth + 1);
        }
    } else if (auto varDecl = dynamic_cast<VariableDecl*>(node)) {
        if (varDecl->initializer) {
            printAST(varDecl->initializer.get(), depth + 1);
        }
    } else if (auto binaryOp = dynamic_cast<BinaryOp*>(node)) {
        printAST(binaryOp->left.get(), depth + 1);
        printAST(binaryOp->right.get(), depth + 1);
    }
}

void ASTVisualizer::printIndent(int depth) {
    for (int i = 0; i < depth; ++i) {
        std::cout << "  ";
    }
}

std::string ASTVisualizer::nodeToString(ASTNode* node) {
    if (auto program = dynamic_cast<Program*>(node)) {
        return "🌍 Program";
    } else if (auto import = dynamic_cast<ImportDecl*>(node)) {
        return "📦 Import: " + import->moduleName;
    } else if (auto varDecl = dynamic_cast<VariableDecl*>(node)) {
        return "📝 Variable: " + varDecl->name;
    } else if (auto funcDecl = dynamic_cast<FunctionDecl*>(node)) {
        return "🔧 Function: " + funcDecl->name;
    } else if (auto structDecl = dynamic_cast<StructDecl*>(node)) {
        return "🏗️ Struct: " + structDecl->name;
    } else if (auto block = dynamic_cast<Block*>(node)) {
        return "📦 Block";
    } else if (auto returnStmt = dynamic_cast<ReturnStmt*>(node)) {
        return "↩️ Return";
    } else if (auto identifier = dynamic_cast<Identifier*>(node)) {
        return "🔗 Identifier: " + identifier->name;
    } else if (auto literal = dynamic_cast<Literal*>(node)) {
        return "💎 Literal: " + literal->value + " (" + literal->type + ")";
    } else if (auto binaryOp = dynamic_cast<BinaryOp*>(node)) {
        return "⚙️ BinaryOp: " + binaryOp->operator_;
    } else if (auto funcCall = dynamic_cast<FunctionCall*>(node)) {
        return "📞 FunctionCall: " + funcCall->name;
    }

    return "❓ Unknown";
}

void ASTVisualizer::exportToJSON(ASTNode* node, const std::string& filename) {
    std::ofstream file(filename);
    file << "{\n  \"type\": \"" << nodeToString(node) << "\"\n}" << std::endl;
    std::cout << "📄 AST已导出到JSON: " << filename << std::endl;
}

void ASTVisualizer::exportToDot(ASTNode* node, const std::string& filename) {
    std::ofstream file(filename);
    file << "digraph AST {\n";
    file << "  node [shape=box];\n";
    file << "  \"root\" [label=\"" << nodeToString(node) << "\"];\n";
    file << "}" << std::endl;
    std::cout << "🎨 AST已导出到DOT格式: " << filename << std::endl;
}

// AST分析器实现
ASTAnalyzer::AnalysisResult ASTAnalyzer::analyze(Program* program) {
    AnalysisResult result;

    std::cout << "🔍 开始分析AST..." << std::endl;

    for (auto& stmt : program->statements) {
        analyzeNode(stmt.get(), result, 0);
    }

    std::cout << "📊 AST分析完成:" << std::endl;
    std::cout << "  - 总节点数: " << result.totalNodes << std::endl;
    std::cout << "  - 函数数量: " << result.functionCount << std::endl;
    std::cout << "  - 变量数量: " << result.variableCount << std::endl;
    std::cout << "  - 最大深度: " << result.maxDepth << std::endl;

    return result;
}

void ASTAnalyzer::analyzeNode(ASTNode* node, AnalysisResult& result, int depth) {
    result.totalNodes++;
    result.maxDepth = std::max(result.maxDepth, depth);

    if (dynamic_cast<FunctionDecl*>(node)) {
        result.functionCount++;
    } else if (dynamic_cast<VariableDecl*>(node)) {
        result.variableCount++;
    }

    // 继续分析子节点...
}

}