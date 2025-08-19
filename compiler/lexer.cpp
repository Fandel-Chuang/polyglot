#include "lexer.h"
#include "error.h"
#include <iostream>
#include <cctype>

Lexer::Lexer(const std::string& sourceCode)
    : source(sourceCode), current(0), line(1), column(1) {
    initSymbolMap();
}

void Lexer::initSymbolMap() {
    // polyglot 符号语法映射
    symbolMap[">>"] = TokenType::IMPORT;
    symbolMap["?"] = TokenType::VARIABLE;
    symbolMap[":="] = TokenType::CONDITIONAL_ASSIGN;
    symbolMap["<-"] = TokenType::RETURN;
    symbolMap["@"] = TokenType::STRUCT_DEF;
    symbolMap["&"] = TokenType::IMPL;
    symbolMap["%"] = TokenType::INTERFACE;
    symbolMap["#"] = TokenType::ENUM;
    symbolMap["^"] = TokenType::LOOP;
    symbolMap["$"] = TokenType::FOR_EACH;
    symbolMap["_"] = TokenType::SELF_REF;
    symbolMap["*"] = TokenType::CONSTANT;
    symbolMap["<<"] = TokenType::BREAK_STMT;
    symbolMap["->"] = TokenType::CONTINUE_STMT;

    // 数据类型符号
    symbolMap["i"] = TokenType::TYPE_I8;
    symbolMap["ii"] = TokenType::TYPE_I16;
    symbolMap["I"] = TokenType::TYPE_I32;
    symbolMap["II"] = TokenType::TYPE_I64;
    symbolMap["f"] = TokenType::TYPE_F32;
    symbolMap["F"] = TokenType::TYPE_F64;
    symbolMap["b"] = TokenType::TYPE_BOOL;
    symbolMap["s"] = TokenType::TYPE_STRING;
    symbolMap["c"] = TokenType::TYPE_CHAR;

    // 布尔值
    symbolMap["1"] = TokenType::BOOL_TRUE;
    symbolMap["0"] = TokenType::BOOL_FALSE;
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

    while (isAlphaNumeric(peek())) {
        text += advance();
    }

    // 检查是否是保留的类型符号
    auto it = symbolMap.find(text);
    if (it != symbolMap.end()) {
        tokens.emplace_back(it->second, text, startLine, startColumn);
    } else {
        tokens.emplace_back(TokenType::IDENTIFIER, text, startLine, startColumn);
    }
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
                tokens.emplace_back(TokenType::RETURN, "<-", startLine, startColumn);
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
                tokens.emplace_back(TokenType::IMPL, "&", startLine, startColumn);
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
        case '$': tokens.emplace_back(TokenType::FOR_EACH, "$", startLine, startColumn); break;
        case '_': tokens.emplace_back(TokenType::SELF_REF, "_", startLine, startColumn); break;
        case '*': tokens.emplace_back(TokenType::CONSTANT, "*", startLine, startColumn); break;
        case '?': tokens.emplace_back(TokenType::VARIABLE, "?", startLine, startColumn); break;
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
    while (current < source.length()) {
        skipWhitespace();

        char c = peek();

        if (c == '\0') break;

        // 跳过注释
        if (c == '/' && (peekNext() == '/' || peekNext() == '*')) {
            skipComment();
            continue;
        }

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
        // 标识符和关键字
        else if (isAlpha(c)) {
            scanIdentifier();
        }
        // 符号
        else {
            scanSymbol();
        }
    }

    // 添加文件结束标记
    tokens.emplace_back(TokenType::EOF_TOKEN, "", line, column);

    return tokens;
}

void Lexer::printTokens() const {
    std::cout << "\n=== Token列表 ===" << std::endl;
    for (const auto& token : tokens) {
        std::cout << "行" << token.line << ":列" << token.column
                  << " - Type: " << static_cast<int>(token.type)
                  << ", Value: '" << token.value << "'" << std::endl;
    }
}
