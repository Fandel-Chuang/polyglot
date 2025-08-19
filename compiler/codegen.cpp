#include "codegen.h"
#include <iostream>

std::string CodeGenerator::generate(const std::unique_ptr<Program>& program) {
    std::cout << "   ⚙️ 开始生成C++代码..." << std::endl;

    // 生成C++头部
    output.clear();
    output += "#include <iostream>\n";
    output += "#include <string>\n";
    output += "#include <memory>\n";
    output += "#include <vector>\n\n";

    // 生成程序内容
    for (const auto& stmt : program->statements) {
        generateStatement(stmt.get());
    }

    // 如果没有main函数，生成一个默认的
    if (output.find("int main(") == std::string::npos) {
        output += "\n// 默认主函数\n";
        output += "int main() {\n";
        output += "    std::cout << \"Hello from polyglot!\" << std::endl;\n";
        output += "    return 0;\n";
        output += "}\n";
    }

    std::cout << "   ✅ C++代码生成完成，共 " << countLines(output) << " 行" << std::endl;

    return output;
}

void CodeGenerator::generateStatement(ASTNode* node) {
    if (!node) return;

    // 尝试转换为不同类型的节点
    if (auto funcDecl = dynamic_cast<FunctionDecl*>(node)) {
        generateFunction(funcDecl);
    } else if (auto structDecl = dynamic_cast<StructDecl*>(node)) {
        generateStruct(structDecl);
    } else if (auto implBlock = dynamic_cast<ImplBlock*>(node)) {
        generateImplBlock(implBlock);
    } else if (auto varDecl = dynamic_cast<VariableDecl*>(node)) {
        generateVariableDecl(varDecl);
    } else if (auto returnStmt = dynamic_cast<ReturnStmt*>(node)) {
        generateReturnStmt(returnStmt);
    } else if (auto exprStmt = dynamic_cast<ExpressionStmt*>(node)) {
        generateExpressionStmt(exprStmt);
    } else if (auto block = dynamic_cast<Block*>(node)) {
        generateBlock(block);
    }
}

void CodeGenerator::generateFunction(FunctionDecl* funcDecl) {
    output += "\n// 函数: " + funcDecl->name + "\n";

    // 确定返回类型
    std::string returnType = "void";
    if (funcDecl->returnType) {
        returnType = convertType(funcDecl->returnType->name);
    }

    // 处理main函数特殊情况
    if (funcDecl->name == "main" || funcDecl->name == "主函数") {
        returnType = "int";
    }

    output += returnType + " ";

    // 函数名转换
    std::string funcName = funcDecl->name;
    if (funcName == "主函数") {
        funcName = "main";
    }
    output += funcName + "(";

    // 生成参数列表
    for (size_t i = 0; i < funcDecl->parameters.size(); ++i) {
        if (i > 0) output += ", ";

        auto& param = funcDecl->parameters[i];
        std::string paramType = "auto";
        if (param->type) {
            paramType = convertType(param->type->name);
        }
        output += paramType + " " + param->name;
    }

    output += ") ";

    // 生成函数体
    if (funcDecl->body) {
        generateStatement(funcDecl->body.get());
    } else {
        output += "{\n    // 函数体为空\n}\n";
    }

    output += "\n";
}

void CodeGenerator::generateStruct(StructDecl* structDecl) {
    output += "\n// 结构体: " + structDecl->name + "\n";
    output += "struct " + structDecl->name + " {\n";

    // 生成字段
    for (const auto& field : structDecl->fields) {
        output += "    ";
        if (field->type) {
            output += convertType(field->type->name) + " " + field->name;
        } else {
            output += "auto " + field->name;
        }
        output += ";\n";
    }

    output += "};\n\n";
}

void CodeGenerator::generateImplBlock(ImplBlock* implBlock) {
    output += "\n// 实现块: " + implBlock->structName + "\n";

    // 为结构体生成成员函数
    for (const auto& method : implBlock->methods) {
        // 生成成员函数的前向声明（在结构体外部）
        std::string returnType = "void";
        if (method->returnType) {
            returnType = convertType(method->returnType->name);
        }

        output += returnType + " " + implBlock->structName + "::" + method->name + "(";

        // 参数列表
        for (size_t i = 0; i < method->parameters.size(); ++i) {
            if (i > 0) output += ", ";

            auto& param = method->parameters[i];
            std::string paramType = "auto";
            if (param->type) {
                paramType = convertType(param->type->name);
            }
            output += paramType + " " + param->name;
        }

        output += ") ";

        // 函数体
        if (method->body) {
            generateStatement(method->body.get());
        } else {
            output += "{\n    // 方法体为空\n}\n";
        }

        output += "\n";
    }
}

void CodeGenerator::generateVariableDecl(VariableDecl* varDecl) {
    output += "    ";

    if (varDecl->type) {
        output += convertType(varDecl->type->name) + " " + varDecl->name;
    } else {
        output += "auto " + varDecl->name;
    }

    if (varDecl->initializer) {
        output += " = ";
        generateExpression(varDecl->initializer.get());
    }

    output += ";\n";
}

void CodeGenerator::generateReturnStmt(ReturnStmt* returnStmt) {
    output += "    return";

    if (returnStmt->value) {
        output += " ";
        generateExpression(returnStmt->value.get());
    }

    output += ";\n";
}

void CodeGenerator::generateExpressionStmt(ExpressionStmt* exprStmt) {
    output += "    ";
    generateExpression(exprStmt->expression.get());
    output += ";\n";
}

void CodeGenerator::generateBlock(Block* block) {
    output += "{\n";

    for (const auto& stmt : block->statements) {
        generateStatement(stmt.get());
    }

    output += "}\n";
}

void CodeGenerator::generateExpression(ASTNode* expr) {
    if (!expr) return;

    if (auto identifier = dynamic_cast<Identifier*>(expr)) {
        output += identifier->name;
    } else if (auto literal = dynamic_cast<Literal*>(expr)) {
        generateLiteral(literal);
    } else if (auto binaryOp = dynamic_cast<BinaryOp*>(expr)) {
        generateBinaryOp(binaryOp);
    }
}

void CodeGenerator::generateLiteral(Literal* literal) {
    if (literal->type == "string") {
        output += "\"" + literal->value + "\"";
    } else {
        output += literal->value;
    }
}

void CodeGenerator::generateBinaryOp(BinaryOp* binaryOp) {
    generateExpression(binaryOp->left.get());
    output += " " + binaryOp->operator_ + " ";
    generateExpression(binaryOp->right.get());
}

std::string CodeGenerator::convertType(const std::string& polyglotType) {
    // polyglot类型到C++类型的映射
    if (polyglotType == "整数" || polyglotType == "int" || polyglotType == "i32") {
        return "int";
    } else if (polyglotType == "浮点数" || polyglotType == "float" || polyglotType == "f32") {
        return "float";
    } else if (polyglotType == "双精度" || polyglotType == "double" || polyglotType == "f64") {
        return "double";
    } else if (polyglotType == "字符串" || polyglotType == "string" || polyglotType == "str") {
        return "std::string";
    } else if (polyglotType == "布尔" || polyglotType == "bool" || polyglotType == "boolean") {
        return "bool";
    } else if (polyglotType == "字符" || polyglotType == "char") {
        return "char";
    } else {
        // 默认返回原类型名（可能是用户定义的结构体）
        return polyglotType;
    }
}

int CodeGenerator::countLines(const std::string& code) {
    return std::count(code.begin(), code.end(), '\n');
}
