// ========================================
// Polyglot编译器集成包管理器
// Integrated Package Manager for Polyglot Compiler
// ========================================

#include "package_manager.h"
#include "error.h"
#include "lexer.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <vector>
#include <map>
#include <set>
#include <cstdlib>

namespace polyglot {

class IntegratedPackageManager {
private:
    std::string project_root;
    std::string cache_dir;
    std::map<std::string, std::string> resolved_dependencies;
    std::set<std::string> processing_deps; // 防止循环依赖

public:
    IntegratedPackageManager(const std::string& root)
        : project_root(root) {
        // 设置缓存目录
        const char* home = getenv("HOME");
        if (home) {
            cache_dir = std::string(home) + "/.pgm/cache";
        } else {
            cache_dir = "/tmp/.pgm/cache";
        }

        // 确保缓存目录存在
        std::filesystem::create_directories(cache_dir);
    }

    // 解析源文件中的所有导入语句
    std::vector<std::string> extractImports(const std::string& file_path) {
        std::vector<std::string> imports;
        std::ifstream file(file_path);
        std::string line;

        // 正则表达式匹配导入语句
        std::regex import_regex(R"(^\s*>>\s*"([^"]+)"\s*$)");
        std::regex chinese_import_regex(R"(^\s*》》\s*"([^"]+)"\s*$)");

        while (std::getline(file, line)) {
            std::smatch match;

            // 检查英文导入
            if (std::regex_match(line, match, import_regex)) {
                std::string import_path = match[1].str();
                if (isRemoteDependency(import_path)) {
                    imports.push_back(import_path);
                }
            }
            // 检查中文导入
            else if (std::regex_match(line, match, chinese_import_regex)) {
                std::string import_path = match[1].str();
                if (isRemoteDependency(import_path)) {
                    imports.push_back(import_path);
                }
            }
        }

        return imports;
    }

    // 判断是否为远程依赖
    bool isRemoteDependency(const std::string& path) {
        return path.find("github.com/") == 0 ||
               path.find("gitlab.com/") == 0 ||
               path.find("bitbucket.org/") == 0 ||
               path.find("git.") != std::string::npos;
    }

    // 递归解析所有依赖
    bool resolveDependencies(const std::vector<std::string>& source_files) {
        std::cout << "🔍 解析项目依赖..." << std::endl;

        // 首先从package.pgm读取显式依赖
        loadPackageConfig();

        // 然后从源文件中提取导入的依赖
        std::set<std::string> all_deps;
        for (const auto& file : source_files) {
            auto file_imports = extractImports(file);
            all_deps.insert(file_imports.begin(), file_imports.end());
        }

        // 解析每个依赖
        for (const auto& dep : all_deps) {
            if (!resolveSingleDependency(dep, "latest")) {
                return false;
            }
        }

        std::cout << "✅ 依赖解析完成，共解析 " << resolved_dependencies.size() << " 个依赖" << std::endl;
        return true;
    }

    // 解析单个依赖
    bool resolveSingleDependency(const std::string& repo_url, const std::string& version) {
        // 防止循环依赖
        if (processing_deps.find(repo_url) != processing_deps.end()) {
            std::cerr << "❌ 检测到循环依赖: " << repo_url << std::endl;
            return false;
        }

        // 检查是否已解析
        if (resolved_dependencies.find(repo_url) != resolved_dependencies.end()) {
            return true;
        }

        processing_deps.insert(repo_url);

        std::cout << "📦 下载依赖: " << repo_url << "@" << version << std::endl;

        // 生成本地路径
        std::string local_path = generateLocalPath(repo_url);

        // 检查本地缓存
        if (std::filesystem::exists(local_path)) {
            std::cout << "💾 使用缓存: " << repo_url << std::endl;
        } else {
            // 下载依赖
            if (!downloadDependency(repo_url, version, local_path)) {
                processing_deps.erase(repo_url);
                return false;
            }
        }

        // 记录解析结果
        resolved_dependencies[repo_url] = local_path;

        // 递归解析此依赖的子依赖
        resolveNestedDependencies(local_path);

        processing_deps.erase(repo_url);
        return true;
    }

    // 下载依赖到本地
    bool downloadDependency(const std::string& repo_url, const std::string& version, const std::string& local_path) {
        std::string git_url = convertToGitUrl(repo_url);

        // 克隆仓库
        std::string clone_cmd = "git clone --quiet " + git_url + " " + local_path;
        int result = std::system(clone_cmd.c_str());

        if (result != 0) {
            std::cerr << "❌ 下载失败: " << repo_url << std::endl;
            return false;
        }

        // 切换到指定版本
        if (version != "latest" && version != "main") {
            std::string checkout_cmd = "cd " + local_path + " && git checkout --quiet " + version;
            result = std::system(checkout_cmd.c_str());

            if (result != 0) {
                std::cerr << "⚠️ 版本切换失败，使用默认分支: " << version << std::endl;
            }
        }

        std::cout << "✅ 下载完成: " << repo_url << std::endl;
        return true;
    }

    // 解析嵌套依赖
    void resolveNestedDependencies(const std::string& dep_path) {
        std::string package_file = dep_path + "/package.pgm";
        if (!std::filesystem::exists(package_file)) {
            return; // 没有package.pgm文件，跳过
        }

        // 解析package.pgm中的依赖
        std::ifstream file(package_file);
        std::string line;
        std::regex dep_regex(R"(dep\.(.+)\s*=\s*"(.+)")");

        while (std::getline(file, line)) {
            std::smatch match;
            if (std::regex_match(line, match, dep_regex)) {
                std::string nested_repo = match[1].str();
                std::string nested_version = match[2].str();

                resolveSingleDependency(nested_repo, nested_version);
            }
        }
    }

    // 从package.pgm加载配置
    void loadPackageConfig() {
        std::string config_path = project_root + "/package.pgm";
        if (!std::filesystem::exists(config_path)) {
            return; // 没有配置文件
        }

        std::ifstream file(config_path);
        std::string line;
        std::regex dep_regex(R"(dep\.(.+)\s*=\s*"(.+)")");

        while (std::getline(file, line)) {
            std::smatch match;
            if (std::regex_match(line, match, dep_regex)) {
                std::string repo = match[1].str();
                std::string version = match[2].str();

                resolveSingleDependency(repo, version);
            }
        }
    }

    // 生成本地路径
    std::string generateLocalPath(const std::string& repo_url) {
        std::string safe_name = repo_url;
        std::replace(safe_name.begin(), safe_name.end(), '/', '_');
        std::replace(safe_name.begin(), safe_name.end(), '.', '_');
        std::replace(safe_name.begin(), safe_name.end(), ':', '_');

        return cache_dir + "/" + safe_name;
    }

    // 转换为Git URL
    std::string convertToGitUrl(const std::string& repo_url) {
        if (repo_url.find("github.com/") == 0) {
            return "https://" + repo_url + ".git";
        } else if (repo_url.find("gitlab.com/") == 0) {
            return "https://" + repo_url + ".git";
        } else if (repo_url.find("bitbucket.org/") == 0) {
            return "https://" + repo_url + ".git";
        }
        return repo_url; // 假设已经是完整的Git URL
    }

    // 获取包含路径列表（用于编译器）
    std::vector<std::string> getIncludePaths() {
        std::vector<std::string> paths;

        // 添加标准库路径
        paths.push_back(project_root + "/stdlib");

        // 添加所有依赖路径
        for (const auto& dep : resolved_dependencies) {
            paths.push_back(dep.second);

            // 检查是否有src目录
            std::string src_path = dep.second + "/src";
            if (std::filesystem::exists(src_path)) {
                paths.push_back(src_path);
            }
        }

        return paths;
    }

    // 解析导入路径到实际文件路径
    std::string resolveImportPath(const std::string& import_path) {
        // 如果是远程依赖
        if (isRemoteDependency(import_path)) {
            auto it = resolved_dependencies.find(import_path);
            if (it != resolved_dependencies.end()) {
                std::string base_path = it->second;

                // 尝试不同的文件位置
                std::vector<std::string> possible_paths = {
                    base_path + "/src/" + extractFileName(import_path) + ".pg",
                    base_path + "/" + extractFileName(import_path) + ".pg",
                    base_path + "/lib/" + extractFileName(import_path) + ".pg",
                    base_path + "/src/" + extractFileName(import_path) + ".文达",
                    base_path + "/" + extractFileName(import_path) + ".文达"
                };

                for (const auto& path : possible_paths) {
                    if (std::filesystem::exists(path)) {
                        return path;
                    }
                }
            }
        }

        // 标准库或本地文件
        std::vector<std::string> search_paths = {
            project_root + "/stdlib/" + import_path + ".pg",
            project_root + "/stdlib/" + import_path + ".文达",
            project_root + "/src/" + import_path + ".pg",
            project_root + "/src/" + import_path + ".文达",
            project_root + "/" + import_path + ".pg",
            project_root + "/" + import_path + ".文达"
        };

        for (const auto& path : search_paths) {
            if (std::filesystem::exists(path)) {
                return path;
            }
        }

        return ""; // 找不到文件
    }

    // 从导入路径提取文件名
    std::string extractFileName(const std::string& import_path) {
        size_t last_slash = import_path.find_last_of('/');
        if (last_slash != std::string::npos) {
            return import_path.substr(last_slash + 1);
        }
        return import_path;
    }

    // 更新所有依赖
    bool updateDependencies() {
        std::cout << "🔄 更新所有依赖..." << std::endl;

        for (auto& dep : resolved_dependencies) {
            std::string repo_url = dep.first;
            std::string local_path = dep.second;

            if (std::filesystem::exists(local_path)) {
                std::cout << "📦 更新: " << repo_url << std::endl;

                std::string pull_cmd = "cd " + local_path + " && git pull --quiet";
                int result = std::system(pull_cmd.c_str());

                if (result == 0) {
                    std::cout << "✅ 更新成功: " << repo_url << std::endl;
                } else {
                    std::cout << "⚠️ 更新失败: " << repo_url << std::endl;
                }
            }
        }

        return true;
    }

    // 清理缓存
    void cleanCache() {
        std::cout << "🧹 清理依赖缓存..." << std::endl;

        if (std::filesystem::exists(cache_dir)) {
            std::filesystem::remove_all(cache_dir);
            std::filesystem::create_directories(cache_dir);
            std::cout << "✅ 缓存已清理" << std::endl;
        }

        resolved_dependencies.clear();
    }

    // 获取依赖信息
    void printDependencyInfo() {
        std::cout << "\n📋 依赖信息:" << std::endl;
        std::cout << "缓存目录: " << cache_dir << std::endl;
        std::cout << "已解析依赖: " << resolved_dependencies.size() << std::endl;

        for (const auto& dep : resolved_dependencies) {
            std::cout << "  " << dep.first << " -> " << dep.second << std::endl;
        }
    }
};

} // namespace polyglot