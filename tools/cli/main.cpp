#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <cstdlib>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

static int run_cmd(const std::string &cmd) {
    std::cout << "[CLI] $ " << cmd << std::endl;
    return std::system(cmd.c_str());
}

static void print_header() {
    std::cout << "文达 CLI (polyglot) — C++17" << std::endl;
    std::cout << "用法: wenda <子命令> [选项]" << std::endl;
    std::cout << std::endl;
}

static int print_help() {
    print_header();
    std::cout << "子命令:" << std::endl;
    std::cout << "  build        配置并编译项目 (CMake)" << std::endl;
    std::cout << "  run          运行程序文件 (.pg / .文达)" << std::endl;
    std::cout << "  test         运行测试 (golden / ai_tests)" << std::endl;
    std::cout << std::endl;
    std::cout << "示例:" << std::endl;
    std::cout << "  wenda build --release" << std::endl;
    std::cout << "  wenda run --file tutorial/示例1/中文编程示例.文达" << std::endl;
    std::cout << "  wenda test" << std::endl;
    return 0;
}

static int cmd_build(const std::vector<std::string> &args) {
    // 解析选项: --release/--debug, --clean
    bool release = false;
    bool debug = false;
    bool clean = false;

    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--release") release = true;
        else if (args[i] == "--debug") debug = true;
        else if (args[i] == "--clean") clean = true;
    }

    std::string build_type = "RelWithDebInfo";
    if (release) build_type = "Release";
    if (debug) build_type = "Debug";

    if (clean && fs::exists("build")) {
        std::cout << "[CLI] 清理构建目录 build/" << std::endl;
        std::error_code ec; fs::remove_all("build", ec);
        if (ec) {
            std::cerr << "[CLI] 清理失败: " << ec.message() << std::endl;
            return 1;
        }
    }

    int rc = 0;
    rc = run_cmd("cmake -S . -B build -DCMAKE_BUILD_TYPE=" + build_type);
    if (rc != 0) return rc;
    rc = run_cmd("cmake --build build -j");
    return rc;
}

static int cmd_run(const std::vector<std::string> &args) {
    // 选项: --file <path> | -f <path>
    std::optional<std::string> file;
    for (size_t i = 0; i < args.size(); ++i) {
        if ((args[i] == "--file" || args[i] == "-f") && i + 1 < args.size()) {
            file = args[i + 1];
            ++i;
        }
    }

    if (!file) {
        std::cerr << "[CLI] 缺少 --file 参数 (支持 .pg / .文达)" << std::endl;
        std::cerr << "示例: wenda run --file tutorial/示例1/中文编程示例.文达" << std::endl;
        return 2;
    }

    // 这里先用占位实现：
    // - 若是 .pg，调用英文版解释/编译入口（假设 build/bin/polyglot 存在）
    // - 若是 .文达，调用中文版入口（假设 构建/可执行程序/文达 存在）

    fs::path p(*file);
    auto ext = p.extension().string();
    if (ext == ".pg") {
        // 优先使用 build/bin 下的英文可执行器
        if (!fs::exists("build/bin/polyglot")) {
            std::cout << "[CLI] 未发现 build/bin/polyglot，先执行构建…" << std::endl;
            int rc = cmd_build({});
            if (rc != 0) return rc;
        }
        return run_cmd("./build/bin/polyglot " + p.string());
    } else {
        // 优先使用 build/bin 下的中文可执行器，其次兼容 旧路径 构建/可执行程序/文达
        std::string zh_exec = "build/bin/文达";
        if (!fs::exists(zh_exec)) {
            if (!fs::exists("构建/可执行程序/文达")) {
                std::cout << "[CLI] 未发现中文可执行器，先执行构建…" << std::endl;
                int rc = cmd_build({});
                if (rc != 0) return rc;
            } else {
                return run_cmd("./构建/可执行程序/文达 " + p.string());
            }
        }
        return run_cmd("./build/bin/文达 " + p.string());
    }
}

static int cmd_test(const std::vector<std::string> &args) {
    (void)args;
    // 最小实现：
    // - 优先尝试 ctest
    // - 若无 ctest 配置，则退化为运行 ai_tests 目录的示例（占位）

    if (!fs::exists("build")) {
        std::cout << "[CLI] 未发现构建目录，先执行构建…" << std::endl;
        int rc = cmd_build({});
        if (rc != 0) return rc;
    }

    // 尝试 ctest
    int rc = run_cmd("ctest --test-dir build --output-on-failure");
    if (rc == 0) return 0;

    std::cout << "[CLI] 未配置 ctest，回退到示例测试 (占位)…" << std::endl;
    // 占位：执行内置样例（后续用 golden 测试框架替代）
    if (fs::exists("ai_tests/examples/test_builtin_print.pg")) {
        rc = cmd_run({"--file", "ai_tests/examples/test_builtin_print.pg"});
    }
    return rc;
}

int main(int argc, char **argv) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) args.emplace_back(argv[i]);

    if (args.empty()) {
        return print_help();
    }

    const auto &cmd = args[0];
    std::vector<std::string> sub(args.begin() + 1, args.end());

    if (cmd == "help" || cmd == "-h" || cmd == "--help") {
        return print_help();
    } else if (cmd == "build") {
        return cmd_build(sub);
    } else if (cmd == "run") {
        return cmd_run(sub);
    } else if (cmd == "test") {
        return cmd_test(sub);
    }

    std::cerr << "未知子命令: " << cmd << std::endl;
    print_help();
    return 1;
}
