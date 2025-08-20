# 任务分配（Sonnet，主程序负责人）

角色定位
- 你（Sonnet）：主程序总负责人（Tech Lead / Integrator）
- 职责：核心语法/语义/代码生成推进，技术决策、接口契约、集成验收、风险兜底
- Sprint 窗口：M1（P0范围）— 目标 7~10 天完成
- 分支前缀：feature/core-*

里程碑目标（M1 / P0）
- 打通“结构体/方法/接口/枚举/控制流/三符号赋值”的 Parser + Semantic + Codegen 闭环
- 建立 golden 测试基线（中英文、全角半角）
- 集成输出 C++17 代码在 Win/Linux 下成功编译运行

本轮任务清单（Owner: Sonnet）
1) Parser（B）
   - B1：结构体 @、方法块 &、接口 %、枚举 # 的完整语法（8SP）
   - B2：控制流：条件 ( )?{}、循环 ^{}、模式匹配 #{} 的 AST（8SP）
   - B3：三符号赋值（= 强、~= 弱、:= 拷贝）语法（5SP）
2) Semantic（C）
   - C1：字面量类型与隐式转换矩阵（int→float、f32/f64 一致性）（8SP）
   - C2：类型系统：结构体字段、方法签名、接口约束、枚举穷尽性检查（12SP）
   - C3：三符号赋值语义：shared_ptr/weak_ptr/拷贝规则（10SP）
3) Codegen（D）
   - D1：三赋值 C++ 映射：=shared_ptr、~=weak_ptr、:=拷贝（12SP）
   - D2：@/&/%/# 生成：struct/成员函数/接口/enum class（15SP）
4) 测试（H）
   - H1：golden 测试（Lexer/Parser/Semantic/Codegen）覆盖 P0 场景（10SP）
5) 集成守护
   - 设计与维护接口契约（AST/类型系统/生成策略）
   - 审核 GPT5 PR 的契约一致性、性能与错误信息质量

交付物
- 实现代码：compiler/*（parser/semantic/codegen）
- 测试用例：tests/golden/**（按模块分层）
- 生成产物：可编译 C++ 源和可执行
- 接口文档：docs/dev/**（AST 结构、类型系统、三赋值规则、生成约定）

验收标准
- 关键示例：中英文/全角半角/混合场景全部通过
- 代码生成 C++17 在 g++/clang++ 编译通过
- 语义错误信息中文化且给修复建议（如 f64↔f32）
- golden 测试稳定，PR 变更需审批

依赖与对接（与 GPT5）
- 依赖：A2/A3（字符串/数字字面量规则）、B4（模块导入）基础设施
- 对接：
  - Codegen 模式匹配降级策略（D3）接口契约
  - 跨平台控制台编码（D4）编译宏与最低 C++ 标准
  - stdlib 类型与内建函数签名（E1/E2）
  - CLI（F1）对接 build/run/test 入口

分支与提交流程
- 分支：feature/core-parser, feature/core-semantic, feature/core-codegen, feature/core-tests
- 提交频率：小步快跑；每个模块独立 PR；必须通过 CI 和 Review
- 合并策略：Squash（保持历史简洁）

风险与回滚
- 风险：
  - AST/类型规则变动影响面大 → 先在 docs/dev/ 走 RFC，锁定后再实施
  - 三赋值语义实现复杂 → 先提供最小可用策略，扩展边界可迭代
- 回滚：保留旧快照与生成策略，启用开关逐步替换

每日节奏
- 上午：合并昨日日志 + 处理冲突 + CI 修复
- 下午：推进核心模块任务 + PR 审核
- 晚间：产出当日《每日测试报告.md》
