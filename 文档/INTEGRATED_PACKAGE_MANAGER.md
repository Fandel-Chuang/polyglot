# Polyglot编译器集成包管理器

## 🎉 功能概述

polyglot编译器现已集成强大的包管理功能，无需单独安装PGM工具！编译器会在编译过程中自动处理依赖下载和管理。

## 🚀 主要特性

- **🔗 编译时依赖解析** - 编译时自动下载所需依赖
- **💾 智能缓存** - 本地缓存避免重复下载
- **🔄 依赖更新** - 可选择更新到最新版本
- **📦 Git原生支持** - 直接使用GitHub/GitLab仓库
- **🔧 灵活配置** - 支持版本、分支、提交哈希
- **⚡ 快速编译** - 可跳过依赖解析加速编译

## 📋 新增命令行选项

```bash
# 基本编译
polyglot main.pg

# 更新依赖后编译
polyglot --update-deps main.pg

# 跳过依赖解析（仅编译本地代码）
polyglot --no-deps main.pg

# 详细输出模式
polyglot --verbose main.pg

# 显示依赖信息
polyglot --deps-info main.pg

# 清理依赖缓存
polyglot --clean-cache

# 显示帮助
polyglot --help
```

## 📁 项目结构

```
my-project/
├── package.pgm          # 包配置文件（可选）
├── main.pg              # 主程序文件
└── src/                 # 其他源文件
    └── utils.pg
```

## 📋 package.pgm 配置文件

在项目根目录创建`package.pgm`文件来配置依赖：

```ini
# 项目信息
name = "my-awesome-project"
version = "1.0.0"
description = "我的超棒项目"
author = "Your Name <email@example.com>"

# 运行时依赖
dep.github.com/polyglot-community/http-client = "v1.5.0"
dep.github.com/user/json-parser = "latest"
dep.gitlab.com/org/math-utils = "branch:main"
dep.bitbucket.org/team/graphics = "commit:abc123"

# 开发依赖
dev.github.com/polyglot-community/test-framework = "v2.0.0"
```

## 💻 代码中使用依赖

```polyglot
// 导入标准库
>> "io/console"
>> "collections/array"

// 导入远程依赖（编译器会自动下载）
>> "github.com/polyglot-community/http-client"
>> "github.com/user/json-parser"
>> "gitlab.com/org/math-utils"

main() {
    // 使用HTTP客户端
    ? client = HttpClient()
    ? response = client.get("https://api.example.com/data")

    // 使用JSON解析器
    (response.is_success()) ? {
        ? json_data = JsonParser::parse(response.unwrap().body)
        println("数据: " + json_data.to_string())
    }

    <- 0
}
```

## 🔄 编译流程

编译器的完整流程现在包含：

1. **📦 步骤 0: 依赖管理**
   - 扫描源文件中的远程导入
   - 解析`package.pgm`配置
   - 下载和缓存依赖包
   - 递归解析子依赖

2. **📝 步骤 1: 词法分析**
   - 标准词法分析流程

3. **🔍 步骤 2: 语法分析**
   - 生成抽象语法树

4. **🧠 步骤 3: 语义分析**
   - 类型检查和语义验证

5. **⚙️ 步骤 4: 代码生成**
   - 生成C++代码

6. **🔨 步骤 5-7: 编译执行**
   - 编译C++代码并运行

## 📂 缓存机制

- **缓存位置**: `~/.pgm/cache/` (Linux/macOS) 或 `%USERPROFILE%\.pgm\cache\` (Windows)
- **缓存策略**: 按仓库URL缓存，支持多版本共存
- **清理命令**: `polyglot --clean-cache`

## 🎯 版本指定

| 格式 | 说明 | 示例 |
|------|------|------|
| `v1.2.3` | 精确版本标签 | `v1.2.3`, `v2.0.0` |
| `latest` | 最新版本 | `latest` |
| `branch:name` | 指定分支 | `branch:main`, `branch:develop` |
| `commit:hash` | 指定提交 | `commit:abc123def456` |

## 🌐 支持的Git平台

- **GitHub**: `github.com/user/repo`
- **GitLab**: `gitlab.com/user/repo`
- **Bitbucket**: `bitbucket.org/user/repo`
- **自托管**: `git.company.com/team/project`

## 🚀 快速开始

### 1. 创建新项目

```bash
# 创建项目目录
mkdir my-polyglot-project
cd my-polyglot-project

# 创建主程序
cat > main.pg << 'EOF'
>> "io/console"
>> "github.com/example/awesome-lib"

main() {
    println("Hello, Polyglot with Package Manager!")
    <- 0
}
EOF

# 创建包配置（可选）
cat > package.pgm << 'EOF'
name = "my-polyglot-project"
version = "1.0.0"
dep.github.com/example/awesome-lib = "latest"
EOF
```

### 2. 编译运行

```bash
# 首次编译（会自动下载依赖）
polyglot main.pg

# 后续编译（使用缓存）
polyglot main.pg

# 更新依赖后编译
polyglot --update-deps main.pg
```

## 🔧 高级用法

### 私有仓库

```bash
# 配置Git凭据（仅需一次）
git config --global credential.helper store
# 或配置SSH密钥
```

### 本地开发依赖

```ini
# 在package.pgm中使用本地路径
dep.my-local-lib = "file:../my-local-lib"
```

### 条件依赖

```ini
# 平台特定依赖
[target.linux]
dep.github.com/linux-only/lib = "v1.0.0"

[target.windows]
dep.github.com/windows-only/lib = "v1.0.0"
```

## 🐛 故障排除

### 依赖下载失败

```bash
# 检查网络连接
ping github.com

# 清理缓存重试
polyglot --clean-cache
polyglot main.pg

# 查看详细信息
polyglot --verbose main.pg
```

### 版本冲突

```bash
# 查看依赖信息
polyglot --deps-info main.pg

# 在package.pgm中锁定版本
dep.conflicting-lib = "=v1.2.3"
```

### 编译加速

```bash
# 跳过依赖解析（适合频繁编译）
polyglot --no-deps main.pg

# 仅在依赖变化时重新解析
polyglot --update-deps main.pg
```

## 📚 最佳实践

1. **版本管理**
   - 生产环境使用精确版本
   - 开发阶段可使用`latest`

2. **性能优化**
   - 频繁编译时使用`--no-deps`
   - 定期运行`--update-deps`获取更新

3. **团队协作**
   - 提交`package.pgm`到版本控制
   - 不要提交缓存目录

4. **错误处理**
   - 提供依赖不可用时的降级方案
   - 检查重要依赖的可用性

## 🔮 与独立PGM工具的关系

- **集成优势**: 编译时自动处理依赖，无需单独工具
- **兼容性**: 完全兼容PGM配置格式
- **选择灵活**: 可继续使用独立PGM工具管理依赖

## 📞 获取帮助

```bash
# 查看完整帮助
polyglot --help

# 查看版本信息
polyglot --version

# 查看依赖状态
polyglot --deps-info
```

---

🎉 现在您的polyglot编译器已经具备现代化的包管理能力！享受无缝的依赖管理和编译体验吧！