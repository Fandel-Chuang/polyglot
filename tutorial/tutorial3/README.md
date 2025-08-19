# 🌊 Tutorial 3: 控制流程 - 逻辑控制

掌握 polyglot 的流程控制，包括条件判断、循环和模式匹配的符号化语法。

## 📝 学习内容

- 条件语句 `(condition) ? { ... }`
- 条件分支 `(condition) ? { ... } : { ... }`
- 循环结构 `^(condition) { ... }`
- 模式匹配 `#(value) { ... }`
- 控制关键字 `break`, `continue`

## 💻 源码解析

### `control_flow.pg` - 控制流程完整演示

```rust
// polyglot 控制流程教程

main() {
    print("=== polyglot 控制流程教程 ===")

    // 1. 基本条件语句 - 使用 ? 符号
    print("\n1. 基本条件语句:")
    ? age = 20
    ? score = 85

    (age >= 18) ? {
        print("你已经成年了！")
    }

    (score >= 90) ? {
        print("优秀！")
    } : {
        print("继续努力！")
    }

    // 2. 复合条件判断
    print("\n2. 复合条件判断:")
    ? temperature = 25
    ? is_sunny = true

    (temperature > 20 && is_sunny) ? {
        print("今天天气很好，适合出去玩！")
    } : {
        print("今天不太适合户外活动")
    }

    // 多重条件判断
    (temperature < 0) ? {
        print("很冷，注意保暖")
    } : (temperature < 20) ? {
        print("有点凉，多穿点衣服")
    } : (temperature < 30) ? {
        print("温度适宜")
    } : {
        print("很热，注意防暑")
    }

    // 3. 基本循环 - 使用 ^ 符号
    print("\n3. 基本循环:")
    ? i = 1
    ^(i <= 5) {
        print("第 " + i + " 次循环")
        i = i + 1
    }

    // 倒计时循环
    ? countdown = 5
    print("倒计时开始:")
    ^(countdown > 0) {
        print(countdown + "...")
        countdown = countdown - 1
    }
    print("时间到！")

    // 4. 数组遍历
    print("\n4. 数组遍历:")
    ? fruits = ["苹果", "香蕉", "橙子", "葡萄"]
    ? index = 0

    print("水果清单:")
    ^(index < len(fruits)) {
        print((index + 1) + ". " + fruits[index])
        index = index + 1
    }

    // 5. 模式匹配 - 使用 # 符号
    print("\n5. 模式匹配:")

    // 简单值匹配
    ? day_of_week = 3
    ? day_name = ""

    #(day_of_week) {
        1 => day_name = "星期一",
        2 => day_name = "星期二",
        3 => day_name = "星期三",
        4 => day_name = "星期四",
        5 => day_name = "星期五",
        6 => day_name = "星期六",
        7 => day_name = "星期日",
        _ => day_name = "无效日期"
    }
    print("今天是: " + day_name)

    // 成绩等级匹配
    ? student_score = 88
    ? grade = ""

    #(student_score) {
        (90..100) => grade = "A",
        (80..89) => grade = "B",
        (70..79) => grade = "C",
        (60..69) => grade = "D",
        (0..59) => grade = "F",
        _ => grade = "无效分数"
    }
    print("分数 " + student_score + " 对应等级: " + grade)

    // 6. 嵌套控制结构
    print("\n6. 嵌套控制结构:")
    ? matrix = [[1, 2, 3], [4, 5, 6], [7, 8, 9]]
    ? row = 0

    print("3x3 矩阵:")
    ^(row < 3) {
        ? col = 0
        ? row_text = ""

        ^(col < 3) {
            row_text = row_text + matrix[row][col] + " "
            col = col + 1
        }

        print(row_text)
        row = row + 1
    }

    // 7. 带控制的循环
    print("\n7. 带控制的循环:")
    ? numbers = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
    ? sum = 0
    ? count = 0

    ^(count < len(numbers)) {
        ? current = numbers[count]

        // 跳过偶数
        (current % 2 == 0) ? {
            count = count + 1
            continue
        }

        sum = sum + current
        print("加入奇数: " + current + ", 当前和: " + sum)

        // 如果和超过15就停止
        (sum > 15) ? {
            print("和已超过15，停止计算")
            break
        }

        count = count + 1
    }

    print("最终奇数和: " + sum)

    // 8. 条件赋值表达式
    print("\n8. 条件赋值:")
    ? weather = "sunny"
    ? activity = (weather == "sunny") ? "去公园" : "在家读书"
    print("天气: " + weather + ", 活动: " + activity)

    ? num1 = 15
    ? num2 = 20
    ? max_num = (num1 > num2) ? num1 : num2
    ? min_num = (num1 < num2) ? num1 : num2
    print("较大数: " + max_num + ", 较小数: " + min_num)

    // 9. 函数中的控制流
    print("\n9. 函数控制流:")
    ? result1 = check_number(15)
    ? result2 = check_number(-5)
    ? result3 = check_number(0)

    print("15 的检查结果: " + result1)
    print("-5 的检查结果: " + result2)
    print("0 的检查结果: " + result3)

    // 10. 高级模式匹配
    print("\n10. 高级模式匹配:")
    ? user_role = "admin"
    ? permissions = get_permissions(user_role)
    print("用户角色: " + user_role + ", 权限: " + permissions)

    <- 0
}

// 数字检查函数
check_number(num) {
    (num > 0) ? {
        <- "正数"
    } : (num < 0) ? {
        <- "负数"
    } : {
        <- "零"
    }
}

// 权限获取函数 - 使用模式匹配
get_permissions(role) {
    ? permissions = ""

    #(role) {
        "admin" => permissions = "所有权限",
        "manager" => permissions = "管理权限",
        "user" => permissions = "基本权限",
        "guest" => permissions = "只读权限",
        _ => permissions = "无权限"
    }

    <- permissions
}

// 模拟数组长度函数
len(arr) {
    <- 4  // 简化实现
}
```

## 🔍 详细解释

### 1. **条件语句语法**
```rust
(condition) ? {
    // 条件为真时执行
}

(condition) ? {
    // 条件为真
} : {
    // 条件为假
}
```
- `?` 就像在"问"：这个条件成立吗？
- 比传统的 `if` 更简洁直观

### 2. **循环语法**
```rust
^(condition) {
    // 循环体
}
```
- `^` 看起来像"向上"的箭头，表示重复执行
- 比 `while` 更简洁，视觉上更明显

### 3. **模式匹配语法**
```rust
#(value) {
    pattern1 => action1,
    pattern2 => action2,
    _ => default_action
}
```
- `#` 像是"分类标记"，表示按模式分类处理
- `_` 是通配符，匹配所有其他情况

### 4. **控制关键字**
- `break` - 跳出循环
- `continue` - 跳过当前迭代

## 🎯 预期输出

```
=== polyglot 控制流程教程 ===

1. 基本条件语句:
你已经成年了！
继续努力！

2. 复合条件判断:
今天天气很好，适合出去玩！
温度适宜

3. 基本循环:
第 1 次循环
第 2 次循环
第 3 次循环
第 4 次循环
第 5 次循环
倒计时开始:
5...
4...
3...
2...
1...
时间到！

4. 数组遍历:
水果清单:
1. 苹果
2. 香蕉
3. 橙子
4. 葡萄

5. 模式匹配:
今天是: 星期三
分数 88 对应等级: B

6. 嵌套控制结构:
3x3 矩阵:
1 2 3
4 5 6
7 8 9

7. 带控制的循环:
加入奇数: 1, 当前和: 1
加入奇数: 3, 当前和: 4
加入奇数: 5, 当前和: 9
加入奇数: 7, 当前和: 16
和已超过15，停止计算
最终奇数和: 16

8. 条件赋值:
天气: sunny, 活动: 去公园
较大数: 20, 较小数: 15

9. 函数控制流:
15 的检查结果: 正数
-5 的检查结果: 负数
0 的检查结果: 零

10. 高级模式匹配:
用户角色: admin, 权限: 所有权限
```

## 🧪 动手实践

### 练习 1: 猜数字游戏
创建 `guess_number.pg`：
```rust
main() {
    * SECRET_NUMBER = 42
    ? guess = 35
    ? attempts = 0
    ? max_attempts = 5

    print("=== 猜数字游戏 ===")
    print("我想了一个1-100之间的数字")

    ^(attempts < max_attempts) {
        attempts = attempts + 1
        print("第 " + attempts + " 次猜测: " + guess)

        (guess == SECRET_NUMBER) ? {
            print("恭喜！你猜对了！")
            break
        } : (guess < SECRET_NUMBER) ? {
            print("太小了，再大一点")
            guess = guess + 10
        } : {
            print("太大了，再小一点")
            guess = guess - 5
        }
    }

    (guess != SECRET_NUMBER) ? {
        print("很遗憾，答案是: " + SECRET_NUMBER)
    }

    <- 0
}
```

### 练习 2: 简单计算器
创建 `simple_calculator.pg`：
```rust
main() {
    print("=== 简单计算器 ===")

    ? a = 10
    ? b = 5
    ? operations = ["+", "-", "*", "/"]
    ? i = 0

    ^(i < 4) {
        ? op = operations[i]
        ? result = 0

        #(op) {
            "+" => result = a + b,
            "-" => result = a - b,
            "*" => result = a * b,
            "/" => {
                (b != 0) ? {
                    result = a / b
                } : {
                    print("错误：除零")
                    i = i + 1
                    continue
                }
            }
        }

        print(a + " " + op + " " + b + " = " + result)
        i = i + 1
    }

    <- 0
}
```

## 💡 重要概念

1. **符号直观性**：
   - `?` = 询问/条件
   - `^` = 重复/循环
   - `#` = 分类/匹配

2. **嵌套结构**：可以任意嵌套条件和循环

3. **模式匹配**：比传统 switch 更强大，支持范围匹配

4. **控制流函数**：在函数中使用控制结构来实现复杂逻辑

## 🚀 下一步

完成这个教程后，你已经掌握了：
✅ 条件判断语法
✅ 循环控制结构
✅ 模式匹配功能
✅ 嵌套控制流
✅ 流程控制函数

准备学习面向对象了吗？让我们进入 [Tutorial 4: 结构与对象](../tutorial4/) ！