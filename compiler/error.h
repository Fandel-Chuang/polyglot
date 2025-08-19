#pragma once

#include <string>
#include <exception>

// polyglot 编译器错误类
class CompilerError : public std::exception {
private:
    std::string message;
    int line;
    int column;

public:
    // 构造函数
    CompilerError(const std::string& msg)
        : message(msg), line(-1), column(-1) {}

    CompilerError(const std::string& msg, int line_num, int col_num)
        : message(msg), line(line_num), column(col_num) {}

    // 获取错误信息
    const char* what() const noexcept override {
        return message.c_str();
    }

    // 获取完整错误信息（包含行列号）
    std::string getFullMessage() const {
        if (line >= 0 && column >= 0) {
            return "行" + std::to_string(line) + ":列" + std::to_string(column) + " - " + message;
        }
        return message;
    }

    int getLine() const { return line; }
    int getColumn() const { return column; }
};

// 具体的错误类型
class LexerError : public CompilerError {
public:
    LexerError(const std::string& msg, int line, int column)
        : CompilerError("词法错误: " + msg, line, column) {}
};

class ParserError : public CompilerError {
public:
    ParserError(const std::string& msg, int line, int column)
        : CompilerError("语法错误: " + msg, line, column) {}
};

class SemanticError : public CompilerError {
public:
    SemanticError(const std::string& msg, int line, int column)
        : CompilerError("语义错误: " + msg, line, column) {}
};

class CodeGenError : public CompilerError {
public:
    CodeGenError(const std::string& msg)
        : CompilerError("代码生成错误: " + msg) {}
};