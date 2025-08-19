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

// Token 类型枚举
enum TokenType {
    // 特殊符号 (polyglot 符号语法)
    IMPORT,           // >>
    VARIABLE,         // ?
    CONDITIONAL_ASSIGN, // :=
    RETURN,           // <-
    STRUCT_DEF,       // @
    IMPL,             // &
    INTERFACE,        // %
    ENUM,             // #
    MATCH,            // #( ... )
    CONDITION,        // ( ... ) ?
    LOOP,             // ^( ... )
    FOR_EACH,         // $( ... )
    SELF_REF,         // _
    CONSTANT,         // *
    BREAK_STMT,       // <<
    CONTINUE_STMT,    // ->

    // 基本数据类型符号
    TYPE_I8,          // i (8位整数)
    TYPE_I16,         // ii (16位整数)
    TYPE_I32,         // I (32位整数)
    TYPE_I64,         // II (64位整数)
    TYPE_F32,         // f (32位浮点)
    TYPE_F64,         // F (64位浮点)
    TYPE_BOOL,        // b (布尔)
    TYPE_STRING,      // s (字符串)
    TYPE_CHAR,        // c (字符)

    // 字面值
    INTEGER_LITERAL,  // 123
    FLOAT_LITERAL,    // 123.45
    STRING_LITERAL,   // "hello"
    CHAR_LITERAL,     // 'a'
    BOOL_TRUE,        // 1
    BOOL_FALSE,       // 0

    // 标识符
    IDENTIFIER,       // 变量名、函数名等

    // 运算符
    PLUS,             // +
    MINUS,            // -
    MULTIPLY,         // *
    DIVIDE,           // /
    MODULO,           // %

    // 比较运算符
    EQUAL,            // ==
    NOT_EQUAL,        // !=
    LESS_THAN,        // <
    GREATER_THAN,     // >
    LESS_EQUAL,       // <=
    GREATER_EQUAL,    // >=

    // 逻辑运算符
    LOGICAL_AND,      // &&
    LOGICAL_OR,       // ||
    LOGICAL_NOT,      // !

    // 赋值运算符
    ASSIGN,           // =
    PLUS_ASSIGN,      // +=
    MINUS_ASSIGN,     // -=

    // 分隔符
    LEFT_PAREN,       // (
    RIGHT_PAREN,      // )
    LEFT_BRACE,       // {
    RIGHT_BRACE,      // }
    LEFT_BRACKET,     // [
    RIGHT_BRACKET,    // ]
    COMMA,            // ,
    SEMICOLON,        // ;
    COLON,            // :
    DOT,              // .

    // 特殊
    NEWLINE,          // \n
    EOF_TOKEN,        // 文件结束
    UNKNOWN           // 未知字符
};

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

public:
    Lexer(const std::string& sourceCode);
    std::vector<Token> tokenize();
    void printTokens() const;
};