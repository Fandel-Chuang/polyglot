# 多AI协作开发路线图与任务拆解

本文档用于指导 polyglot/文达 项目的多 AI 协同开发，按模块拆分剩余工作量，明确优先级、依赖关系、验收标准与分工建议。

---

## 一、当前能力基线
- 中英文/全角半角识别；主函数 main/主函数；print/打印 内建
- 变量声明、基本类型（int/f32/f64/string/bool/char）
- 词法/语法/语义/代码生成最小闭环跑通
- Windows 控制台 UTF-8 输出处理（SetConsoleOutputCP）
- 符号映射配置 symbol_mapping.json 统一

---

## 二、模块划分与任务清单
以下采用 Story Points（SP）估算，1SP≈0.5~1 人日。

### 模块A. 词法/符号配置与Lexer（A）
- A1 全角/半角符号一致性策略（容错与提示）[P1][3SP]
- A2 字符串字面量：中文书名号“”+转义+Unicode [P1][5SP]
- A3 数字字面量：默认类型与后缀（如 1.0f）[P0][5SP]
- A4 注释/文档注释 [P2][3SP]
- 验收：30+ golden 测试，错误定位到行列

### 模块B. 语法分析器 Parser（B）
- B1 结构体 @、方法块 &、接口 %、枚举 # 语法 [P0][8SP]
- B2 控制流：条件 ( )?{}、循环 ^{}、模式匹配 #{} [P0][8SP]
- B3 三符号赋值（= 强、~= 弱、:= 拷贝）语法 [P0][5SP]
- B4 模块导入 >>/》》 与多文件工程入口 [P1][5SP]
- 验收：AST 快照覆盖，错误恢复不崩溃

### 模块C. 语义分析器 Semantic（C）
- C1 字面量类型与隐式转换矩阵（int→float、f32/f64）[P0][8SP]
- C2 类型系统：结构体字段、方法、接口约束、枚举匹配穷尽性 [P0][12SP]
- C3 三符号赋值语义：shared_ptr/weak_ptr/拷贝规则 [P0][10SP]
- C4 生命周期/逃逸/循环引用图检测 [P1][13SP]
- C5 模块/命名空间/可见性/重复定义 [P1][8SP]
- 验收：中文错误+修复建议（如 f64↔f32）

### 模块D. 代码生成 Codegen→C++（D）
- D1 三赋值 C++ 映射：=shared_ptr、~=weak_ptr、:=拷贝 [P0][12SP]
- D2 @/&/%/# 生成：struct/成员函数/接口/enum class [P0][15SP]
- D3 模式匹配 #{} 降级：switch/if 链（穷尽性防御）[P1][6SP]
- D4 跨平台控制台编码（clang++/g++）[P1][3SP]
- D5 语法糖：数组/元组/Option/Result/字符串连接 [P2][10SP]
- 验收：C++17 编译通过，关键样例运行正确

### 模块E. 标准库 stdlib（E）
- E1 Array/Map/String 工具 [P1][8SP]
- E2 Option/Result（错误传播/匹配）[P1][6SP]
- E3 数学/时间/IO 基础 [P2][6SP]
- 验收：示例仅依赖 stdlib 可运行

### 模块F. 工具链/CLI/包管理（F）
- F1 CLI：build/run/format/lint/test [P1][8SP]
- F2 格式化/Lint（最小规则）[P2][10SP]
- F3 包管理 demo：package.pgm fetch/lock [P3][12SP]
- 验收：run_tests.* 可调用 CLI；examples package demo 可运行

### 模块G. IDE/编辑器支持（G）
- G1 VSCode 扩展：高亮/基本诊断/运行 [P2][12SP]
- G2 LSP：Hover/Diagnostics/Go-to-definition [P3][20SP]
- 验收：VSCode 可用，配套文档

### 模块H. 测试与CI（H）
- H1 golden 测试集：Lexer/Parser/Semantic/Codegen（中英文、全角半角）[P0][10SP]
- H2 集成测试：tutorial/examples/tests + Windows/Linux CI [P1][10SP]
- H3 Fuzz 测试（Lexer/Parser）[P3][10SP]
- 验收：PR 自动跑；快照变更需批准

### 模块I. 文档与示例（I）
- I1 语言规范（中/英）、内存模型 [P1][8SP]
- I2 教程与示例升级（游戏、包管理、错误演示）[P1][6SP]
- I3 贡献指南/代码规范/分支策略 [P1][3SP]
- 验收：docs 站点构建、示例可复制运行

---

## 三、优先级与依赖
- P0（立即）：B1/B2/B3、C1/C2/C3、D1/D2、H1
- P1（下一迭代）：A2/A3、B4、C5、D3/D4、E1/E2、F1、I1/I2/I3、H2
- P2：A4、D5、E3、F2、G1
- P3：G2、H3、F3

---

## 四、工作量汇总（粗估）
- A:16、B:26、C:51、D:46、E:20、F:30、G:32、H:30、I:17 → 合计≈268SP
- 并行优化：约 4~8 周可达 Beta（多 AI 分工并行）

---

## 五、分工建议（多AI角色）
- Parser-Scout：B+A 协作
- Semantic-Guru：C
- Codegen-Engineer：D
- Stdlib-Builder：E
- Tooling-Smith：F
- IDE-Agent：G
- QA-Orchestrator：H
- Docs-Curator：I

---

## 六、里程碑
- M1（第1-2周）：P0 完成 → Alpha 内核可用，golden 测试就绪
- M2（第3-4周）：P1 完成 → CLI/stdlib/CI/docs 完备
- M3（第5-6周）：P2/P3 重点 → 格式化/LSP/更多库与稳定性

---

## 七、工作流规范
- 分支：feature/<module>-<task>
- PR 要求：关联测试+docs；CI 全绿
- 代码所有权：模块 Owner 必须 review
- 快照测试：Parser/Codegen 输出全量快照（防回归）
- 每日回归：tutorial/examples/tests 全量跑，产出《每日测试报告.md》

---

## 八、立即行动（Quick Wins）
- 修正：浮点字面量默认类型与 f64 显式声明兼容（C1）
- 打通：三符号赋值 AST+语义（B3/C3）
- 构建：30+ 中文“全角模式”golden 语料（H1）

---

如需我进一步：
- 创建每个模块的 issue/任务条目与验收清单
- 初始化 feature 分支模板与 PR 检查表
- 启动 M1 的第一批任务与测试脚手架

请在此基础上指示下一步优先项。