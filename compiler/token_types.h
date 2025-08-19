#pragma once

// Token类型枚举定义
enum TokenType {
    // 特殊符号 (polyglot 符号语法)
    IMPORT,           // >>
    QUESTION,         // ?
    CONDITIONAL_ASSIGN, // :=
    RETURN_ARROW,     // <-
    STRUCT_DEF,       // @
    IMPL_DEF,         // &
    INTERFACE,        // %
    ENUM,             // #
    ARROW,            // ->
    MATCH,            // #( ... )
    CONDITION,        // ( ... ) ?
    LOOP,             // ^( ... )
    FOR_EACH,         // $( ... )
    SELF_REF,         // _
    CONSTANT,         // *
    BREAK_STMT,       // <<
    CONTINUE_STMT,    // ->
    VARIABLE_PREFIX,  // $ (变量名前缀)

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
    TRUE,             // true
    FALSE,            // false

    // 标识符
    IDENTIFIER,       // 变量名、函数名等

    // 运算符
    PLUS,             // +
    MINUS,            // -
    STAR,             // *
    SLASH,            // /
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