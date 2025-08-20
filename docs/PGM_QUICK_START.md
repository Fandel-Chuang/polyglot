# Polyglot包管理器(PGM) - 快速入门指南

## 🚀 概述

PGM (Polyglot Package Manager) 是为polyglot/文达编程语言设计的现代化包管理器，类似于Go的模块系统。它支持直接使用Git仓库作为依赖源，无需中央仓库注册。

## ✨ 主要特性

- **Git原生支持**: 直接使用GitHub、GitLab、Bitbucket等Git仓库
- **版本控制**: 支持标签、分支、提交哈希等多种版本指定方式
- **依赖解析**: 自动递归解析和下载依赖
- **本地缓存**: 智能缓存机制，避免重复下载
- **项目初始化**: 快速创建新项目结构
- **符号语法**: 完全兼容polyglot语言的符号化语法

## 📦 安装

```bash
# 下载PGM可执行文件
curl -L https://github.com/polyglot-lang/pgm/releases/latest/download/pgm-linux -o pgm
chmod +x pgm
sudo mv pgm /usr/local/bin/

# 或者从源码编译
git clone https://github.com/polyglot-lang/pgm.git
cd pgm
polyglot build -o pgm tools/pgm/package_manager.pg
```

## 🏁 快速开始

### 1. 初始化新项目

```bash
# 创建新项目
pgm init my-awesome-project
cd my-awesome-project

# 项目结构：
# my-awesome-project/
# ├── package.pgm       # 包配置文件
# ├── src/
# │   └── main.pg       # 主程序文件
# ├── tests/            # 测试目录
# └── README.md         # 项目说明
```

### 2. 添加依赖

```bash
# 添加GitHub依赖（最新版本）
pgm add github.com/polyglot-community/http-client

# 添加特定版本
pgm add github.com/user/awesome-lib v2.1.0

# 添加特定分支
pgm add gitlab.com/org/utils branch:develop

# 添加特定提交
pgm add bitbucket.org/team/config commit:abc123def
```

### 3. 安装依赖

```bash
# 安装所有依赖
pgm install

# 更新所有依赖
pgm update
```

### 4. 使用依赖

在你的polyglot代码中直接导入远程依赖：

```polyglot
// main.pg
>> "io/console"
>> "github.com/polyglot-community/http-client"
>> "github.com/user/json-parser"

main() {
    ? client = HttpClient()
    ? response = client.get("https://api.example.com/data")

    (response.is_success()) ? {
        ? json = JsonParser::parse(response.unwrap().body)
        println("数据: " + json.to_string())
    }

    <- 0
}
```

## 📋 package.pgm 配置文件

```ini
# 项目基本信息
name = "my-awesome-project"
version = "1.0.0"
description = "我的超棒项目"
author = "Your Name <your.email@example.com>"
license = "MIT"
repository = "github.com/username/my-awesome-project"

# 运行时依赖
dep.github.com/polyglot-community/http-client = "v1.5.0"
dep.github.com/user/graphics-lib = "v2.3.1"
dep.gitlab.com/org/data-structures = "latest"

# 开发依赖
dev.github.com/polyglot-community/test-framework = "v3.0.0"
dev.github.com/polyglot-community/benchmark = "latest"
```

## 🎯 版本指定方式

| 格式 | 说明 | 示例 |
|------|------|------|
| `v1.2.3` | 精确版本标签 | `v1.2.3`, `v2.0.0` |
| `latest` | 最新版本 | `latest` |
| `branch:name` | 特定分支 | `branch:main`, `branch:develop` |
| `commit:hash` | 特定提交 | `commit:abc123def456` |

## 🛠️ 常用命令

### 项目管理
```bash
pgm init <project_name>    # 初始化新项目
pgm list                   # 列出所有依赖
```

### 依赖管理
```bash
pgm add <package> [version]    # 添加依赖
pgm install                    # 安装所有依赖
pgm update                     # 更新所有依赖
pgm clean                      # 清理缓存
```

### 发布管理
```bash
pgm publish                    # 发布包到Git仓库
pgm search <query>             # 搜索包（未来功能）
```

## 🌐 支持的Git平台

- **GitHub**: `github.com/user/repo`
- **GitLab**: `gitlab.com/user/repo`
- **Bitbucket**: `bitbucket.org/user/repo`
- **自托管Git**: `git.company.com/team/project`
- **SSH格式**: `git@github.com:user/repo.git`

## 📁 项目结构建议

```
my-project/
├── package.pgm              # 包配置
├── src/                     # 源代码
│   ├── main.pg             # 主程序
│   ├── lib/                # 库代码
│   └── utils/              # 工具函数
├── tests/                   # 测试代码
│   ├── unit/               # 单元测试
│   └── integration/        # 集成测试
├── docs/                    # 文档
├── examples/                # 示例代码
├── scripts/                 # 构建脚本
└── README.md               # 项目说明
```

## 🔧 高级用法

### 1. 私有仓库

```bash
# 设置Git凭据
git config --global credential.helper store

# 添加私有仓库依赖
pgm add github.com/private-org/secret-lib v1.0.0
```

### 2. 本地开发

```bash
# 使用本地路径进行开发
dep.local-lib = "file:../local-lib"
```

### 3. 条件依赖

```ini
# 平台特定依赖
[target.linux]
dep.github.com/linux-specific/lib = "v1.0.0"

[target.windows]
dep.github.com/windows-specific/lib = "v1.0.0"
```

## 🐛 故障排除

### 依赖下载失败
```bash
# 清理缓存重试
pgm clean
pgm install

# 检查网络连接
curl -I https://github.com
```

### 版本冲突
```bash
# 查看依赖树
pgm list --tree

# 强制使用特定版本
dep.conflicting-lib = "=v1.2.3"  # 精确版本
```

### 私有仓库访问
```bash
# 配置SSH密钥
ssh-keygen -t rsa -b 4096 -C "your.email@example.com"
# 将公钥添加到Git平台

# 或使用访问令牌
git config --global credential.helper store
```

## 📚 最佳实践

1. **版本锁定**: 生产环境使用精确版本号
2. **定期更新**: 定期运行 `pgm update` 获取安全更新
3. **文档完善**: 在README中说明如何安装和使用
4. **测试覆盖**: 使用测试框架确保依赖兼容性
5. **许可证兼容**: 检查依赖许可证与项目兼容

## 🔮 未来规划

- [ ] 中央包注册表
- [ ] 包搜索功能
- [ ] 安全漏洞扫描
- [ ] 构建缓存优化
- [ ] IDE集成支持
- [ ] 包质量评分

## 📞 支持与社区

- **文档**: https://polyglot-lang.org/docs/pgm
- **问题反馈**: https://github.com/polyglot-lang/pgm/issues
- **社区讨论**: https://discord.gg/polyglot
- **贡献指南**: https://github.com/polyglot-lang/pgm/blob/main/CONTRIBUTING.md

---

开始你的polyglot项目之旅吧！🚀