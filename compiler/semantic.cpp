#include "semantic.h"
#include <iostream>
#include <typeinfo>
#include <functional>
#include <algorithm>

// ========== SymbolTable 实现 ==========

SymbolTable::SymbolTable() {
    // 创建全局作用域
    enterScope();
}

void SymbolTable::enterScope() {
    scopes.emplace_back();
}

void SymbolTable::exitScope() {
    if (!scopes.empty()) {
        scopes.pop_back();
    }
}

bool SymbolTable::declareSymbol(const std::string& name, std::unique_ptr<Symbol> symbol) {
    if (scopes.empty()) {
        return false;
    }

    // 检查当前作用域是否已存在同名符号
    auto& currentScope = scopes.back();
    if (currentScope.find(name) != currentScope.end()) {
        return false; // 重复声明
    }

    // 记录符号位置信息
    symbol->name = name;
    currentScope[name] = std::move(symbol);
    return true;
}

Symbol* SymbolTable::lookupSymbol(const std::string& name) {
    // 从内层作用域到外层作用域查找
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) {
            return found->second.get();
        }
    }
    return nullptr;
}

bool SymbolTable::isSymbolInCurrentScope(const std::string& name) {
    if (scopes.empty()) {
        return false;
    }

    auto& currentScope = scopes.back();
    return currentScope.find(name) != currentScope.end();
}

void SymbolTable::printCurrentScope() {
    if (scopes.empty()) {
        std::cout << "   📋 当前作用域为空" << std::endl;
        return;
    }

    auto& currentScope = scopes.back();
    std::cout << "   📋 当前作用域符号数量: " << currentScope.size() << std::endl;
    for (const auto& pair : currentScope) {
        std::cout << "     - " << pair.first << " : " << pair.second->type << std::endl;
    }
}

// ========== SemanticAnalyzer 实现 ==========

SemanticAnalyzer::SemanticAnalyzer() : hasErrors(false) {
    // 初始化内置函数符号
    initializeBuiltinFunctions();
}

void SemanticAnalyzer::initializeBuiltinFunctions() {
    // 添加内置的 print 函数
    // print(string) -> void
    std::vector<std::string> printParams = {"string"};
    auto printFunc = std::make_unique<FunctionSymbol>("print", printParams, "void");
    symbolTable.declareSymbol("print", std::move(printFunc));

    // 添加内置的 打印 函数 (中文版本)
    auto printChineseFunc = std::make_unique<FunctionSymbol>("打印", printParams, "void");
    symbolTable.declareSymbol("打印", std::move(printChineseFunc));
}

bool SemanticAnalyzer::analyze(const std::unique_ptr<Program>& program) {
    std::cout << "   🧠 开始语义分析..." << std::endl;

    errors.clear();
    hasErrors = false;

    if (program) {
        // 常规语义遍历
        visitProgram(program.get());
        // 构建类型依赖图（骨架）并做环检测
        buildTypeGraph(program.get());
        std::vector<std::string> cyclePath;
        if (detectCycle(cyclePath)) {
            hasErrors = true;
            std::string pathStr;
            for (size_t i = 0; i < cyclePath.size(); ++i) {
                if (i) pathStr += " -> ";
                pathStr += cyclePath[i];
            }
            errors.emplace_back(
                std::string("编译错误：检测到循环引用\n") + pathStr +
                "\n建议：使用弱引用（~=）打破循环");
        }
    }

    if (hasErrors) {
        std::cout << "   ❌ 语义分析发现 " << errors.size() << " 个错误" << std::endl;
        printErrors();
        return false;
    } else {
        std::cout << "   ✅ 语义分析完成，无错误" << std::endl;
        return true;
    }
}

void SemanticAnalyzer::visitProgram(Program* program) {
    // 遍历所有顶级声明
    for (const auto& stmt : program->statements) {
        if (auto importDecl = dynamic_cast<ImportDecl*>(stmt.get())) {
            visitImportDecl(importDecl);
        } else if (auto funcDecl = dynamic_cast<FunctionDecl*>(stmt.get())) {
            visitFunctionDecl(funcDecl);
        } else if (auto structDecl = dynamic_cast<StructDecl*>(stmt.get())) {
            visitStructDecl(structDecl);
        } else if (auto implBlock = dynamic_cast<ImplBlock*>(stmt.get())) {
            visitImplBlock(implBlock);
        } else if (auto varDecl = dynamic_cast<VariableDecl*>(stmt.get())) {
            visitVariableDecl(varDecl);
        }
    }
}

void SemanticAnalyzer::visitImportDecl(ImportDecl* importDecl) {
    // 验证模块名
    if (importDecl->moduleName.empty()) {
        reportError("模块名不能为空", importDecl);
        return;
    }

    std::cout << "     导入模块: " << importDecl->moduleName << std::endl;
}

void SemanticAnalyzer::visitFunctionDecl(FunctionDecl* funcDecl) {
    // 检查函数名
    if (funcDecl->name.empty()) {
        reportError("函数名不能为空", funcDecl);
        return;
    }

    // 检查是否重复声明
    if (symbolTable.isSymbolInCurrentScope(funcDecl->name)) {
        reportError("函数 '" + funcDecl->name + "' 重复声明", funcDecl);
        return;
    }

    // 构建参数类型列表
    std::vector<std::string> paramTypes;
    for (const auto& param : funcDecl->parameters) {
        if (param->type) {
            paramTypes.push_back(param->type->name);
        } else {
            paramTypes.push_back("auto"); // 类型推导
        }
    }

    // 获取返回类型
    std::string returnType = "void";
    if (funcDecl->returnType) {
        returnType = funcDecl->returnType->name;
    }

    // 注册函数符号（返回类型暂不做严格校验）
    auto funcSymbol = std::make_unique<FunctionSymbol>(
        funcDecl->name, paramTypes, returnType);
    funcSymbol->line = funcDecl->line;
    funcSymbol->column = funcDecl->column;

    if (!symbolTable.declareSymbol(funcDecl->name, std::move(funcSymbol))) {
        reportError("无法声明函数: " + funcDecl->name, funcDecl);
        return;
    }

    // 进入函数作用域分析函数体
    symbolTable.enterScope();

    // 将参数加入函数作用域
    for (const auto& param : funcDecl->parameters) {
        visitVariableDecl(param.get());
    }

    // 分析函数体
    if (funcDecl->body) {
        if (auto block = dynamic_cast<Block*>(funcDecl->body.get())) {
            visitBlock(block);
        }
    }

    symbolTable.exitScope();

    std::cout << "     函数声明: " << funcDecl->name
              << "(" << paramTypes.size() << " 参数) -> " << returnType << std::endl;
}

void SemanticAnalyzer::visitStructDecl(StructDecl* structDecl) {
    // 暂时仅登记类型名，字段解析待 AST 支持
    std::cout << "     结构体声明: " << structDecl->name << std::endl;
    typeGraph.addVertex(structDecl->name);
}

void SemanticAnalyzer::visitImplBlock(ImplBlock* implBlock) {
    std::cout << "     实现块: " << implBlock->structName << " (暂时跳过)" << std::endl;
}

void SemanticAnalyzer::visitVariableDecl(VariableDecl* varDecl) {
    // 检查变量名
    if (varDecl->name.empty()) {
        reportError("变量名不能为空", varDecl);
        return;
    }

    // 检查当前作用域重复声明
    if (symbolTable.isSymbolInCurrentScope(varDecl->name)) {
        reportError("变量 '" + varDecl->name + "' 重复声明", varDecl);
        return;
    }

    // 确定变量类型（简化版）
    std::string varType = "auto";
    if (varDecl->type) {
        varType = varDecl->type->name;
    } else if (varDecl->initializer) {
        varType = getExpressionType(dynamic_cast<Expression*>(varDecl->initializer.get()));
    }

    // 注册变量符号
    auto varSymbol = std::make_unique<Symbol>(varDecl->name, varType, varDecl->isConst);
    varSymbol->line = varDecl->line;
    varSymbol->column = varDecl->column;

    if (!symbolTable.declareSymbol(varDecl->name, std::move(varSymbol))) {
        reportError("无法声明变量: " + varDecl->name, varDecl);
        return;
    }

    std::cout << "     变量声明: " << varDecl->name << " : " << varType
              << (varDecl->isConst ? " (常量)" : "") << std::endl;
}

void SemanticAnalyzer::visitBlock(Block* block) {
    symbolTable.enterScope();

    for (const auto& stmt : block->statements) {
        if (auto varDecl = dynamic_cast<VariableDecl*>(stmt.get())) {
            visitVariableDecl(varDecl);
        } else if (auto returnStmt = dynamic_cast<ReturnStmt*>(stmt.get())) {
            visitReturnStmt(returnStmt);
        } else if (auto exprStmt = dynamic_cast<ExpressionStmt*>(stmt.get())) {
            visitExpressionStmt(exprStmt);
        } else if (auto nestedBlock = dynamic_cast<Block*>(stmt.get())) {
            visitBlock(nestedBlock);
        }
    }

    symbolTable.exitScope();
}

void SemanticAnalyzer::visitReturnStmt(ReturnStmt* returnStmt) {
    if (returnStmt->value) {
        std::string returnType = visitExpression(dynamic_cast<Expression*>(returnStmt->value.get()));
        std::cout << "     返回语句: " << returnType << std::endl;
    } else {
        std::cout << "     返回语句: void" << std::endl;
    }
}

void SemanticAnalyzer::visitExpressionStmt(ExpressionStmt* exprStmt) {
    if (exprStmt->expression) {
        visitExpression(dynamic_cast<Expression*>(exprStmt->expression.get()));
    }
}

std::string SemanticAnalyzer::visitExpression(Expression* expr) {
    if (!expr) {
        return "void";
    }

    if (auto identifier = dynamic_cast<Identifier*>(expr)) {
        return visitIdentifier(identifier);
    } else if (auto literal = dynamic_cast<Literal*>(expr)) {
        return visitLiteral(literal);
    } else if (auto binaryOp = dynamic_cast<BinaryOp*>(expr)) {
        return visitBinaryOp(binaryOp);
    } else if (auto funcCall = dynamic_cast<FunctionCall*>(expr)) {
        // 函数调用：目前仅校验函数是否存在；参数类型暂放宽
        Symbol* sym = symbolTable.lookupSymbol(funcCall->name);
        if (!sym || sym->type != std::string("function")) {
            std::cout << "     ⚠️ 未登记的函数调用: " << funcCall->name << std::endl;
            for (auto& arg : funcCall->arguments) {
                visitExpression(arg.get());
            }
            return "void";
        }
        for (auto& arg : funcCall->arguments) {
            visitExpression(arg.get());
        }
        return "void";
    }

    return "unknown";
}

std::string SemanticAnalyzer::visitIdentifier(Identifier* identifier) {
    Symbol* symbol = symbolTable.lookupSymbol(identifier->name);
    if (!symbol) {
        reportError("未声明的标识符: " + identifier->name, identifier);
        return "error";
    }

    return symbol->type;
}

std::string SemanticAnalyzer::visitLiteral(Literal* literal) {
    return literal->type;
}

std::string SemanticAnalyzer::visitBinaryOp(BinaryOp* binaryOp) {
    std::string leftType = visitExpression(binaryOp->left.get());
    std::string rightType = visitExpression(binaryOp->right.get());

    if (leftType == "error" || rightType == "error") {
        return "error";
    }

    if (binaryOp->operator_ == "+" || binaryOp->operator_ == "-" ||
        binaryOp->operator_ == "*" || binaryOp->operator_ == "/") {
        if (leftType == "int" && rightType == "int") return "int";
        if ((leftType == "float" || leftType == "int") && (rightType == "float" || rightType == "int")) return "float";
        if (leftType == "string" && rightType == "string" && binaryOp->operator_ == "+") return "string";
        reportError("类型不兼容的二元运算: " + leftType + " " + binaryOp->operator_ + " " + rightType, binaryOp);
        return "error";
    }

    if (binaryOp->operator_ == "==" || binaryOp->operator_ == "!=" ||
        binaryOp->operator_ == "<" || binaryOp->operator_ == ">" ||
        binaryOp->operator_ == "<=" || binaryOp->operator_ == ">=") {
        return isTypeCompatible(leftType, rightType) ? "bool" : (reportError("类型不兼容的比较运算: " + leftType + " " + binaryOp->operator_ + " " + rightType, binaryOp), "error");
    }

    return "unknown";
}

std::string SemanticAnalyzer::getExpressionType(Expression* expr) { return visitExpression(expr); }

bool SemanticAnalyzer::isBuiltinType(const std::string& type) {
    return type == "int" || type == "i32" || type == "i64" ||
           type == "float" || type == "f32" || type == "f64" ||
           type == "string" || type == "bool" || type == "char" ||
           type == "void" || type == "auto";
}

bool SemanticAnalyzer::isTypeCompatible(const std::string& expected, const std::string& actual) {
    if (expected == actual) return true;
    if (expected == "auto" || actual == "auto") return true;
    if ((expected == "float" || expected == "f32" || expected == "f64") &&
        (actual == "int" || actual == "i32" || actual == "i64")) return true;
    if ((expected == "int" && actual == "i32") || (expected == "i32" && actual == "int")) return true;
    if ((expected == "float" && actual == "f32") || (expected == "f32" && actual == "float")) return true;
    return false;
}

void SemanticAnalyzer::reportError(const std::string& message, ASTNode* node) {
    SemanticErrorInfo error(message);
    if (node) { error.line = node->line; error.column = node->column; }
    errors.push_back(error); hasErrors = true;
}

// === 循环引用检测骨架 ===
void SemanticAnalyzer::buildTypeGraph(Program* program) {
    // 骨架：等待 AST 提供结构体字段/修饰符后补全
    // 目前仅保证类型顶点记录在 visitStructDecl 中完成
    (void)program;
}

bool SemanticAnalyzer::detectCycle(std::vector<std::string>& pathOut) {
    enum class Color { White, Gray, Black };
    std::unordered_map<std::string, Color> color;
    for (const auto& v : typeGraph.vertices) color[v] = Color::White;

    std::vector<std::string> stack;
    std::function<bool(const std::string&)> dfs = [&](const std::string& u) -> bool {
        color[u] = Color::Gray;
        stack.push_back(u);
        for (const auto& p : typeGraph.adj[u]) {
            const auto& v = p.first; bool strong = p.second;
            if (!strong) continue; // 弱引用不计入环
            if (color[v] == Color::White) {
                if (dfs(v)) return true;
            } else if (color[v] == Color::Gray) {
                // 找到环，从 v 回溯
                std::vector<std::string> cycle;
                cycle.push_back(v);
                for (auto it = stack.rbegin(); it != stack.rend(); ++it) {
                    cycle.push_back(*it);
                    if (*it == v) break;
                }
                std::reverse(cycle.begin(), cycle.end());
                pathOut = cycle; return true;
            }
        }
        stack.pop_back();
        color[u] = Color::Black; return false;
    };

    for (const auto& v : typeGraph.vertices) {
        if (color[v] == Color::White) {
            if (dfs(v)) return true;
        }
    }
    return false;
}

void SemanticAnalyzer::printErrors() {
    std::cout << "\n❌ 语义分析错误列表:" << std::endl;
    for (size_t i = 0; i < errors.size(); ++i) {
        const auto& error = errors[i];
        std::cout << "   " << (i + 1) << ". ";
        if (error.line > 0) { std::cout << "第" << error.line << "行:" << error.column << "列 - "; }
        std::cout << error.message << std::endl;
    }
    std::cout << std::endl;
}