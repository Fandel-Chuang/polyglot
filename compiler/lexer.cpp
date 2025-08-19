#include "lexer.h"
#include "error.h"
#include <iostream>
#include <cctype>
#include <algorithm>

Lexer::Lexer(const std::string& sourceCode)
    : source(sourceCode), current(0), line(1), column(1) {
    initSymbolMap();
}

void Lexer::initSymbolMap() {
    // === polyglot 英文版符号语法映射 ===

    // 核心符号
    symbolMap[">>"] = TokenType::IMPORT;           // 模块导入
    symbolMap["?"] = TokenType::QUESTION;          // 变量声明
    symbolMap[":="] = TokenType::CONDITIONAL_ASSIGN; // 值拷贝赋值
    symbolMap["<-"] = TokenType::RETURN_ARROW;     // 返回语句
    symbolMap["@"] = TokenType::STRUCT_DEF;        // 结构体定义
    symbolMap["&"] = TokenType::IMPL_DEF;          // 方法实现
    symbolMap["->"] = TokenType::ARROW;            // 函数返回类型
    symbolMap["%"] = TokenType::INTERFACE;         // 接口定义
    symbolMap["#"] = TokenType::ENUM;              // 枚举定义
    symbolMap["^"] = TokenType::LOOP;              // 循环语句
    symbolMap["*"] = TokenType::CONSTANT;          // 常量声明

    // 控制流复合符号
    symbolMap["<<"] = TokenType::BREAK_STMT;       // 跳出循环

    // 特殊引用
    symbolMap["_"] = TokenType::SELF_REF;          // 自身引用

    // === 文达 中文版全角符号映射 ===

    // 核心符号（全角版本）
    symbolMap["》》"] = TokenType::IMPORT;          // 模块导入（全角）
    symbolMap["？"] = TokenType::QUESTION;         // 变量声明（全角）
    symbolMap["：＝"] = TokenType::CONDITIONAL_ASSIGN; // 值拷贝赋值（全角）
    symbolMap["＜－"] = TokenType::RETURN_ARROW;    // 返回语句（全角）
    symbolMap["＠"] = TokenType::STRUCT_DEF;       // 结构体定义（全角）
    symbolMap["＆"] = TokenType::IMPL_DEF;         // 方法实现（全角）
    symbolMap["－》"] = TokenType::ARROW;          // 函数返回类型（全角）
    symbolMap["％"] = TokenType::INTERFACE;        // 接口定义（全角）
    symbolMap["＃"] = TokenType::ENUM;             // 枚举定义（全角）
    symbolMap["＾"] = TokenType::LOOP;             // 循环语句（全角）
    symbolMap["＊"] = TokenType::CONSTANT;         // 常量声明（全角）

    // 控制流复合符号（全角版本）
    symbolMap["＜＜"] = TokenType::BREAK_STMT;      // 跳出循环（全角）

    // 特殊引用（全角版本）
    symbolMap["＿"] = TokenType::SELF_REF;         // 自身引用（全角）
    symbolMap["＄"] = TokenType::VARIABLE_PREFIX;  // 变量前缀（全角）

    // 全角基本符号
    symbolMap["＝"] = TokenType::ASSIGN;           // 赋值（全角）
    symbolMap["＋"] = TokenType::PLUS;             // 加号（全角）
    symbolMap["－"] = TokenType::MINUS;            // 减号（全角）
    symbolMap["＊"] = TokenType::MULTIPLY;         // 乘号（全角）
    symbolMap["／"] = TokenType::DIVIDE;           // 除号（全角）
    symbolMap["．"] = TokenType::DOT;              // 点号（全角）

    // 全角复合操作符
    symbolMap["－＝"] = TokenType::MINUS_ASSIGN;   // 减等于（全角）
    symbolMap["＋＝"] = TokenType::PLUS_ASSIGN;    // 加等于（全角）

    // === 基本数据类型映射 ===

    // 英文版类型关键字
    symbolMap["i8"] = TokenType::TYPE_I8;
    symbolMap["i16"] = TokenType::TYPE_I16;
    symbolMap["i32"] = TokenType::TYPE_I32;
    symbolMap["i64"] = TokenType::TYPE_I64;
    symbolMap["f32"] = TokenType::TYPE_F32;
    symbolMap["f64"] = TokenType::TYPE_F64;
    symbolMap["bool"] = TokenType::TYPE_BOOL;
    symbolMap["string"] = TokenType::TYPE_STRING;
    symbolMap["char"] = TokenType::TYPE_CHAR;

    // 中文版类型关键字
    symbolMap["整数"] = TokenType::TYPE_I32;
    symbolMap["长整数"] = TokenType::TYPE_I64;
    symbolMap["浮点数"] = TokenType::TYPE_F32;
    symbolMap["双精度浮点数"] = TokenType::TYPE_F64;
    symbolMap["布尔"] = TokenType::TYPE_BOOL;
    symbolMap["字符串"] = TokenType::TYPE_STRING;
    symbolMap["字符"] = TokenType::TYPE_CHAR;

    // === 布尔值映射 ===
    symbolMap["true"] = TokenType::BOOL_TRUE;
    symbolMap["false"] = TokenType::BOOL_FALSE;
    symbolMap["真"] = TokenType::BOOL_TRUE;          // 中文布尔值
    symbolMap["假"] = TokenType::BOOL_FALSE;         // 中文布尔值
}

char Lexer::peek() {
    if (current >= source.length()) return '\0';
    return source[current];
}

char Lexer::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

char Lexer::advance() {
    if (current >= source.length()) return '\0';

    char c = source[current++];
    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return c;
}

void Lexer::skipWhitespace() {
    while (true) {
        char c = peek();
        if (c == ' ' || c == '\r' || c == '\t') {
            advance();
        } else {
            break;
        }
    }
}

void Lexer::skipComment() {
    // 跳过单行注释 //
    if (peek() == '/' && peekNext() == '/') {
        while (peek() != '\n' && peek() != '\0') {
            advance();
        }
    }

    // 跳过多行注释 /* */
    if (peek() == '/' && peekNext() == '*') {
        advance(); // /
        advance(); // *

        while (true) {
            if (peek() == '\0') {
                throw LexerError("未结束的多行注释", line, column);
            }
            if (peek() == '*' && peekNext() == '/') {
                advance(); // *
                advance(); // /
                break;
            }
            advance();
        }
    }
}

bool Lexer::isAlpha(char c) {
    return std::isalpha(c) || c == '_';
}

bool Lexer::isDigit(char c) {
    return std::isdigit(c);
}

bool Lexer::isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}

void Lexer::scanString() {
    int startLine = line;
    int startColumn = column;

    advance(); // 跳过开始的 "

    std::string value = "";
    while (peek() != '"' && peek() != '\0') {
        if (peek() == '\n') {
            throw LexerError("字符串字面值不能跨行", startLine, startColumn);
        }

        if (peek() == '\\') {
            advance(); // 跳过反斜杠
            char escaped = advance();
            switch (escaped) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case 'r': value += '\r'; break;
                case '\\': value += '\\'; break;
                case '"': value += '"'; break;
                default:
                    throw LexerError("未知的转义序列: \\" + std::string(1, escaped), line, column);
            }
        } else {
            value += advance();
        }
    }

    if (peek() == '\0') {
        throw LexerError("未结束的字符串字面值", startLine, startColumn);
    }

    advance(); // 跳过结束的 "
    tokens.emplace_back(TokenType::STRING_LITERAL, value, startLine, startColumn);
}

void Lexer::scanChar() {
    int startLine = line;
    int startColumn = column;

    advance(); // 跳过开始的 '

    if (peek() == '\0' || peek() == '\n') {
        throw LexerError("未结束的字符字面值", startLine, startColumn);
    }

    char value;
    if (peek() == '\\') {
        advance(); // 跳过反斜杠
        char escaped = advance();
        switch (escaped) {
            case 'n': value = '\n'; break;
            case 't': value = '\t'; break;
            case 'r': value = '\r'; break;
            case '\\': value = '\\'; break;
            case '\'': value = '\''; break;
            default:
                throw LexerError("未知的转义序列: \\" + std::string(1, escaped), line, column);
        }
    } else {
        value = advance();
    }

    if (peek() != '\'') {
        throw LexerError("字符字面值必须以 ' 结束", line, column);
    }

    advance(); // 跳过结束的 '
    tokens.emplace_back(TokenType::CHAR_LITERAL, std::string(1, value), startLine, startColumn);
}

void Lexer::scanNumber() {
    int startLine = line;
    int startColumn = column;
    std::string number = "";

    // 扫描整数部分
    while (isDigit(peek())) {
        number += advance();
    }

    // 检查是否是浮点数
    if (peek() == '.' && isDigit(peekNext())) {
        number += advance(); // .

        while (isDigit(peek())) {
            number += advance();
        }

        tokens.emplace_back(TokenType::FLOAT_LITERAL, number, startLine, startColumn);
    } else {
        tokens.emplace_back(TokenType::INTEGER_LITERAL, number, startLine, startColumn);
    }
}

void Lexer::scanIdentifier() {
    int startLine = line;
    int startColumn = column;
    std::string text = "";

    // 检查是否以$开头（变量前缀）
    bool hasVariablePrefix = false;
    if (peek() == '$') {
        text += advance();
        hasVariablePrefix = true;
    }

    // 扫描标识符的其余部分
    while (isAlphaNumeric(peek())) {
        text += advance();
    }

    // 如果只有$符号，当作变量前缀Token处理
    if (text == "$") {
        tokens.emplace_back(TokenType::VARIABLE_PREFIX, text, startLine, startColumn);
        return;
    }

    // 检查是否是保留的类型符号（不包含$前缀的标识符）
    if (!hasVariablePrefix) {
        auto it = symbolMap.find(text);
        if (it != symbolMap.end()) {
            tokens.emplace_back(it->second, text, startLine, startColumn);
            return;
        }
    }

    // 普通标识符（可能带有$前缀）
    tokens.emplace_back(TokenType::IDENTIFIER, text, startLine, startColumn);
}

void Lexer::scanSymbol() {
    int startLine = line;
    int startColumn = column;
    char c = advance();

    switch (c) {
        // 检查多字符符号
        case '>':
            if (peek() == '>') {
                advance();
                tokens.emplace_back(TokenType::IMPORT, ">>", startLine, startColumn);
            } else if (peek() == '=') {
                advance();
                tokens.emplace_back(TokenType::GREATER_EQUAL, ">=", startLine, startColumn);
            } else {
                tokens.emplace_back(TokenType::GREATER_THAN, ">", startLine, startColumn);
            }
            break;

        case '<':
            if (peek() == '<') {
                advance();
                tokens.emplace_back(TokenType::BREAK_STMT, "<<", startLine, startColumn);
            } else if (peek() == '-') {
                advance();
                tokens.emplace_back(TokenType::RETURN_ARROW, "<-", startLine, startColumn);
            } else if (peek() == '=') {
                advance();
                tokens.emplace_back(TokenType::LESS_EQUAL, "<=", startLine, startColumn);
            } else {
                tokens.emplace_back(TokenType::LESS_THAN, "<", startLine, startColumn);
            }
            break;

        case ':':
            if (peek() == '=') {
                advance();
                tokens.emplace_back(TokenType::CONDITIONAL_ASSIGN, ":=", startLine, startColumn);
            } else {
                tokens.emplace_back(TokenType::COLON, ":", startLine, startColumn);
            }
            break;

        case '-':
            if (peek() == '>') {
                advance();
                tokens.emplace_back(TokenType::CONTINUE_STMT, "->", startLine, startColumn);
            } else if (peek() == '=') {
                advance();
                tokens.emplace_back(TokenType::MINUS_ASSIGN, "-=", startLine, startColumn);
            } else {
                tokens.emplace_back(TokenType::MINUS, "-", startLine, startColumn);
            }
            break;

        case '=':
            if (peek() == '=') {
                advance();
                tokens.emplace_back(TokenType::EQUAL, "==", startLine, startColumn);
            } else {
                tokens.emplace_back(TokenType::ASSIGN, "=", startLine, startColumn);
            }
            break;

        case '!':
            if (peek() == '=') {
                advance();
                tokens.emplace_back(TokenType::NOT_EQUAL, "!=", startLine, startColumn);
            } else {
                tokens.emplace_back(TokenType::LOGICAL_NOT, "!", startLine, startColumn);
            }
            break;

        case '&':
            if (peek() == '&') {
                advance();
                tokens.emplace_back(TokenType::LOGICAL_AND, "&&", startLine, startColumn);
            } else {
                tokens.emplace_back(TokenType::IMPL_DEF, "&", startLine, startColumn);
            }
            break;

        case '|':
            if (peek() == '|') {
                advance();
                tokens.emplace_back(TokenType::LOGICAL_OR, "||", startLine, startColumn);
            } else {
                throw LexerError("未知符号: " + std::string(1, c), startLine, startColumn);
            }
            break;

        case '+':
            if (peek() == '=') {
                advance();
                tokens.emplace_back(TokenType::PLUS_ASSIGN, "+=", startLine, startColumn);
            } else {
                tokens.emplace_back(TokenType::PLUS, "+", startLine, startColumn);
            }
            break;

        // 单字符符号
        case '@': tokens.emplace_back(TokenType::STRUCT_DEF, "@", startLine, startColumn); break;
        case '%': tokens.emplace_back(TokenType::INTERFACE, "%", startLine, startColumn); break;
        case '#': tokens.emplace_back(TokenType::ENUM, "#", startLine, startColumn); break;
        case '^': tokens.emplace_back(TokenType::LOOP, "^", startLine, startColumn); break;
        case '$': tokens.emplace_back(TokenType::VARIABLE_PREFIX, "$", startLine, startColumn); break;
        case '_': tokens.emplace_back(TokenType::SELF_REF, "_", startLine, startColumn); break;
        case '*': tokens.emplace_back(TokenType::CONSTANT, "*", startLine, startColumn); break;
        case '?': tokens.emplace_back(TokenType::QUESTION, "?", startLine, startColumn); break;
        case '/': tokens.emplace_back(TokenType::DIVIDE, "/", startLine, startColumn); break;
        case '(': tokens.emplace_back(TokenType::LEFT_PAREN, "(", startLine, startColumn); break;
        case ')': tokens.emplace_back(TokenType::RIGHT_PAREN, ")", startLine, startColumn); break;
        case '{': tokens.emplace_back(TokenType::LEFT_BRACE, "{", startLine, startColumn); break;
        case '}': tokens.emplace_back(TokenType::RIGHT_BRACE, "}", startLine, startColumn); break;
        case '[': tokens.emplace_back(TokenType::LEFT_BRACKET, "[", startLine, startColumn); break;
        case ']': tokens.emplace_back(TokenType::RIGHT_BRACKET, "]", startLine, startColumn); break;
        case ',': tokens.emplace_back(TokenType::COMMA, ",", startLine, startColumn); break;
        case ';': tokens.emplace_back(TokenType::SEMICOLON, ";", startLine, startColumn); break;
        case '.': tokens.emplace_back(TokenType::DOT, ".", startLine, startColumn); break;
        case '\n': tokens.emplace_back(TokenType::NEWLINE, "\\n", startLine, startColumn); break;

        default:
            throw LexerError("未知符号: " + std::string(1, c), startLine, startColumn);
    }
}

std::vector<Token> Lexer::tokenize() {
    // 首先验证符号一致性
    validateSymbolConsistency();

    while (current < source.length()) {
        skipWhitespace();

        // 检查是否到达文件末尾
        if (current >= source.length()) break;

        // 先尝试读取UTF-8字符
        std::string nextChar = peekUTF8Char();
        if (nextChar.empty()) break;

        // 跳过注释 (只检查ASCII注释)
        if (nextChar.length() == 1) {
            char c = nextChar[0];
            if ((c == '/' && peekNext() == '/') || (c == '/' && peekNext() == '*')) {
                skipComment();
                continue;
            }
        }

        // 处理Unicode字符（中文字符或全角符号）
        if (nextChar.length() > 1) {
            if (isChineseChar(nextChar) || nextChar == "＄") {
                // 中文标识符（包括以＄开头的变量）
                scanChineseIdentifier();
            } else if (isFullWidthSymbol(nextChar) || isUnicodeSymbol(nextChar)) {
                // 全角符号
                scanUnicodeSymbol();
            } else {
                // 其他Unicode字符（目前不支持）
                throw LexerError("不支持的Unicode字符: " + nextChar, line, column);
            }
        }
        // 处理ASCII字符
        else {
            char c = nextChar[0];

            // 字符串字面值
            if (c == '"') {
                scanString();
            }
            // 字符字面值
            else if (c == '\'') {
                scanChar();
            }
            // 数字字面值
            else if (isDigit(c)) {
                scanNumber();
            }
            // 标识符和关键字（包括$开头的变量）
            else if (isAlpha(c) || c == '$') {
                scanIdentifier();
            }
            // 换行符处理
            else if (c == '\n') {
                advance();
                tokens.emplace_back(TokenType::NEWLINE, "\\n", line-1, column);
            }
            // ASCII符号
            else {
                scanSymbol();
            }
        }
    }

    // 添加文件结束标记
    tokens.emplace_back(TokenType::EOF_TOKEN, "", line, column);

    return tokens;
}

// === Unicode和全角符号处理函数实现 ===

int Lexer::getUTF8CharLength(char firstByte) {
    if ((firstByte & 0x80) == 0) return 1;  // ASCII
    if ((firstByte & 0xE0) == 0xC0) return 2;  // 110xxxxx
    if ((firstByte & 0xF0) == 0xE0) return 3;  // 1110xxxx
    if ((firstByte & 0xF8) == 0xF0) return 4;  // 11110xxx
    return 1; // 错误情况，当作单字节处理
}

std::string Lexer::peekUTF8Char() {
    if (current >= source.length()) return "";

    int charLength = getUTF8CharLength(source[current]);
    if (current + charLength > source.length()) return "";

    return source.substr(current, charLength);
}

std::string Lexer::advanceUTF8Char() {
    if (current >= source.length()) return "";

    int charLength = getUTF8CharLength(source[current]);
    if (current + charLength > source.length()) return "";

    std::string utf8Char = source.substr(current, charLength);
    current += charLength;

    // 处理换行符
    if (utf8Char == "\n") {
        line++;
        column = 1;
    } else {
        column++;
    }

    return utf8Char;
}

bool Lexer::isChineseChar(const std::string& str) {
    if (str.length() != 3) return false; // 中文字符通常是3字节UTF-8

    unsigned char first = str[0];
    // 中文字符的UTF-8编码范围：0xE4-0xE9 (CJK统一汉字)
    return (first >= 0xE4 && first <= 0xE9);
}

bool Lexer::isFullWidthSymbol(const std::string& str) {
    // 检查是否是全角符号（通常在全角标点符号区域）
    if (str.length() != 3) return false;

    unsigned char first = str[0];
    unsigned char second = str[1];

    // 全角符号的UTF-8编码范围
    // 0xEF 0xBC 0x80 - 0xEF 0xBF 0xBF (全角ASCII和标点符号)
    if (first == 0xEF) {
        return (second >= 0xBC && second <= 0xBF);
    }

    // 0xE3 0x80 0x80 - 0xE3 0x80 0xBF (部分中文标点符号)
    if (first == 0xE3 && second == 0x80) {
        return true;
    }

    return false;
}

bool Lexer::isUnicodeSymbol(const std::string& str) {
    return isFullWidthSymbol(str) || isChineseChar(str);
}

void Lexer::scanChineseIdentifier() {
    int startLine = line;
    int startColumn = column;
    std::string identifier = "";

    // 检查是否以全角$开头（变量前缀）
    bool hasVariablePrefix = false;
    std::string firstChar = peekUTF8Char();
    if (firstChar == "＄") {
        identifier += advanceUTF8Char();
        hasVariablePrefix = true;
    }

    // 扫描中文标识符（可以包含中文字符、字母、数字）
    while (true) {
        std::string nextChar = peekUTF8Char();
        if (nextChar.empty()) break;

        // 中文字符
        if (isChineseChar(nextChar)) {
            identifier += advanceUTF8Char();
        }
        // 英文字母或数字
        else if (nextChar.length() == 1 && isAlphaNumeric(nextChar[0])) {
            identifier += advance();
        }
        else {
            break;
        }
    }

    // 如果只有＄符号，当作变量前缀Token处理
    if (identifier == "＄") {
        tokens.emplace_back(TokenType::VARIABLE_PREFIX, identifier, startLine, startColumn);
        return;
    }

    // 检查是否是中文关键字（不包含$前缀的标识符）
    if (!hasVariablePrefix) {
        auto it = symbolMap.find(identifier);
        if (it != symbolMap.end()) {
            tokens.emplace_back(it->second, identifier, startLine, startColumn);
            return;
        }
    }

    // 普通标识符（可能带有＄前缀）
    tokens.emplace_back(TokenType::IDENTIFIER, identifier, startLine, startColumn);
}

void Lexer::scanUnicodeSymbol() {
    int startLine = line;
    int startColumn = column;

    // 尝试匹配最长的全角符号序列
    std::string longestMatch = "";
    TokenType matchedType = TokenType::UNKNOWN;

    // 从长到短匹配符号（确保先匹配较长的符号如"》》"）
    for (int maxLength = 6; maxLength >= 3; maxLength -= 3) { // 每个全角字符3字节
        if (current + maxLength > source.length()) continue;

        std::string candidate = source.substr(current, maxLength);
        auto it = symbolMap.find(candidate);
        if (it != symbolMap.end()) {
            longestMatch = candidate;
            matchedType = it->second;
            break;
        }
    }

    if (!longestMatch.empty()) {
        // 找到匹配的符号
        for (int i = 0; i < longestMatch.length(); i += 3) {
            advanceUTF8Char();
        }
        tokens.emplace_back(matchedType, longestMatch, startLine, startColumn);
    } else {
        // 没有找到匹配的符号，按单个UTF-8字符处理
        std::string singleChar = advanceUTF8Char();

        // 处理全角标点符号
        if (singleChar == "（") {
            tokens.emplace_back(TokenType::LEFT_PAREN, singleChar, startLine, startColumn);
        } else if (singleChar == "）") {
            tokens.emplace_back(TokenType::RIGHT_PAREN, singleChar, startLine, startColumn);
        } else if (singleChar == "｛") {
            tokens.emplace_back(TokenType::LEFT_BRACE, singleChar, startLine, startColumn);
        } else if (singleChar == "｝") {
            tokens.emplace_back(TokenType::RIGHT_BRACE, singleChar, startLine, startColumn);
        } else if (singleChar == "，") {
            tokens.emplace_back(TokenType::COMMA, singleChar, startLine, startColumn);
        } else if (singleChar == "；") {
            tokens.emplace_back(TokenType::SEMICOLON, singleChar, startLine, startColumn);
        } else if (singleChar == "：") {
            tokens.emplace_back(TokenType::COLON, singleChar, startLine, startColumn);
        } else if (singleChar == "。") {
            tokens.emplace_back(TokenType::DOT, singleChar, startLine, startColumn);
        } else {
            throw LexerError("未知的Unicode字符: " + singleChar, startLine, startColumn);
        }
    }
}

Lexer::SymbolMode Lexer::detectSymbolMode() {
    bool hasHalfWidth = false;
    bool hasFullWidth = false;

    // 扫描整个源代码，检测符号类型
    for (const auto& pair : symbolMap) {
        if (source.find(pair.first) != std::string::npos) {
            if (pair.first.length() == 1 ||
                (pair.first.length() == 2 && pair.first[0] != 0xEF)) {
                hasHalfWidth = true;
            } else {
                hasFullWidth = true;
            }
        }
    }

    if (hasHalfWidth && hasFullWidth) return MIXED;
    if (hasFullWidth) return FULL_WIDTH;
    return HALF_WIDTH;
}

void Lexer::validateSymbolConsistency() {
    SymbolMode mode = detectSymbolMode();

    if (mode == MIXED) {
        std::cout << "⚠️  警告：检测到混合符号模式！" << std::endl;
        std::cout << "建议：在同一个文件中统一使用半角符号或全角符号" << std::endl;
        std::cout << "半角符号示例：>>、?、@、&" << std::endl;
        std::cout << "全角符号示例：》》、？、＠、＆" << std::endl;
    } else if (mode == FULL_WIDTH) {
        std::cout << "✅ 检测到文达中文编程模式（全角符号）" << std::endl;
    } else {
        std::cout << "✅ 检测到polyglot英文编程模式（半角符号）" << std::endl;
    }
}

void Lexer::printTokens() const {
    std::cout << "\n=== Token列表 ===" << std::endl;
    for (const auto& token : tokens) {
        std::cout << "行" << token.line << ":列" << token.column
                  << " - Type: " << static_cast<int>(token.type)
                  << ", Value: '" << token.value << "'" << std::endl;
    }
}
