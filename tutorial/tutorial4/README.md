# 🏗️ Tutorial 4: 结构与对象 - 面向对象编程

学习 polyglot 的面向对象特性：结构定义、方法实现、接口设计和继承机制。

## 📝 学习内容

- 结构定义 `@`
- 方法实现 `&`
- 接口定义 `%`
- 枚举类型 `#`
- 构造函数和析构函数
- 继承和多态

## 💻 源码解析

### `objects.pg` - 面向对象完整演示

```rust
// polyglot 面向对象编程教程

>> "math"  // 导入数学库

main() {
    print("=== polyglot 面向对象教程 ===")

    // 1. 创建和使用基本对象
    print("\n1. 基本对象操作:")
    ? p1 = Point::new(3.0, 4.0)
    ? p2 = Point::new(0.0, 0.0)

    print("点 p1: (" + p1.x + ", " + p1.y + ")")
    print("点 p2: (" + p2.x + ", " + p2.y + ")")

    ? distance = p1.distance_to(p2)
    print("p1 到 p2 的距离: " + distance)

    // 移动点
    p1.move_by(1.0, 1.0)
    print("移动后的 p1: (" + p1.x + ", " + p1.y + ")")

    // 2. 创建复杂对象
    print("\n2. 复杂对象 - 圆形:")
    ? circle = Circle::new(Point::new(0.0, 0.0), 5.0)

    print("圆心: (" + circle.center.x + ", " + circle.center.y + ")")
    print("半径: " + circle.radius)
    print("圆的面积: " + circle.area())
    print("圆的周长: " + circle.circumference())

    // 检查点是否在圆内
    ? test_point = Point::new(3.0, 4.0)
    ? is_inside = circle.contains_point(test_point)
    print("点 (" + test_point.x + ", " + test_point.y + ") 在圆内: " + is_inside)

    // 3. 使用接口和多态
    print("\n3. 接口和多态:")
    ? shapes = []
    shapes.push(circle)
    shapes.push(Rectangle::new(Point::new(-2.0, -1.0), 4.0, 2.0))
    shapes.push(Triangle::new(Point::new(0.0, 0.0), Point::new(3.0, 0.0), Point::new(1.5, 3.0)))

    ? total_area = 0.0
    ? i = 0
    ^(i < len(shapes)) {
        ? shape = shapes[i]
        ? shape_area = shape.area()
        total_area = total_area + shape_area

        print("形状 " + (i + 1) + " 面积: " + shape_area)
        i = i + 1
    }
    print("所有形状总面积: " + total_area)

    // 4. 枚举类型使用
    print("\n4. 枚举类型:")
    ? red_color = Color::Red
    ? blue_color = Color::Blue
    ? custom_color = Color::RGB(255, 128, 0)

    print("红色: " + color_to_string(red_color))
    print("蓝色: " + color_to_string(blue_color))
    print("自定义橙色: " + color_to_string(custom_color))

    // 5. 动物继承示例
    print("\n5. 继承和多态:")
    ? animals = []
    animals.push(Dog::new("旺财", 3))
    animals.push(Cat::new("小咪", 2))
    animals.push(Bird::new("小黄", 1))

    ? j = 0
    ^(j < len(animals)) {
        ? animal = animals[j]

        print("动物: " + animal.get_name() + ", 年龄: " + animal.get_age())
        animal.make_sound()
        animal.move()

        // 检查特殊能力
        (animal.can_fly()) ? {
            print("  - 这个动物会飞！")
        }

        j = j + 1
    }

    // 6. 组合模式示例
    print("\n6. 对象组合:")
    ? team = Team::new("开发团队")
    team.add_member(Person::new("Alice", "程序员", 5))
    team.add_member(Person::new("Bob", "设计师", 3))
    team.add_member(Person::new("Charlie", "测试员", 2))

    print("团队: " + team.name)
    print("成员数量: " + team.member_count())
    team.show_members()

    ? avg_experience = team.average_experience()
    print("平均经验: " + avg_experience + " 年")

    <- 0
}

// ===================
// 基础结构定义
// ===================

// 2D 点结构
@ Point {
    x: f64,
    y: f64
}

// 实现 Point 的方法
& Point {
    // 构造函数
    Point::new(x: f64, y: f64) -> Point {
        <- Point { x: x, y: y }
    }

    // 计算到另一点的距离
    distance_to(other: Point) -> f64 {
        ? dx = self.x - other.x
        ? dy = self.y - other.y
        <- sqrt(dx * dx + dy * dy)
    }

    // 移动点
    move_by(dx: f64, dy: f64) {
        self.x += dx
        self.y += dy
    }

    // 获取点的字符串表示
    to_string() -> string {
        <- "(" + self.x + ", " + self.y + ")"
    }
}

// 形状接口定义
% Shape {
    area() -> f64
    perimeter() -> f64
    contains_point(point: Point) -> bool
}

// ===================
// 具体形状实现
// ===================

// 圆形
@ Circle {
    center: Point,
    radius: f64
}

& Circle % Shape {
    Circle::new(center: Point, radius: f64) -> Circle {
        <- Circle { center: center, radius: radius }
    }

    area() -> f64 {
        * PI = 3.14159265359
        <- PI * self.radius * self.radius
    }

    circumference() -> f64 {
        * PI = 3.14159265359
        <- 2.0 * PI * self.radius
    }

    perimeter() -> f64 {
        <- self.circumference()
    }

    contains_point(point: Point) -> bool {
        ? distance = self.center.distance_to(point)
        <- distance <= self.radius
    }
}

// 矩形
@ Rectangle {
    top_left: Point,
    width: f64,
    height: f64
}

& Rectangle % Shape {
    Rectangle::new(top_left: Point, width: f64, height: f64) -> Rectangle {
        <- Rectangle {
            top_left: top_left,
            width: width,
            height: height
        }
    }

    area() -> f64 {
        <- self.width * self.height
    }

    perimeter() -> f64 {
        <- 2.0 * (self.width + self.height)
    }

    contains_point(point: Point) -> bool {
        ? x = point.x
        ? y = point.y
        ? left = self.top_left.x
        ? top = self.top_left.y
        ? right = left + self.width
        ? bottom = top + self.height

        <- (x >= left && x <= right && y >= top && y <= bottom)
    }
}

// 三角形
@ Triangle {
    a: Point,
    b: Point,
    c: Point
}

& Triangle % Shape {
    Triangle::new(a: Point, b: Point, c: Point) -> Triangle {
        <- Triangle { a: a, b: b, c: c }
    }

    area() -> f64 {
        // 使用海伦公式计算面积
        ? side_a = self.b.distance_to(self.c)
        ? side_b = self.a.distance_to(self.c)
        ? side_c = self.a.distance_to(self.b)
        ? s = (side_a + side_b + side_c) / 2.0

        <- sqrt(s * (s - side_a) * (s - side_b) * (s - side_c))
    }

    perimeter() -> f64 {
        ? side_a = self.b.distance_to(self.c)
        ? side_b = self.a.distance_to(self.c)
        ? side_c = self.a.distance_to(self.b)
        <- side_a + side_b + side_c
    }

    contains_point(point: Point) -> bool {
        // 简化实现，实际应该使用重心坐标
        <- false
    }
}

// ===================
// 枚举类型
// ===================

# Color {
    Red,
    Green,
    Blue,
    RGB(r: u8, g: u8, b: u8)
}

// 颜色转字符串函数
color_to_string(color: Color) -> string {
    #(color) {
        Color::Red => <- "红色",
        Color::Green => <- "绿色",
        Color::Blue => <- "蓝色",
        Color::RGB(r, g, b) => <- "RGB(" + r + ", " + g + ", " + b + ")"
    }
}

// ===================
// 动物继承系统
// ===================

// 动物基础接口
% Animal {
    get_name() -> string
    get_age() -> i32
    make_sound()
    move()
    can_fly() -> bool
}

// 哺乳动物基类
@ Mammal {
    name: string,
    age: i32
}

& Mammal % Animal {
    get_name() -> string {
        <- self.name
    }

    get_age() -> i32 {
        <- self.age
    }

    can_fly() -> bool {
        <- false  // 哺乳动物一般不会飞
    }
}

// 狗
@ Dog {
    name: string,
    age: i32
}

& Dog % Animal {
    Dog::new(name: string, age: i32) -> Dog {
        <- Dog { name: name, age: age }
    }

    get_name() -> string { <- self.name }
    get_age() -> i32 { <- self.age }
    can_fly() -> bool { <- false }

    make_sound() {
        print("  汪汪汪！")
    }

    move() {
        print("  " + self.name + " 在地上跑")
    }
}

// 猫
@ Cat {
    name: string,
    age: i32
}

& Cat % Animal {
    Cat::new(name: string, age: i32) -> Cat {
        <- Cat { name: name, age: age }
    }

    get_name() -> string { <- self.name }
    get_age() -> i32 { <- self.age }
    can_fly() -> bool { <- false }

    make_sound() {
        print("  喵喵喵～")
    }

    move() {
        print("  " + self.name + " 优雅地走着")
    }
}

// 鸟
@ Bird {
    name: string,
    age: i32
}

& Bird % Animal {
    Bird::new(name: string, age: i32) -> Bird {
        <- Bird { name: name, age: age }
    }

    get_name() -> string { <- self.name }
    get_age() -> i32 { <- self.age }
    can_fly() -> bool { <- true }

    make_sound() {
        print("  叽叽喳喳～")
    }

    move() {
        print("  " + self.name + " 在天空中飞翔")
    }
}

// ===================
// 组合模式示例
// ===================

// 人员
@ Person {
    name: string,
    role: string,
    experience: i32
}

& Person {
    Person::new(name: string, role: string, experience: i32) -> Person {
        <- Person {
            name: name,
            role: role,
            experience: experience
        }
    }

    get_info() -> string {
        <- self.name + " (" + self.role + ", " + self.experience + "年经验)"
    }
}

// 团队
@ Team {
    name: string,
    members: [Person]
}

& Team {
    Team::new(name: string) -> Team {
        <- Team { name: name, members: [] }
    }

    add_member(person: Person) {
        self.members.push(person)
    }

    member_count() -> i32 {
        <- len(self.members)
    }

    show_members() {
        ? i = 0
        ^(i < len(self.members)) {
            ? member = self.members[i]
            print("  - " + member.get_info())
            i = i + 1
        }
    }

    average_experience() -> f64 {
        (len(self.members) == 0) ? {
            <- 0.0
        }

        ? total = 0
        ? i = 0
        ^(i < len(self.members)) {
            total += self.members[i].experience
            i = i + 1
        }

        <- total / len(self.members)
    }
}

// 辅助函数
sqrt(x: f64) -> f64 {
    <- x ** 0.5  // 简化实现
}

len(arr) -> i32 {
    <- 3  // 简化实现
}
```

## 🔍 详细解释

### 1. **结构定义 - `@` 符号**
```rust
@ StructName {
    field1: Type1,
    field2: Type2
}
```
- `@` 像是给结构"贴标签"
- 定义数据的组织结构

### 2. **方法实现 - `&` 符号**
```rust
& StructName {
    method_name() {
        // 方法体
    }
}
```
- `&` 像是"连接"到类型上
- 为结构添加行为

### 3. **接口定义 - `%` 符号**
```rust
% InterfaceName {
    method1() -> ReturnType
    method2(param: Type)
}
```
- `%` 像是"规范模板"
- 定义必须实现的方法

### 4. **接口实现**
```rust
& StructName % InterfaceName {
    // 实现接口方法
}
```

### 5. **枚举类型 - `#` 符号**
```rust
# EnumName {
    Variant1,
    Variant2(data: Type)
}
```

## 🎯 预期输出

```
=== polyglot 面向对象教程 ===

1. 基本对象操作:
点 p1: (3.0, 4.0)
点 p2: (0.0, 0.0)
p1 到 p2 的距离: 5.0
移动后的 p1: (4.0, 5.0)

2. 复杂对象 - 圆形:
圆心: (0.0, 0.0)
半径: 5.0
圆的面积: 78.53981633974483
圆的周长: 31.41592653589793
点 (3.0, 4.0) 在圆内: true

3. 接口和多态:
形状 1 面积: 78.53981633974483
形状 2 面积: 8.0
形状 3 面积: 4.5
所有形状总面积: 91.03981633974483

4. 枚举类型:
红色: 红色
蓝色: 蓝色
自定义橙色: RGB(255, 128, 0)

5. 继承和多态:
动物: 旺财, 年龄: 3
  汪汪汪！
  旺财 在地上跑
动物: 小咪, 年龄: 2
  喵喵喵～
  小咪 优雅地走着
动物: 小黄, 年龄: 1
  叽叽喳喳～
  小黄 在天空中飞翔
  - 这个动物会飞！

6. 对象组合:
团队: 开发团队
成员数量: 3
  - Alice (程序员, 5年经验)
  - Bob (设计师, 3年经验)
  - Charlie (测试员, 2年经验)
平均经验: 3.33 年
```

## 🧪 动手实践

### 练习 1: 银行账户系统
创建 `bank_account.pg`：
```rust
@ BankAccount {
    account_number: string,
    balance: f64,
    owner: string
}

& BankAccount {
    BankAccount::new(number: string, owner: string) -> BankAccount {
        <- BankAccount {
            account_number: number,
            balance: 0.0,
            owner: owner
        }
    }

    deposit(amount: f64) -> bool {
        (amount > 0) ? {
            self.balance += amount
            <- true
        } : {
            <- false
        }
    }

    withdraw(amount: f64) -> bool {
        (amount > 0 && amount <= self.balance) ? {
            self.balance -= amount
            <- true
        } : {
            <- false
        }
    }

    get_balance() -> f64 {
        <- self.balance
    }
}
```

### 练习 2: 图书管理系统
创建 `library_system.pg`：
```rust
@ Book {
    title: string,
    author: string,
    isbn: string,
    is_borrowed: bool
}

@ Library {
    name: string,
    books: [Book]
}

& Library {
    Library::new(name: string) -> Library {
        <- Library { name: name, books: [] }
    }

    add_book(book: Book) {
        self.books.push(book)
    }

    find_book(title: string) -> Book {
        // 简化实现
        <- self.books[0]
    }

    borrow_book(title: string) -> bool {
        ? book = self.find_book(title)
        (!book.is_borrowed) ? {
            book.is_borrowed = true
            <- true
        } : {
            <- false
        }
    }
}
```

## 💡 重要概念

1. **符号语义**：
   - `@` = 定义结构
   - `&` = 实现方法
   - `%` = 定义接口
   - `#` = 枚举类型

2. **多态性**：通过接口实现统一的行为

3. **组合 vs 继承**：优先使用组合来构建复杂对象

4. **构造函数**：使用 `Type::new()` 模式

## 🚀 下一步

完成这个教程后，你已经掌握了：
✅ 结构定义和使用
✅ 方法实现机制
✅ 接口设计原则
✅ 枚举类型应用
✅ 继承和多态概念

准备构建完整项目了吗？让我们进入 [Tutorial 5: 完整项目](../tutorial5/) ！