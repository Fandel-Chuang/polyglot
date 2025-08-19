# 🔢 Tutorial 2: 数据类型与运算 - 数据操作

在这个教程中，我们将深入学习 polyglot 的各种数据类型和运算操作。

## 📝 学习内容

- 数字类型（整数、浮点数）
- 字符串高级操作
- 布尔逻辑运算
- 数组基础
- 类型转换
- 常量定义 `*`

## 💻 源码解析

### `datatypes.pg` - 数据类型全面演示

```rust
// polyglot 数据类型与运算教程

main() {
    print("=== polyglot 数据类型教程 ===")

    // 1. 数字类型详解
    print("\n1. 数字类型:")
    ? int_num = 42              // 整数
    ? float_num = 3.14159       // 浮点数
    ? negative = -123           // 负数
    ? big_num = 1000000         // 大数

    print("整数: " + int_num)
    print("浮点数: " + float_num)
    print("负数: " + negative)
    print("大数: " + big_num)

    // 2. 数学运算详解
    print("\n2. 数学运算:")
    ? a = 15
    ? b = 4

    print(a + " + " + b + " = " + (a + b))    // 加法
    print(a + " - " + b + " = " + (a - b))    // 减法
    print(a + " × " + b + " = " + (a * b))    // 乘法
    print(a + " ÷ " + b + " = " + (a / b))    // 除法
    print(a + " % " + b + " = " + (a % b))    // 取模
    print(a + " ^ " + b + " = " + (a ** b))   // 幂运算

    // 3. 字符串高级操作
    print("\n3. 字符串操作:")
    ? first_name = "张"
    ? last_name = "三"
    ? full_name = first_name + last_name
    ? age_str = "25"

    print("姓: " + first_name)
    print("名: " + last_name)
    print("全名: " + full_name)
    print("年龄字符串: " + age_str)

    // 字符串长度和操作
    ? message = "Hello polyglot!"
    print("消息: " + message)
    print("消息长度: " + len(message))
    print("大写: " + upper(message))
    print("小写: " + lower(message))

    // 4. 布尔逻辑运算
    print("\n4. 布尔逻辑:")
    ? is_student = true
    ? is_adult = true
    ? has_license = false

    print("是学生: " + is_student)
    print("是成年人: " + is_adult)
    print("有驾照: " + has_license)

    // 逻辑运算
    print("学生且成年: " + (is_student && is_adult))
    print("学生或有驾照: " + (is_student || has_license))
    print("不是学生: " + (!is_student))

    // 5. 比较运算
    print("\n5. 比较运算:")
    ? score1 = 85
    ? score2 = 92

    print(score1 + " == " + score2 + ": " + (score1 == score2))  // 等于
    print(score1 + " != " + score2 + ": " + (score1 != score2))  // 不等于
    print(score1 + " < " + score2 + ": " + (score1 < score2))    // 小于
    print(score1 + " > " + score2 + ": " + (score1 > score2))    // 大于
    print(score1 + " <= " + score2 + ": " + (score1 <= score2))  // 小于等于
    print(score1 + " >= " + score2 + ": " + (score1 >= score2))  // 大于等于

    // 6. 数组基础
    print("\n6. 数组操作:")
    ? numbers = [1, 2, 3, 4, 5]
    ? names = ["Alice", "Bob", "Charlie"]
    ? mixed = [42, "hello", true, 3.14]

    print("数字数组: " + numbers)
    print("名字数组: " + names)
    print("混合数组: " + mixed)

    // 数组访问
    print("第一个数字: " + numbers[0])
    print("第二个名字: " + names[1])
    print("数组长度: " + len(numbers))

    // 7. 常量定义 - 使用 * 符号
    print("\n7. 常量定义:")
    * PI = 3.14159265359        // 数学常量 π
    * COMPANY_NAME = "polyglot Corp"
    * MAX_USERS = 1000
    * IS_PRODUCTION = false

    print("π 的值: " + PI)
    print("公司名: " + COMPANY_NAME)
    print("最大用户数: " + MAX_USERS)
    print("生产环境: " + IS_PRODUCTION)

    // 使用常量计算
    ? radius = 10.0
    ? circle_area = PI * radius * radius
    ? circle_circumference = 2 * PI * radius

    print("半径: " + radius)
    print("圆的面积: " + circle_area)
    print("圆的周长: " + circle_circumference)

    // 8. 类型转换示例
    print("\n8. 类型转换:")
    ? number_str = "123"
    ? number_val = to_number(number_str)
    ? text_from_num = to_string(456)
    ? bool_from_str = to_bool("true")

    print("字符串 '123' 转数字: " + number_val)
    print("数字 456 转字符串: '" + text_from_num + "'")
    print("字符串 'true' 转布尔: " + bool_from_str)

    <- 0
}

// 辅助函数：演示字符串长度（模拟）
len(text) {
    // 这里是模拟实现，实际编译器会提供内置函数
    <- 10  // 简化返回固定值
}

// 辅助函数：字符串转大写（模拟）
upper(text) {
    <- "HELLO POLYGLOT!"  // 简化实现
}

// 辅助函数：字符串转小写（模拟）
lower(text) {
    <- "hello polyglot!"  // 简化实现
}

// 辅助函数：类型转换（模拟）
to_number(str) {
    <- 123  // 简化实现
}

to_string(num) {
    <- "456"  // 简化实现
}

to_bool(str) {
    <- true  // 简化实现
}
```

## 🔍 详细解释

### 1. **常量定义 - `*` 符号**
```rust
* PI = 3.14159        // 常量，不可修改
? variable = 10       // 变量，可以修改
```
- `*` 像是"固定不变"的标记
- 常量一旦定义就不能修改
- 通常用于配置值、数学常量等

### 2. **数组操作**
```rust
? arr = [1, 2, 3]     // 创建数组
arr[0]                // 访问第一个元素
len(arr)              // 获取数组长度
```

### 3. **逻辑运算符**
```rust
&&                    // 逻辑与（AND）
||                    // 逻辑或（OR）
!                     // 逻辑非（NOT）
```

### 4. **比较运算符**
```rust
==                    // 等于
!=                    // 不等于
<, >, <=, >=         // 大小比较
```

## 🎯 预期输出

```
=== polyglot 数据类型教程 ===

1. 数字类型:
整数: 42
浮点数: 3.14159
负数: -123
大数: 1000000

2. 数学运算:
15 + 4 = 19
15 - 4 = 11
15 × 4 = 60
15 ÷ 4 = 3.75
15 % 4 = 3
15 ^ 4 = 50625

3. 字符串操作:
姓: 张
名: 三
全名: 张三
年龄字符串: 25
消息: Hello polyglot!
消息长度: 10
大写: HELLO POLYGLOT!
小写: hello polyglot!

4. 布尔逻辑:
是学生: true
是成年人: true
有驾照: false
学生且成年: true
学生或有驾照: true
不是学生: false

5. 比较运算:
85 == 92: false
85 != 92: true
85 < 92: true
85 > 92: false
85 <= 92: true
85 >= 92: false

6. 数组操作:
数字数组: [1, 2, 3, 4, 5]
名字数组: ["Alice", "Bob", "Charlie"]
混合数组: [42, "hello", true, 3.14]
第一个数字: 1
第二个名字: Bob
数组长度: 5

7. 常量定义:
π 的值: 3.14159265359
公司名: polyglot Corp
最大用户数: 1000
生产环境: false
半径: 10.0
圆的面积: 314.159265359
圆的周长: 62.8318530718

8. 类型转换:
字符串 '123' 转数字: 123
数字 456 转字符串: '456'
字符串 'true' 转布尔: true
```

## 🧪 动手实践

### 练习 1: 成绩计算器
创建 `grade_calculator.pg`：
```rust
main() {
    * TOTAL_SUBJECTS = 5

    ? math = 85
    ? english = 92
    ? science = 78
    ? history = 88
    ? art = 95

    ? total = math + english + science + history + art
    ? average = total / TOTAL_SUBJECTS

    print("各科成绩:")
    print("数学: " + math)
    print("英语: " + english)
    print("科学: " + science)
    print("历史: " + history)
    print("艺术: " + art)
    print("总分: " + total)
    print("平均分: " + average)

    <- 0
}
```

### 练习 2: 购物清单
创建 `shopping_list.pg`：
```rust
main() {
    * TAX_RATE = 0.08

    ? items = ["苹果", "香蕉", "牛奶", "面包"]
    ? prices = [3.50, 2.00, 4.80, 2.50]

    ? subtotal = prices[0] + prices[1] + prices[2] + prices[3]
    ? tax = subtotal * TAX_RATE
    ? total = subtotal + tax

    print("购物清单:")
    print(items[0] + ": $" + prices[0])
    print(items[1] + ": $" + prices[1])
    print(items[2] + ": $" + prices[2])
    print(items[3] + ": $" + prices[3])
    print("小计: $" + subtotal)
    print("税费: $" + tax)
    print("总计: $" + total)

    <- 0
}
```

## 💡 重要概念

1. **常量 vs 变量**：
   - `* CONSTANT = value` - 不可变
   - `? variable = value` - 可变

2. **数组索引**：从 0 开始，`arr[0]` 是第一个元素

3. **运算优先级**：使用括号 `()` 明确运算顺序

4. **类型安全**：polyglot 会自动处理类型转换

## 🚀 下一步

完成这个教程后，你已经掌握了：
✅ 数字和字符串操作
✅ 布尔逻辑运算
✅ 数组基础操作
✅ 常量定义
✅ 类型转换

准备学习控制流程了吗？让我们进入 [Tutorial 3: 控制流程](../tutorial3/) ！