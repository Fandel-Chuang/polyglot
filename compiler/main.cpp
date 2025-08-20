// 先包含我们自己的头文件
#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "error.h"
#include "ast_interpreter.h"
#include "symbol_config.h"

// 然后包含标准库
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <unordered_map>
#include <algorithm>

// 最后包含系统特定头文件
#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#else
#include <clocale>
#endif

// 简化的包管理器类（用于AST解释器模式）
namespace polyglot {
    class IntegratedPackageManager {
    private:
        std::string project_root;

    public:
        IntegratedPackageManager(const std::string& root) : project_root(root) {}

        bool resolveDependencies(const std::vector<std::string>& source_files) {
            return true; // AST模式不需要复杂的依赖解析
        }

        bool resolveDependencies(const std::string& source_code) {
            return true; // AST模式不需要复杂的依赖解析
        }

        void updateDependencies() {
            std::cout << "📦 AST模式：跳过依赖更新" << std::endl;
        }

        void cleanCache() {
            std::cout << "🧹 AST模式：无需清理缓存" << std::endl;
        }

        void printDependencyInfo() {
            std::cout << "📋 AST模式：直接解释执行，无外部依赖" << std::endl;
        }

        std::vector<std::string> getIncludePaths() {
            return {}; // AST模式不需要包含路径
        }
    };
}

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
    std::cout << "Polyglot编程语言编译器 v1.0.0" << std::endl;
    std::cout << std::endl;
    std::cout << "用法: polyglot [选项] <源文件.pg>" << std::endl;
    std::cout << std::endl;
    std::cout << "选项:" << std::endl;
    std::cout << "  -h, --help          显示帮助信息" << std::endl;
    std::cout << "  --update-deps       编译前更新所有依赖" << std::endl;
    std::cout << "  --clean-cache       清理依赖缓存" << std::endl;
    std::cout << "  --no-deps          跳过依赖解析（仅编译本地代码）" << std::endl;
    std::cout << "  --deps-info        显示依赖信息" << std::endl;
    std::cout << "  -v, --verbose       详细输出模式" << std::endl;
    std::cout << std::endl;
    std::cout << "示例:" << std::endl;
    std::cout << "  polyglot main.pg                编译程序" << std::endl;
    std::cout << "  polyglot --update-deps main.pg  更新依赖后编译" << std::endl;
    std::cout << "  polyglot --clean-cache          清理依赖缓存" << std::endl;
}

std::string readFile(const std::string& filename) {
#ifdef _WIN32
    // 使用宽字符接口读取UTF-8路径的文件
    int wlen = MultiByteToWideChar(CP_UTF8, 0, filename.c_str(), -1, nullptr, 0);
    if (wlen <= 0) {
        throw CompilerError("路径转码失败: " + filename);
    }
    std::wstring wpath;
    wpath.resize(wlen - 1);
    MultiByteToWideChar(CP_UTF8, 0, filename.c_str(), -1, &wpath[0], wlen);

    FILE* fp = _wfopen(wpath.c_str(), L"rb");
    if (!fp) {
        throw CompilerError("无法打开文件: " + filename);
    }
    std::string content;
    char buf[4096];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), fp)) > 0) {
        content.append(buf, n);
    }
    fclose(fp);
    // 规范换行到\n
    std::string normalized;
    normalized.reserve(content.size());
    for (size_t i = 0; i < content.size(); ++i) {
        if (content[i] == '\r') {
            if (i + 1 < content.size() && content[i + 1] == '\n') {
                // skip, handled by next char
            } else {
                normalized.push_back('\n');
            }
        } else {
            normalized.push_back(content[i]);
        }
    }
    return normalized;
#else
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
#endif
}

// 判断文件名（不含扩展名）是否纯英文（ASCII 字母/数字/下划线/连字符）
bool isEnglishFilename(const std::string& filepath) {
    // 避免 Windows 上 std::filesystem 对中文路径的转换问题，这里用字符串解析
    size_t pos = filepath.find_last_of("/\\");
    std::string name = (pos == std::string::npos) ? filepath : filepath.substr(pos + 1);
    size_t dot = name.find_last_of('.');
    std::string stem = (dot == std::string::npos) ? name : name.substr(0, dot);
    if (stem.empty()) return true;
    for (unsigned char ch : stem) {
        if (ch > 127) return false; // 非 ASCII 直接判定为非英文
        if (!(std::isalnum(ch) || ch == '_' || ch == '-')) return false;
    }
    return true;
}

// 简单解析 JSON，提取 string_delimiters.double_quote 列表（不依赖第三方库）
static std::vector<std::string> parseDoubleQuoteVariants(const std::string& json) {
    std::vector<std::string> variants;
    const std::string key = "\"double_quote\"";
    size_t pos = json.find(key);
    if (pos == std::string::npos) return variants;
    size_t lb = json.find('[', pos);
    size_t rb = json.find(']', lb);
    if (lb == std::string::npos || rb == std::string::npos) return variants;
    std::string arr = json.substr(lb + 1, rb - lb - 1);
    size_t cur = 0;
    while (true) {
        size_t q1 = arr.find('"', cur);
        if (q1 == std::string::npos) break;
        size_t q2 = arr.find('"', q1 + 1);
        if (q2 == std::string::npos) break;
        variants.push_back(arr.substr(q1 + 1, q2 - q1 - 1));
        cur = q2 + 1;
    }
    return variants;
}

// 将源码中的非英文符号替换为 ASCII 规范符号（依据 symbol_mapping.json 和类型同类映射）
std::string normalizeSourceBySymbols(const std::string& source, const std::string& jsonPath) {
    std::string normalized = source;

    // 1) 加载 JSON 配置
    SymbolConfigLoader loader(jsonPath);
    loader.loadConfig();
    const auto& allMap = loader.getAllSymbolTokenTypes();

    // 2) 为每个 TokenType 选取一个 ASCII 代表符号
    std::unordered_map<int, std::string> asciiCanonical;
    for (const auto& kv : allMap) {
        const std::string& sym = kv.first;
        auto tt = kv.second;
        bool isAscii = std::all_of(sym.begin(), sym.end(), [](unsigned char c){ return c <= 127; });
        if (isAscii) {
            int key = static_cast<int>(tt);
            if (!asciiCanonical.count(key)) asciiCanonical[key] = sym;
        }
    }

    // 3) 构造替换表：将所有非 ASCII 的同类符号替换为 ASCII 代表
    std::vector<std::pair<std::string,std::string>> replaces;
    for (const auto& kv : allMap) {
        const std::string& sym = kv.first;
        auto tt = kv.second;
        bool isAscii = std::all_of(sym.begin(), sym.end(), [](unsigned char c){ return c <= 127; });
        if (!isAscii) {
            int key = static_cast<int>(tt);
            auto it = asciiCanonical.find(key);
            if (it != asciiCanonical.end() && it->second != sym) {
                replaces.emplace_back(sym, it->second);
            }
        }
    }

    // 4) 单独处理字符串引号：将中文书名号 “ ” 等替换为 ASCII 双引号
    try {
        std::string json = readFile(jsonPath);
        auto dq = parseDoubleQuoteVariants(json);
        if (!dq.empty()) {
            std::string asciiQ = dq.front();
            for (const auto& v : dq) {
                if (v != asciiQ) replaces.emplace_back(v, asciiQ);
            }
        }
    } catch (...) {
        // 读取失败忽略，继续用默认映射
    }

    // 5) 执行替换（从长到短，避免前后覆盖）
    std::sort(replaces.begin(), replaces.end(), [](const auto& a, const auto& b){ return a.first.size() > b.first.size(); });
    for (const auto& rp : replaces) {
        const std::string& from = rp.first;
        const std::string& to   = rp.second;
        size_t pos = 0;
        while ((pos = normalized.find(from, pos)) != std::string::npos) {
            normalized.replace(pos, from.size(), to);
            pos += to.size();
        }
    }

    return normalized;
}

// 构建并运行生成的C++代码
bool runGeneratedCppCode(const std::string& cppCode, const std::string& baseName) {
    // 生成输出文件名
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

// 带选项的编译函数
void compileWithOptions(const std::string& sourceCode, const std::string& filename,
                       bool updateDeps, bool noDeps, bool verbose, polyglot::IntegratedPackageManager& packageManager) {
    std::cout << "🚀 开始解释执行 polyglot 程序: " << filename << std::endl;

    try {
        // 0. 包管理和依赖解析
        if (!noDeps) {
            std::cout << "📦 步骤 0: 依赖管理..." << std::endl;

            if (updateDeps) {
                std::cout << "🔄 更新依赖..." << std::endl;
                packageManager.updateDependencies();
            }

            // 解析依赖
            std::vector<std::string> source_files = {filename};
            bool depResolved = packageManager.resolveDependencies(source_files);

            if (!depResolved) {
                std::cerr << "❌ 依赖解析失败，停止编译" << std::endl;
                return;
            }

            // 设置包含路径（为后续的导入解析做准备）
            auto include_paths = packageManager.getIncludePaths();
            if (verbose) {
                std::cout << "   📂 包含路径:" << std::endl;
                for (const auto& path : include_paths) {
                    std::cout << "     - " << path << std::endl;
                }
            } else {
                std::cout << "   📂 设置包含路径: " << include_paths.size() << " 个路径" << std::endl;
            }
        } else {
            std::cout << "⏭️  跳过依赖解析" << std::endl;
        }

        // 1. 词法分析 (Lexical Analysis)
        std::cout << "📝 步骤 1: 词法分析..." << std::endl;
        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();
        std::cout << "   🔤 词法分析完成" << std::endl;

        // 调试：打印前20个Token用于分析
        if (verbose) {
            std::cout << "   🔍 前20个Token:" << std::endl;
            size_t maxTokens = tokens.size() < 20 ? tokens.size() : 20;
            for (size_t i = 0; i < maxTokens; ++i) {
                std::cout << "     [" << i << "] 类型=" << static_cast<int>(tokens[i].type)
                          << ", 值='" << tokens[i].value << "'" << std::endl;
            }
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

        // 4. AST可视化（如果需要）
        if (verbose) {
            std::cout << "🌳 步骤 4: AST可视化..." << std::endl;
            polyglot::ASTVisualizer::printAST(ast.get());
        }

        // 5. AST分析
        std::cout << "🔍 步骤 5: AST分析..." << std::endl;
        auto analysis = polyglot::ASTAnalyzer::analyze(dynamic_cast<Program*>(ast.get()));
        std::cout << "   ✅ AST分析完成" << std::endl;

        // 6. AST解释执行
        std::cout << "🚀 步骤 7: AST解释执行..." << std::endl;
        polyglot::ASTInterpreter interpreter;
        auto result = interpreter.interpret(ast);

        std::cout << "\n🎉 polyglot程序解释执行完成！" << std::endl;
        std::cout << "   📊 程序统计:" << std::endl;
        std::cout << "     - 总节点数: " << analysis.totalNodes << std::endl;
        std::cout << "     - 函数数量: " << analysis.functionCount << std::endl;
        std::cout << "     - 变量数量: " << analysis.variableCount << std::endl;

        // 可选：导出AST到文件
        if (verbose) {
            std::string baseName = filename.substr(0, filename.find_last_of('.'));
            polyglot::ASTVisualizer::exportToJSON(ast.get(), baseName + "_ast.json");
            polyglot::ASTVisualizer::exportToDot(ast.get(), baseName + "_ast.dot");
        }

        // 上面的代码已改为AST解释执行

        return;

    } catch (const CompilerError& e) {
        std::cerr << "解释执行错误: " << e.what() << std::endl;
        exit(1);
    }
}

// 简化的编译函数（向后兼容）
void compile(const std::string& sourceCode, const std::string& filename) {
    // 简化：避免对中文路径做 std::filesystem 处理
    std::string project_root = ".";

    polyglot::IntegratedPackageManager packageManager(project_root);

    // 使用默认选项调用带选项的编译函数
    compileWithOptions(sourceCode, filename, false, false, false, packageManager);
}


int main(int argc, char* argv[]) {
    // 设置控制台UTF-8编码
    setupConsoleUTF8();

    if (argc < 2) {
        printUsage();
        return 1;
    }

    // 解析命令行参数
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }

    bool updateDeps = false;
    bool cleanCache = false;
    bool noDeps = false;
    bool showDepsInfo = false;
    bool verbose = false;
    std::string sourceFile;

    // 处理选项
    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];

        if (arg == "-h" || arg == "--help") {
            printUsage();
            return 0;
        } else if (arg == "--update-deps") {
            updateDeps = true;
        } else if (arg == "--clean-cache") {
            cleanCache = true;
        } else if (arg == "--no-deps") {
            noDeps = true;
        } else if (arg == "--deps-info") {
            showDepsInfo = true;
        } else if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        } else if (arg.find("--") == 0) {
            std::cerr << "❌ 未知选项: " << arg << std::endl;
            printUsage();
            return 1;
        } else if (sourceFile.empty()) {
            sourceFile = arg;
        } else {
            std::cerr << "❌ 只能指定一个源文件" << std::endl;
            printUsage();
            return 1;
        }
    }

    try {
        // 获取项目根目录（简化避免中文路径导致的转换异常）
        std::string project_root = ".";

        // 初始化包管理器
        polyglot::IntegratedPackageManager packageManager(project_root);

        // 处理包管理命令
        if (cleanCache) {
            packageManager.cleanCache();
            if (sourceFile.empty()) {
                return 0; // 只清理缓存，不编译
            }
        }

        if (updateDeps && !sourceFile.empty()) {
            std::cout << "🔄 更新项目依赖..." << std::endl;
            packageManager.updateDependencies();
        }

        if (showDepsInfo) {
            // 先解析依赖以获取信息
            if (!sourceFile.empty()) {
                std::vector<std::string> source_files = {sourceFile};
                packageManager.resolveDependencies(source_files);
            }
            packageManager.printDependencyInfo();
            if (sourceFile.empty()) {
                return 0; // 只显示信息，不编译
            }
        }

        if (sourceFile.empty()) {
            std::cerr << "❌ 请指定要解释执行的源文件" << std::endl;
            printUsage();
            return 1;
        }

        // 读取源代码
        std::string sourceCode = readFile(sourceFile);

        // 语种检测：英文文件名 -> 使用默认符号；非英文文件名 -> 加载 JSON 并进行源码规范化
        if (!isEnglishFilename(sourceFile)) {
            std::cout << "🌐 检测到非英文文件名，按本地化符号配置进行规范化处理..." << std::endl;
            // 1) 预处理：将源代码中的本地化符号规范化为ASCII（避免在编译器内部处理全角）
            sourceCode = normalizeSourceBySymbols(sourceCode, "symbol_mapping.json");
            // 2) 替换内存中的符号映射（如需在后续阶段基于JSON的符号集做进一步处理）
            SymbolConfigLoader loader("symbol_mapping.json");
            if (loader.loadConfig()) {
                Lexer::OverrideSymbolMap(loader.getAllSymbolTokenTypes());
            }
        } else {
            std::cout << "🔤 检测到英文文件名，使用默认符号映射（不加载JSON）" << std::endl;
            Lexer::ClearOverride();
        }

        // 使用AST解释器模式进行编译执行
        compileWithOptions(sourceCode, sourceFile, updateDeps, noDeps, verbose, packageManager);

    } catch (const CompilerError& e) {
        std::cerr << "解释执行错误: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "系统错误: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}