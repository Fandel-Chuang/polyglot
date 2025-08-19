#pragma once

// 防止Windows宏定义冲突
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
// 取消可能冲突的宏定义
#ifdef CONST
#undef CONST
#endif
#ifdef BREAK
#undef BREAK
#endif
#ifdef CONTINUE
#undef CONTINUE
#endif
#ifdef INTERFACE
#undef INTERFACE
#endif
#ifdef ENUM
#undef ENUM
#endif
#endif

#include <string>
#include <vector>
#include <unordered_map>
#include "token_types.h"

// Token 结构体
struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;

    Token(TokenType t, const std::string& v, int l, int c)
        : type(t), value(v), line(l), column(c) {}
};

// 词法分析器类
class Lexer {
private:
    std::string source;
    size_t current;
    int line;
    int column;
    std::vector<Token> tokens;

    // 符号映射表
    std::unordered_map<std::string, TokenType> symbolMap;

    void initSymbolMap();
    char peek();
    char peekNext();
    char advance();
    void skipWhitespace();
    void skipComment();
    bool isAlpha(char c);
    bool isDigit(char c);
    bool isAlphaNumeric(char c);

    void scanString();
    void scanChar();
    void scanNumber();
    void scanIdentifier();
    void scanSymbol();

    // Unicode和全角符号处理函数
    std::string peekUTF8Char();
    std::string advanceUTF8Char();
    bool isUnicodeSymbol(const std::string& str);
    bool isChineseChar(const std::string& str);
    bool isFullWidthSymbol(const std::string& str);
    void scanUnicodeSymbol();
    void scanChineseIdentifier();
    int getUTF8CharLength(char firstByte);

    // 符号模式检测
    enum SymbolMode { HALF_WIDTH, FULL_WIDTH, MIXED };
    SymbolMode detectSymbolMode();
    void validateSymbolConsistency();

public:
    Lexer(const std::string& sourceCode);
    std::vector<Token> tokenize();
    void printTokens() const;
};