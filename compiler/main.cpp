// 先包含我们自己的头文件
#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "codegen.h"
#include "error.h"

// 然后包含标准库
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>

// 最后包含系统特定头文件
#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#else
#include <clocale>
#endif

void setupConsoleUTF8() {
#ifdef _WIN32
    // Windows: 设置控制台代码页为UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#else
    // Linux/macOS: 设置locale为UTF-8
    std::setlocale(LC_ALL, "");
    std::setlocale(LC_CTYPE, "C.UTF-8");
#endif
}

void printUsage() {
    std::cout << "用法: polyglot <源文件.pg>" << std::endl;
}

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw CompilerError("无法打开文件: " + filename);
    }

    std::string content;
    std::string line;
    while (std::getline(file, line)) {
        content += line + "\n";
    }

    return content;
}

// 编译生成的C++代码
bool compileCppCode(const std::string& cppCode, const std::string& originalFilename) {
    // 生成输出文件名
    std::string baseName = originalFilename.substr(0, originalFilename.find_last_of('.'));
    std::string cppFilename = baseName + "_generated.cpp";
    std::string exeFilename = baseName;

#ifdef _WIN32
    exeFilename += ".exe";
#endif

    try {
        // 1. 写入C++源文件
        std::cout << "📝 步骤 5: 写入C++源文件..." << std::endl;
        std::ofstream cppFile(cppFilename);
        if (!cppFile.is_open()) {
            std::cerr << "❌ 无法创建C++源文件: " << cppFilename << std::endl;
            return false;
        }
        cppFile << cppCode;
        cppFile.close();
        std::cout << "   ✅ 已写入: " << cppFilename << std::endl;

        // 2. 编译C++代码
        std::cout << "🔨 步骤 6: 编译C++代码..." << std::endl;

        std::string compileCmd;
#ifdef _WIN32
        // Windows: 尝试g++，如果失败则尝试cl
        compileCmd = "g++ -std=c++17 -O2 \"" + cppFilename + "\" -o \"" + exeFilename + "\" 2>nul";
        int result = system(compileCmd.c_str());

        if (result != 0) {
            // 尝试MSVC
            compileCmd = "cl /std:c++17 /EHsc /O2 \"" + cppFilename + "\" /Fe\"" + exeFilename + "\" >nul 2>&1";
            result = system(compileCmd.c_str());
        }
#else
        // Linux/macOS
        compileCmd = "g++ -std=c++17 -O2 \"" + cppFilename + "\" -o \"" + exeFilename + "\" 2>/dev/null";
        int result = system(compileCmd.c_str());

        if (result != 0) {
            // 尝试clang++
            compileCmd = "clang++ -std=c++17 -O2 \"" + cppFilename + "\" -o \"" + exeFilename + "\" 2>/dev/null";
            result = system(compileCmd.c_str());
        }
#endif

        if (result == 0) {
            std::cout << "   ✅ C++编译成功!" << std::endl;
            std::cout << "   🎯 生成可执行文件: " << exeFilename << std::endl;

            // 3. 运行生成的程序
            std::cout << "🚀 步骤 7: 运行生成的程序..." << std::endl;
            std::cout << "--- 程序输出 ---" << std::endl;

#ifdef _WIN32
            std::string runCmd = "\"" + exeFilename + "\"";
#else
            std::string runCmd = "./" + exeFilename;
#endif
            int runResult = system(runCmd.c_str());

            std::cout << "--- 程序结束 ---" << std::endl;
            if (runResult == 0) {
                std::cout << "✅ 程序执行成功!" << std::endl;
            } else {
                std::cout << "⚠️  程序执行完成 (退出码: " << runResult << ")" << std::endl;
            }

            return true;
        } else {
            std::cerr << "❌ C++编译失败" << std::endl;
            std::cerr << "   尝试的编译命令: " << compileCmd << std::endl;
            std::cerr << "   请检查是否安装了C++编译器 (g++, clang++, 或 MSVC)" << std::endl;
            return false;
        }

    } catch (const std::exception& e) {
        std::cerr << "❌ C++编译过程出错: " << e.what() << std::endl;
        return false;
    }
}

void compile(const std::string& sourceCode, const std::string& filename) {
    std::cout << "🚀 开始编译 polyglot 程序: " << filename << std::endl;

    try {
        // 1. 词法分析 (Lexical Analysis)
        std::cout << "📝 步骤 1: 词法分析..." << std::endl;
        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();
        std::cout << "   🔤 词法分析完成" << std::endl;

        // 调试：仅打印前几个Token
        std::cout << "   🔍 前5个Token:" << std::endl;
        size_t maxTokens = tokens.size() < 5 ? tokens.size() : 5;
        for (size_t i = 0; i < maxTokens; ++i) {
            std::cout << "     [" << i << "] 类型=" << static_cast<int>(tokens[i].type)
                      << ", 值='" << tokens[i].value << "'" << std::endl;
        }

        // 2. 语法分析 (Syntax Analysis)
        std::cout << "🔍 步骤 2: 语法分析..." << std::endl;
        Parser parser(tokens);
        auto ast = parser.parse();
        std::cout << "   ✅ 生成了抽象语法树" << std::endl;

        // 3. 语义分析 (Semantic Analysis)
        std::cout << "🧠 步骤 3: 语义分析..." << std::endl;
        SemanticAnalyzer semanticAnalyzer;
        bool semanticSuccess = semanticAnalyzer.analyze(ast);
        if (!semanticSuccess) {
            std::cerr << "❌ 语义分析失败，停止编译" << std::endl;
            return;
        }
        std::cout << "   ✅ 语义检查通过" << std::endl;

        // 4. 代码生成 (Code Generation)
        std::cout << "⚙️ 步骤 4: 代码生成..." << std::endl;
        CodeGenerator codeGenerator;
        std::string output = codeGenerator.generate(ast);
        std::cout << "   ✅ 代码生成完成" << std::endl;

        std::cout << "\n📋 生成的C++代码:" << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        std::cout << output << std::endl;
        std::cout << "----------------------------------------" << std::endl;

        // 5-7. 编译并运行C++代码
        bool success = compileCppCode(output, filename);

        if (success) {
            std::cout << "\n🎉 polyglot编译完整流程成功！" << std::endl;
        } else {
            std::cout << "\n⚠️  polyglot转换成功，但C++编译失败" << std::endl;
        }

    } catch (const CompilerError& e) {
        std::cerr << "编译错误: " << e.what() << std::endl;
        exit(1);
    }
}

int main(int argc, char* argv[]) {
    // 设置控制台UTF-8编码
    setupConsoleUTF8();

    if (argc != 2) {
        printUsage();
        return 1;
    }

    std::string filename = argv[1];

    try {
        std::string sourceCode = readFile(filename);
        compile(sourceCode, filename);
    } catch (const CompilerError& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}