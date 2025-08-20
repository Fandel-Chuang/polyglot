# 任务分配（GPT5，共同开发者）

角色定位
- 你（GPT5）：核心配套设施与质量保障负责人（Infra / QA / Tooling）
- 职责：CLI、测试/CI、基础库、字符串与数字字面量规则、跨平台兼容、协作规范落地
- Sprint 窗口：M1（P0范围）— 目标 7~10 天完成
- 分支前缀：feature/infra-*、feature/qa-*、feature/stdlib-*

对齐目标（M1 / P0 协同）
- 为 Sonnet 的核心语言特性提供稳定的构建/运行/测试入口与基础设施
- 建立 golden 测试与 CI，保障每个 PR 的质量门槛
- 初步 stdlib 能支撑示例运行

本轮任务清单（Owner: GPT5）
1) 词法/符号与字面量（A）
   - A2：字符串字面量：中文书名号“”+ 转义 + Unicode（5SP）
   - A3：数字字面量默认类型与后缀（1.0f 等），与 f32/f64 兼容策略（5SP）
   - 产出：规范文档 + Lexer 单测 + 语料用例
2) CLI 与工具链（F）
   - F1：CLI 子命令（build/run/test），与 run_tests.* 打通（8SP）
   - 产出：tools/cli/**；docs/QUICKSTART.md 更新
3) 测试与CI（H）
   - H1：golden 测试驱动器（Lexer/Parser/Semantic/Codegen）（10SP）
   - 集成 Windows（GitHub Actions）工作流（先最小集）（6SP, 并入H2）
   - 产出：.github/workflows/**、tests/golden/**、每日报告脚本
4) 标准库（E）
   - E1：Array/Map/String 最小实现（8SP）
   - E2：Option/Result（错误传播基础）（6SP）
   - 产出：stdlib/**；与 Codegen 的签名契约
5) 协作规范
   - CODEOWNERS、PR 模板、协作锁定流程（Issue 软锁 + 可选 LFS 硬锁）（4SP）
   - 产出：.github/**、docs/协作规范/**、人工智能协作/协作锁定流程.md

交付物
- CLI：tools/cli/** 可用（build/run/test）
- CI：GitHub Actions 最小流水线 + golden 测试
- stdlib：最小容器与 Option/Result
- 文档：字面量规范、CLI 使用文档、协作规范

验收标准
- PR 必须通过 golden 测试与 CI 才可合并
- CLI 可一键 build/run/test 示例与 tests/
- stdlib 支撑 tutorial/examples 中基础示例运行

与 Sonnet 的对接
- 字面量类型规则（A2/A3）做兜底；变更需提前同步
- CLI (F1) 与 Codegen 的输出/输入约定
- stdlib 与内建函数签名与 Codegen/语义分析一致
- golden 测试目录结构标准化，便于 Sonnet 新增用例

分支与提交流程
- 分支：feature/infra-cli, feature/qa-golden, feature/stdlib-core
- 必须通过 CI、由对应 CODEOWNERS 审核
- 合并策略：Squash

风险与回滚
- 风险：
  - 字面量规则调整会影响 Parser/Semantic → 先出规范再改实现
  - CI 不稳会阻塞合并 → 从最小工作流开始，逐步扩展
- 回滚：保留旧用例与旧脚本，行动开关控制新流程

每日节奏
- 上午：检查失败用例与 CI 状态，修复环境与脚本
- 下午：推进 CLI/stdlib 与 golden 测试
- 晚间：输出《每日测试报告.md》，与 Sonnet 同步接口变化