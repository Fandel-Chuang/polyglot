# 🚀 Tutorial 1: Hello World - 基础入门

欢迎来到 polyglot 的第一个教程！我们将从最简单的 "Hello World" 开始，学习基本语法。

## 📝 学习内容

- 基本输出函数
- 变量声明 `?`
- 字符串操作
- 简单数学运算
- 注释语法

## 💻 源码解析

### 📁 本教程包含的示例文件

- **`hello.pg`** - 你的第一个 polyglot 程序（基础入门）
- **`basic_example.pg`** - 结构体和方法的基本示例
- **`error_example.pg`** - 错误示例（用于测试编译器错误处理）

---

### `hello.pg` - 你的第一个 polyglot 程序

```rust
// 这是注释：我的第一个 polyglot 程序！

main() {
    // 1. 基本输出 - 最简单的 Hello World
    print("Hello, polyglot!")

    // 2. 变量声明 - 使用 ? 符号"询问"系统要一个变量
    ? greeting = "Welcome to polyglot"
    ? language = "polyglot"
    ? version = 1.0

    // 3. 输出变量
    print(greeting)
    print("你正在学习: " + language)
    print("版本: " + version)

    // 4. 基本数学运算
    ? x = 10
    ? y = 20
    ? sum = x + y
    ? product = x * y

    print("10 + 20 = " + sum)
    print("10 × 20 = " + product)

    // 5. 字符串拼接展示
    ? name = "polyglot 学习者"
    ? message = "你好，" + name + "！欢迎来到符号化编程的世界！"
    print(message)

    // 6. 更多数学运算
    ? pi = 3.14159
    ? radius = 5.0
    ? area = pi * radius * radius
    print("半径为 " + radius + " 的圆的面积是: " + area)

    // 7. 布尔值
    ? is_awesome = true
    ? is_difficult = false
    print("polyglot 很棒吗？" + is_awesome)
    print("polyglot 很难吗？" + is_difficult)

    // 8. 函数返回值
    <- 0
}
```

## 🔍 详细解释

### 1. **注释语法**
```rust
// 单行注释 - 用来解释代码
/* 多行注释
   可以跨越多行 */
```

### 2. **变量声明 - `?` 符号**
```rust
? variable_name = value
```
- `?` 像是在"询问"系统：能给我一个变量吗？
- 直观易记，比 `let` 或 `var` 更简洁

### 3. **基本数据类型**
```rust
? name = "字符串"        // 字符串类型
? age = 25              // 整数类型
? height = 1.75         // 浮点数类型
? is_student = true     // 布尔类型
```

### 4. **函数定义**
```rust
main() {
    // 函数体
    <- 0    // 返回值，<- 像是"返回"的箭头
}
```

### 5. **输出函数**
```rust
print("文本")           // 输出文本
print(variable)         // 输出变量
print("文本" + variable) // 输出拼接结果
```

## 🎯 运行结果

当你运行 `hello.pg` 时，应该看到：

```
Hello, polyglot!
Welcome to polyglot
你正在学习: polyglot
版本: 1.0
10 + 20 = 30
10 × 20 = 200
你好，polyglot 学习者！欢迎来到符号化编程的世界！
半径为 5.0 的圆的面积是: 78.53975
polyglot 很棒吗？true
polyglot 很难吗？false
```

## 🧪 动手实践

### 练习 1: 自我介绍
创建 `my_intro.pg`，包含：
```rust
main() {
    ? my_name = "你的名字"
    ? my_age = 你的年龄
    ? my_hobby = "你的爱好"

    print("大家好！我是 " + my_name)
    print("我今年 " + my_age + " 岁")
    print("我喜欢 " + my_hobby)

    <- 0
}
```

### 练习 2: 简单计算器
创建 `calculator.pg`：
```rust
main() {
    ? a = 15
    ? b = 7

    print(a + " + " + b + " = " + (a + b))
    print(a + " - " + b + " = " + (a - b))
    print(a + " × " + b + " = " + (a * b))
    print(a + " ÷ " + b + " = " + (a / b))

    <- 0
}
```

## 💡 重要提示

1. **符号含义**：`?` 就像在问"能给我一个变量吗？"，很直观！
2. **返回箭头**：`<-` 看起来像箭头指向左边，表示"返回"
3. **字符串拼接**：使用 `+` 连接字符串和数字
4. **函数定义**：直接写函数名，不需要 `func` 关键字

## 🚀 下一步

完成这个教程后，你已经掌握了：
✅ 基本输出
✅ 变量声明
✅ 数据类型
✅ 简单运算
✅ 函数结构

准备好了吗？让我们进入 [Tutorial 2: 数据类型与运算](../tutorial2/) 学习更多内容！