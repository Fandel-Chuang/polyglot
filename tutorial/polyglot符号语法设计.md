# polyglot 符号化语法设计

## 🎯 设计理念：用符号替代关键字

为了让语法更简洁、更易学，polyglot 使用符号替代传统的冗长关键字。

## 📝 符号语法对照表

| 功能 | 传统关键字 | polyglot符号 | 说明 |
|------|-----------|--------------|------|
| 导入模块 | `import` | `>>` | >> 像是"引入"的箭头 |
| 变量声明 | `let` | `?` | ? 像是"询问" |
| 条件赋值 | `if let` | `:=` | 海象运算符（仅限条件语句） |
| 函数定义 | `func` | *直接定义* | 函数名开头即函数定义 |
| 返回语句 | `return` | `<-` | <- 像是"返回"的箭头 |
| 条件语句 | `if/else` | `(条件) ? {}` 或 `(条件) ? {} : {}` | 简化形式和完整形式 |
| 模式匹配 | `match` | `#(条件) {}` | # 像是"分类匹配" |
| 结构定义 | `struct` | `@` | @ 像是"定义"的标记 |
| 方法实现 | `impl` | `&` | & 像是"连接"到类型上 |
| 接口定义 | `interface` | `%` | % 像是"规范/模板" |
| 枚举类型 | `enum` | `#` | # 像是"分类编号" |
| 构造函数 | `new` | `类名()` | 函数名必须为类名 |
| 析构函数 | `drop/~` | `~类名()` | ~ 像是"销毁"符号 |
| 循环语句 | `while` | `^(条件) {}` | ^ 像是"循环向上" |
| 遍历循环 | `for..in` | `$(item : collection) {}` | $ 像是"逐个处理" |
| 自引用 | `self` | `_` | _ 像是"当前对象" |
| 可变标记 | `mut` | `*` | * 像是"可变的" |
| 布尔真值 | `true` | `1` | 1 像是"真/有" |
| 布尔假值 | `false` | `0` | 0 像是"假/无" |
| 跳出循环 | `break` | `<<` | << 像是"跳出" |
| 继续循环 | `continue` | `->` | -> 像是"继续向前" |

## � 数据类型符号化

| 数据类型 | 传统写法 | polyglot符号 | 说明 |
|----------|----------|--------------|------|
| 整数32位 | `i32` | `i` | I 像是"整数Integer" |
| 整数64位 | `i64` | `I` | II 像是"大整数" |
| 浮点32位 | `f32` | `f` | F 像是"浮点Float" |
| 浮点64位 | `f64` | `F` | FF 像是"双精度浮点" |
| 布尔类型 | `bool` | `b` | B 像是"布尔Boolean" |
| 字符串 | `String` | `s` | S 像是"字符串String" |
| 字符 | `char` | `c` | C 像是"字符Character" |
| 数组 | `Vec<T>` | `[T]` | [] 像是"容器" |

## �🔤 完整语法示例

### 1. 基础数据结构定义
```rust
// 传统语法（其他语言）
struct Player {
    id: i32,
    health: f32,
    position: Vector3
}

// polyglot 符号语法 ✨
@ Player {
    id: i32,
    health: f32,
    position: Vector3
}
```

### 2. 方法实现
```rust
// 传统语法
impl Player {
    fn get_health(&self) -> f32 {
        return self.health;
    }
}

// polyglot 符号语法 ✨
& Player {
    get_health() -> f32 {
        <- _.health
    }

    take_damage(amount: f32) {
        _.health -= amount
        (_.health < 0) ? {
            _.health = 0
        }
    }

    move_to(new_position: Vector3) {
        _.position = new_position
    }
}
```

### 3. 接口定义
```rust
// 传统语法
interface GameEntity {
    fn get_id(&self) -> i32;
    fn get_position(&self) -> Vector3;
}

// polyglot 符号语法 ✨
% GameEntity {
    get_id() -> i32
    get_position() -> Vector3
    is_alive() -> bool
}
```

### 4. 枚举类型
```rust
// 传统语法
enum PlayerState {
    Idle,
    Moving,
    Attacking,
    Dead
}

// polyglot 符号语法 ✨
# PlayerState {
    Idle,
    Moving,
    Attacking,
    Dead
}
```

### 5. 完整的游戏实体示例
```rust
// 定义向量结构
@ Vector3 {
    x: f32,
    y: f32,
    z: f32
}

// 定义实体ID
@ EntityId {
    value: i32
}

// 定义游戏实体接口
% GameEntity {
    get_id() -> EntityId
    get_position() -> Vector3
    get_health() -> f32
    take_damage(amount: f32)
    is_alive() -> bool
}

// 定义玩家状态
# PlayerState {
    Idle,
    Moving,
    Attacking,
    Casting,
    Dead
}

// 定义玩家结构
@ Player {
    id: EntityId,
    health: f32,
    max_health: f32,
    position: Vector3,
    state: PlayerState,
    attack_power: f32,
    defense: f32,
    target_enemy_id: EntityId
}

// 实现游戏实体接口
& Player % GameEntity {
    get_id() -> EntityId {
        <- _.id
    }

    get_position() -> Vector3 {
        <- _.position
    }

    get_health() -> f32 {
        <- _.health
    }

    take_damage(amount: f32) {
        (_.state == PlayerState::Dead) ? {
            <-
        }

        ? actual_damage = amount - _.defense
        (actual_damage > 0) ? {
            _.health -= actual_damage
            (_.health <= 0) ? {
                _.health = 0
                _.state = PlayerState::Dead
            }
        }
    }

    is_alive() -> bool {
        <- _.health > 0
    }
}

// 玩家专有方法
& Player {
    Player(id: EntityId, position: Vector3) {
        <- Player {
            id: id,
            health: 100.0,
            max_health: 100.0,
            position: position,
            state: PlayerState::Idle,
            attack_power: 25.0,
            defense: 5.0,
            target_enemy_id: EntityId { value: 0 }
        }
    }

    set_target(enemy_id: EntityId) {
        _.target_enemy_id = enemy_id
        _.state = PlayerState::Attacking
    }

    move_to(target: Vector3) {
        _.position = target
        _.state = PlayerState::Moving
    }

    attack(enemy: Enemy) -> f32 {
        (_.is_alive() == 0) ? {
            <- 0.0
        } : {
            _.state = PlayerState::Attacking
            <- _.attack_power
        }
    }

    heal(amount: f32) {
        (_.is_alive()) ? {
            _.health += amount
            (_.health > _.max_health) ? {
                _.health = _.max_health
            }
        }
    }

    ~Player() {
        // 析构函数 - 清理资源
        print("Player destroyed: " + _.id.value)
        _.target_enemy_id = EntityId { value: 0 }
        notify_player_destroyed(_.id)
    }
}
```

### 6. 敌人实体
```rust
// 定义敌人结构
@ Enemy {
    id: EntityId,
    health: f32,
    position: Vector3,
    attack_power: f32,
    defense: f32,
    target_player_id: EntityId,
    ai_state: EnemyAIState
}

// 敌人AI状态
# EnemyAIState {
    Patrolling,
    Chasing,
    Attacking,
    Retreating
}

// 实现游戏实体接口
& Enemy % GameEntity {
    get_id() -> EntityId {
        <- _.id
    }

    get_position() -> Vector3 {
        <- _.position
    }

    get_health() -> f32 {
        <- _.health
    }

    take_damage(amount: f32) {
        ? actual_damage = amount - _.defense
        (actual_damage > 0) ? {
            _.health -= actual_damage
        }
    }

    is_alive() -> bool {
        <- _.health > 0
    }
}

// 敌人专有行为
& Enemy {
    update_ai(entities: EntityManager) {
        #(_.ai_state) {
            EnemyAIState::Patrolling => {
                // 寻找附近的玩家
                (player_id := entities.find_nearest_player(_.id, 8.0)) ? {
                    _.target_player_id = player_id
                    _.ai_state = EnemyAIState::Chasing
                }
            },

            EnemyAIState::Chasing => {
                (player := entities.get_player(_.target_player_id)) ? {
                    ? distance = calculate_distance(_.position, player.position)
                    (distance <= 2.0) ? {
                        _.ai_state = EnemyAIState::Attacking
                    } : {
                        (distance > 15.0) ? {
                            _.ai_state = EnemyAIState::Patrolling
                            _.target_player_id = EntityId { value: 0 }
                        }
                    }
                }
            },

            EnemyAIState::Attacking => {
                // 攻击逻辑
            },

            EnemyAIState::Retreating => {
                (_.health > 20.0) ? {
                    _.ai_state = EnemyAIState::Chasing
                }
            }
        }
    }
}
```

## 🎨 符号选择的设计考量

### `@` (结构定义)
- **视觉联想**: @ 像是"标记"或"标签"，用于定义数据结构
- **简洁性**: 只需一个字符
- **直观性**: 在其他语言中 @ 常用于注解，这里用于"注解"数据类型

### `&` (方法实现)
- **视觉联想**: & 像是"连接符"，将方法连接到类型上
- **语义关联**: 表示"属于"某个类型的方法
- **易记性**: 类似"and"的概念，方法和类型结合

### `%` (接口定义)
- **视觉联想**: % 像是"模板"或"规格"符号
- **抽象性**: 接口是抽象的规范，% 符号体现这种抽象性
- **区分度**: 与其他符号有明显区别

### `#` (枚举类型)
- **视觉联想**: # 像是"编号"或"分类"标记
- **广泛认知**: # 在很多地方表示编号或分类
- **简洁清晰**: 一目了然

## 🌟 语法优势

### 1. **学习成本低**
- 符号比单词更容易记忆
- 减少拼写错误
- 视觉上更简洁

### 2. **代码密度高**
```rust
// 传统语法 (16个字符)
struct Player {

// polyglot语法 (9个字符)
@ Player {
```

### 3. **跨语言友好**
- 符号在各种语言中都容易处理
- 不受自然语言限制
- 便于代码生成

### 4. **IDE友好**
- 符号易于语法高亮
- 自动完成更简单
- 错误检测更准确

这种符号化语法让 polyglot 语言更加简洁直观，特别适合编程新手快速上手