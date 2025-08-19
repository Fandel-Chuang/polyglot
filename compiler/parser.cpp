#include "parser.h"
#include "error.h"
#include <iostream>
#include <algorithm>
#include <unordered_set>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0) {
    std::cout << "🚀 Parser初始化完成，准备解析 " << tokens.size() << " 个Token" << std::endl;
}

Token& Parser::peek() {
    if (current >= tokens.size()) {
        static Token eofToken = {TokenType::EOF_TOKEN, "", 0, 0};
        return eofToken;
    }
    return tokens[current];
}

Token& Parser::advance() {
    if (!isAtEnd()) current++;
    return tokens[current - 1];
}

bool Parser::isAtEnd() {
    return current >= tokens.size() || peek().type == TokenType::EOF_TOKEN;
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

    std::cout << "   📋 开始解析polyglot代码..." << std::endl;
    std::cout << "   📋 Token数量: " << tokens.size() << std::endl;

    try {
        while (!isAtEnd()) {
            if (auto stmt = parseTopLevelStatement()) {
                program->statements.push_back(std::move(stmt));
            }
        }

        std::cout << "   ✅ 解析完成，生成了 " << program->statements.size() << " 个顶级语句" << std::endl;
    } catch (const ParserError& e) {
        std::cout << "   ❌ 解析错误: " << e.what() << std::endl;
        throw;
    }

    return program;
}

// 解析顶级语句（模块导入、函数定义、结构体定义等）
std::unique_ptr<ASTNode> Parser::parseTopLevelStatement() {
    // 跳过换行符和空白符
    while (peek().type == TokenType::NEWLINE && !isAtEnd()) {
        advance();
    }

    if (isAtEnd()) {
        return nullptr;
    }

    Token& current = peek();

    switch (current.type) {
        case TokenType::IMPORT:        // >>导入
            return parseImport();
        case TokenType::STRUCT_DEF:    // @结构体定义
            return parseStructDef();
        case TokenType::IMPL_DEF:      // &实现块
            return parseImplBlock();
        case TokenType::IDENTIFIER:    // 函数定义
            return parseFunctionDef();
        default:
            // 跳过未知token
            advance();
            return nullptr;
    }
}

// 解析导入语句: >> "module_name"
std::unique_ptr<ASTNode> Parser::parseImport() {
    advance(); // 跳过 >>

    if (peek().type != TokenType::STRING_LITERAL) {
        throw ParserError("期望字符串字面值", peek().line, peek().column);
    }

    std::string moduleName = advance().value;

    // 移除引号
    if (moduleName.length() >= 2 && moduleName[0] == '"' && moduleName.back() == '"') {
        moduleName = moduleName.substr(1, moduleName.length() - 2);
    }

    std::cout << "   📦 解析导入模块: " << moduleName << std::endl;

    return std::make_unique<ImportDecl>(moduleName);
}

// 解析结构体定义: @ StructName { field1: type, field2: type }
std::unique_ptr<StructDecl> Parser::parseStructDef() {
    advance(); // 跳过 @

    if (peek().type != TokenType::IDENTIFIER) {
        throw ParserError("期望结构体名称", peek().line, peek().column);
    }

    auto structDecl = std::make_unique<StructDecl>();
    structDecl->name = advance().value;

    consume(TokenType::LEFT_BRACE, "期望 '{'");

    // 解析字段
    while (peek().type != TokenType::RIGHT_BRACE && !isAtEnd()) {
        auto field = parseVariableDecl();
        if (field) {
            structDecl->fields.push_back(std::move(field));
        }

        if (peek().type == TokenType::COMMA) {
            advance(); // 跳过逗号
        }
    }

    consume(TokenType::RIGHT_BRACE, "期望 '}'");

    return structDecl;
}

// 解析变量声明: name: type 或 ? name = value
std::unique_ptr<VariableDecl> Parser::parseVariableDecl() {
    auto varDecl = std::make_unique<VariableDecl>();

    if (peek().type == TokenType::QUESTION) {
        // ? variable = value 形式
        advance(); // 跳过 ?

        if (peek().type != TokenType::IDENTIFIER) {
            throw ParserError("期望变量名", peek().line, peek().column);
        }

        varDecl->name = advance().value;

        if (peek().type == TokenType::COLON) {
            // 显式类型: ? name: type = value
            advance(); // 跳过 :
            if (peek().type == TokenType::IDENTIFIER) {
                varDecl->type = std::make_unique<TypeNode>(advance().value);
            }
        }

        if (peek().type == TokenType::ASSIGN) {
            advance(); // 跳过 =
            varDecl->initializer = parseExpression();
        }
    } else if (peek().type == TokenType::IDENTIFIER) {
        // name: type 形式（结构体字段）
        varDecl->name = advance().value;

        consume(TokenType::COLON, "期望 ':'");

        if (peek().type == TokenType::IDENTIFIER) {
            varDecl->type = std::make_unique<TypeNode>(advance().value);
        }
    }

    return varDecl;
}

// 解析函数定义: function_name(param1: type, param2: type) { body }
std::unique_ptr<FunctionDecl> Parser::parseFunctionDef() {
    auto funcDecl = std::make_unique<FunctionDecl>();

    if (peek().type != TokenType::IDENTIFIER) {
        throw ParserError("期望函数名", peek().line, peek().column);
    }

    funcDecl->name = advance().value;
    std::cout << "   🔧 解析函数定义: " << funcDecl->name << std::endl;

    consume(TokenType::LEFT_PAREN, "期望 '('");

    // 解析参数列表
    while (peek().type != TokenType::RIGHT_PAREN && !isAtEnd()) {
        auto param = parseVariableDecl();
        if (param) {
            funcDecl->parameters.push_back(std::move(param));
        }

        if (peek().type == TokenType::COMMA) {
            advance(); // 跳过逗号
        }
    }

    consume(TokenType::RIGHT_PAREN, "期望 ')'");

    // 解析返回类型（可选）
    if (peek().type == TokenType::ARROW) {
        advance(); // 跳过 ->
        if (peek().type == TokenType::IDENTIFIER) {
            funcDecl->returnType = std::make_unique<TypeNode>(advance().value);
        }
    }

    // 解析函数体
    if (peek().type == TokenType::LEFT_BRACE) {
        funcDecl->body = parseBlock();
    }

    return funcDecl;
}

// 解析实现块: & StructName { methods... }
std::unique_ptr<ImplBlock> Parser::parseImplBlock() {
    advance(); // 跳过 &

    if (peek().type != TokenType::IDENTIFIER) {
        throw ParserError("期望结构体名称", peek().line, peek().column);
    }

    auto implBlock = std::make_unique<ImplBlock>();
    implBlock->structName = advance().value;

    consume(TokenType::LEFT_BRACE, "期望 '{'");

    // 解析方法
    while (peek().type != TokenType::RIGHT_BRACE && !isAtEnd()) {
        if (peek().type == TokenType::IDENTIFIER) {
            auto method = parseFunctionDef();
            if (method) {
                implBlock->methods.push_back(std::move(method));
            }
        } else {
            advance(); // 跳过未知token
        }
    }

    consume(TokenType::RIGHT_BRACE, "期望 '}'");

    return implBlock;
}

// 解析代码块: { statements... }
std::unique_ptr<Block> Parser::parseBlock() {
    auto block = std::make_unique<Block>();

    consume(TokenType::LEFT_BRACE, "期望 '{'");

    while (peek().type != TokenType::RIGHT_BRACE && !isAtEnd()) {
        if (auto stmt = parseStatement()) {
            block->statements.push_back(std::move(stmt));
        }
    }

    consume(TokenType::RIGHT_BRACE, "期望 '}'");

    return block;
}

// 解析语句
std::unique_ptr<Statement> Parser::parseStatement() {
    // 跳过换行符
    while (peek().type == TokenType::NEWLINE && !isAtEnd()) {
        advance();
    }

    if (isAtEnd()) {
        return nullptr;
    }

    Token& current = peek();
    std::cout << "   🔄 parseStatement: Token类型=" << static_cast<int>(current.type)
              << ", 值='" << current.value << "'" << std::endl;

    switch (current.type) {
        case TokenType::QUESTION:      // ? 变量声明
            return parseVariableDeclStmt();
        case TokenType::RETURN_ARROW:  // <- 返回语句
            return parseReturnStmt();
        case TokenType::LEFT_BRACE:    // { 代码块
            return parseBlock();
        case TokenType::RIGHT_BRACE:   // } 代码块结束
            return nullptr;  // 不消费token，让调用者处理
        default:
            // 对于不能处理的token，跳过以避免死循环
            advance();
            std::cout << "   ⚠️  跳过未识别的token: " << current.value << std::endl;
            return nullptr;
    }
}

// 解析变量声明语句
std::unique_ptr<Statement> Parser::parseVariableDeclStmt() {
    auto varDecl = parseVariableDecl();
    return std::move(varDecl);  // VariableDecl现在继承自Statement，可以直接move
}

// 解析返回语句: <- expression
std::unique_ptr<ReturnStmt> Parser::parseReturnStmt() {
    auto returnStmt = std::make_unique<ReturnStmt>();

    advance(); // 跳过 <-

    if (peek().type != TokenType::RIGHT_BRACE && !isAtEnd()) {
        returnStmt->value = parseExpression();
    }

    return returnStmt;
}

// 解析表达式语句
std::unique_ptr<ExpressionStmt> Parser::parseExpressionStmt() {
    auto exprStmt = std::make_unique<ExpressionStmt>();
    exprStmt->expression = parseExpression();
    return exprStmt;
}

// 解析表达式
std::unique_ptr<Expression> Parser::parseExpression() {
    return parseAssignmentExpression();
}

// 解析赋值表达式
std::unique_ptr<Expression> Parser::parseAssignmentExpression() {
    auto expr = parseLogicalOrExpression();

    if (peek().type == TokenType::ASSIGN) {
        advance(); // 跳过 =
        auto right = parseAssignmentExpression();

        auto binaryOp = std::make_unique<BinaryOp>();
        binaryOp->left = std::move(expr);
        binaryOp->operator_ = "=";
        binaryOp->right = std::move(right);

        return binaryOp;
    }

    return expr;
}

// 解析逻辑或表达式
std::unique_ptr<Expression> Parser::parseLogicalOrExpression() {
    return parseLogicalAndExpression(); // 简化实现
}

// 解析逻辑与表达式
std::unique_ptr<Expression> Parser::parseLogicalAndExpression() {
    return parseEqualityExpression(); // 简化实现
}

// 解析相等性表达式
std::unique_ptr<Expression> Parser::parseEqualityExpression() {
    return parseRelationalExpression(); // 简化实现
}

// 解析关系表达式
std::unique_ptr<Expression> Parser::parseRelationalExpression() {
    return parseAdditiveExpression(); // 简化实现
}

// 解析加减表达式
std::unique_ptr<Expression> Parser::parseAdditiveExpression() {
    auto expr = parseMultiplicativeExpression();

    while (peek().type == TokenType::PLUS || peek().type == TokenType::MINUS) {
        std::string op = advance().value;
        auto right = parseMultiplicativeExpression();

        auto binaryOp = std::make_unique<BinaryOp>();
        binaryOp->left = std::move(expr);
        binaryOp->operator_ = op;
        binaryOp->right = std::move(right);

        expr = std::move(binaryOp);
    }

    return expr;
}

// 解析乘除表达式
std::unique_ptr<Expression> Parser::parseMultiplicativeExpression() {
    auto expr = parseUnaryExpression();

    while (peek().type == TokenType::STAR || peek().type == TokenType::SLASH) {
        std::string op = advance().value;
        auto right = parseUnaryExpression();

        auto binaryOp = std::make_unique<BinaryOp>();
        binaryOp->left = std::move(expr);
        binaryOp->operator_ = op;
        binaryOp->right = std::move(right);

        expr = std::move(binaryOp);
    }

    return expr;
}

// 解析一元表达式
std::unique_ptr<Expression> Parser::parseUnaryExpression() {
    return parsePrimaryExpression(); // 简化实现
}

// 解析基础表达式
std::unique_ptr<Expression> Parser::parsePrimaryExpression() {
    Token& current = peek();

    switch (current.type) {
        case TokenType::IDENTIFIER:
            return std::make_unique<Identifier>(advance().value);

        case TokenType::INTEGER_LITERAL:
            return std::make_unique<Literal>(advance().value, "int");

        case TokenType::FLOAT_LITERAL:
            return std::make_unique<Literal>(advance().value, "float");

        case TokenType::STRING_LITERAL:
            return std::make_unique<Literal>(advance().value, "string");

        case TokenType::TRUE:
        case TokenType::FALSE:
            return std::make_unique<Literal>(advance().value, "bool");

        case TokenType::LEFT_PAREN: {
            advance(); // 跳过 (
            auto expr = parseExpression();
            consume(TokenType::RIGHT_PAREN, "期望 ')'");
            return expr;
        }

        default:
            throw ParserError("期望表达式", current.line, current.column);
    }
}
