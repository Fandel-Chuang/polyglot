# polyglot 完整符号语法示例

## 🎯 polyglot 语言符号对照表

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
| 不可变标记 | `const` | `*` | * 像是"固定不变" |

## 🚀 极简语法对比

### 传统语言 vs polyglot
```rust
// 传统语言 (70+ 字符)
import "module"
func calculate() {}
struct Player {}
impl Player {}
interface Entity {}
enum State {}

// polyglot 语言 (30 字符) ✨
>> "module"
calculate() {}
@ Player {}
& Player {}
% Entity {}
# State {}
```

**代码量减少 50%！**

## 🎮 完整游戏示例

### common.pg - 公共模块
```rust
// 导入外部库
>> "math"

// 基础数据类型
@ Vector3 {
    x: f32,
    y: f32,
    z: f32
}

@ EntityId {
    value: i32
}

// 游戏实体通用接口
% GameEntity {
    get_id() -> EntityId
    get_position() -> Vector3
    get_health() -> f32
    take_damage(damage: f32)
    is_alive() -> bool
}

// 公共工具函数
calculate_damage(attack: f32, defense: f32) -> f32 {
    ? damage = attack - defense
    (damage < 0) ? {
        <- 0.0
    } : {
        <- damage
    }
}

calculate_distance(pos1: Vector3, pos2: Vector3) -> f32 {
    ? dx = pos1.x - pos2.x
    ? dy = pos1.y - pos2.y
    ? dz = pos1.z - pos2.z
    <- sqrt(dx*dx + dy*dy + dz*dz)
}
```

### player.pg - 玩家模块
```rust
>> "common"

// 玩家状态枚举
# PlayerState {
    Idle,
    Moving,
    Attacking,
    Casting,
    Dead
}

// 玩家数据结构
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
        <- self.id
    }

    get_position() -> Vector3 {
        <- self.position
    }

    get_health() -> f32 {
        <- self.health
    }

    take_damage(damage: f32) {
        (self.state == PlayerState::Dead) ? {
            <-
        }

        ? = damage - self.defense
        (actual_damage > 0) ? {
            self.health -= actual_damage
            (self.health <= 0) ? {
                self.health = 0
                self.state = PlayerState::Dead
            }
        }
    }

    is_alive() -> bool {
        <- self.health > 0
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

    ~Player() {
        // 释放资源，清理引用
        print("Player " + self.id.value + " is being destroyed")

        // 清理目标引用
        self.target_enemy_id = EntityId { value: 0 }

        // 通知其他系统玩家已销毁
        notify_player_destroyed(self.id)
    }

    set_target(enemy_id: EntityId) {
        self.target_enemy_id = enemy_id
        self.state = PlayerState::Attacking
    }

    move_to(target: Vector3) {
        self.position = target
        self.state = PlayerState::Moving
    }

    attack() -> f32 {
        (!self.is_alive()) ? {
            <- 0.0
        } : {
            self.state = PlayerState::Attacking
            <- self.attack_power
        }
    }

    heal(amount: f32) {
        (self.is_alive()) ? {
            self.health += amount
            (self.health > self.max_health) ? {
                self.health = self.max_health
            }
        }
    }
}

// 玩家AI行为函数
player_find_and_attack(player_id: EntityId, entities: EntityManager) -> bool {
    (player := entities.get_player(player_id)) ? {
        // 检查当前目标
        (player.target_enemy_id.value != 0) ? {
            (enemy := entities.get_enemy(player.target_enemy_id)) ? {
                distance := calculate_distance(player.position, enemy.position)
                (distance <= 5.0) ? {
                    player_attack_enemy(player_id, player.target_enemy_id, entities)
                    <- true
                }
            }
        }

        // 寻找新目标
        (enemy_id := entities.find_nearest_enemy(player_id, 10.0)) ? {
            ? updated_player = player
            updated_player.set_target(enemy_id)
            entities.update_player(player_id, updated_player)
            <- true
        }
    }
    <- false
}

player_attack_enemy(player_id: EntityId, enemy_id: EntityId, entities: EntityManager) {
    (? player = entities.get_player(player_id)) ? {
        (? enemy = entities.get_enemy(enemy_id)) ? {
            ? damage = calculate_damage(player.attack_power, enemy.defense)
            ? damaged_enemy = enemy
            damaged_enemy.take_damage(damage)
            entities.update_enemy(enemy_id, damaged_enemy)

            (!damaged_enemy.is_alive()) ? {
                entities.remove_enemy(enemy_id)
                ? updated_player = player
                updated_player.target_enemy_id = EntityId { value: 0 }
                entities.update_player(player_id, updated_player)
            } : {}
        } : {}
    } : {}
}
```

### enemy.pg - 敌人模块
```rust
>> "common"

// 敌人AI状态
# EnemyAIState {
    Patrolling,
    Chasing,
    Attacking,
    Retreating
}

// 敌人数据结构
@ Enemy {
    id: EntityId,
    health: f32,
    position: Vector3,
    attack_power: f32,
    defense: f32,
    ai_state: EnemyAIState,
    target_player_id: EntityId
}

// 实现游戏实体接口
& Enemy % GameEntity {
    get_id() -> EntityId {
        <- self.id
    }

    get_position() -> Vector3 {
        <- self.position
    }

    get_health() -> f32 {
        <- self.health
    }

    take_damage(damage: f32) {
        ? actual_damage = damage - self.defense
        (actual_damage > 0) ? {
            self.health -= actual_damage
            (self.health <= 20.0) ? {
                self.ai_state = EnemyAIState::Retreating
            } : {}
        } : {}
    }

    is_alive() -> bool {
        <- self.health > 0
    }
}

// 敌人专有方法
& Enemy {
    Enemy(id: EntityId, position: Vector3) {
        <- Enemy {
            id: id,
            health: 80.0,
            position: position,
            attack_power: 20.0,
            defense: 3.0,
            ai_state: EnemyAIState::Patrolling,
            target_player_id: EntityId { value: 0 }
        }
    }

    ~Enemy() {
        // 清理敌人资源
        print("Enemy " + self.id.value + " is being destroyed")

        // 清理目标引用
        self.target_player_id = EntityId { value: 0 }

        // 停止AI行为
        self.ai_state = EnemyAIState::Patrolling

        // 通知游戏系统敌人已销毁
        notify_enemy_destroyed(self.id)
    }

    update_ai(entities: EntityManager) {
        #(self.ai_state) {
            EnemyAIState::Patrolling => {
                (player_id := entities.find_nearest_player(self.id, 8.0)) ? {
                    self.target_player_id = player_id
                    self.ai_state = EnemyAIState::Chasing
                }
            },

            EnemyAIState::Chasing => {
                (player := entities.get_player(self.target_player_id)) ? {
                    ? distance = calculate_distance(self.position, player.position)
                    (distance <= 2.0) ? {
                        self.ai_state = EnemyAIState::Attacking
                    } : {
                        (distance > 15.0) ? {
                            self.ai_state = EnemyAIState::Patrolling
                            self.target_player_id = EntityId { value: 0 }
                        }
                    }
                }
            },

            EnemyAIState::Attacking => {
                // 执行攻击
            },

            EnemyAIState::Retreating => {
                (self.health > 40.0) ? {
                    self.ai_state = EnemyAIState::Chasing
                }
            }
        }
    }
}
```

### main.pg - 主程序
```rust
>> "common"
>> "player"
>> "enemy"

@ EntityManager {
    players: HashMap<EntityId, Player>,
    enemies: HashMap<EntityId, Enemy>,
    next_id: i32
}

& EntityManager {
    new() -> EntityManager {
        <- EntityManager {
            players: HashMap::new(),
            enemies: HashMap::new(),
            next_id: 1
        }
    }

    create_player(position: Vector3) -> EntityId {
        ? id = EntityId { value: self.next_id }
        self.next_id += 1
        ? player = Player::new(id, position)
        self.players.insert(id, player)
        <- id
    }

    create_enemy(position: Vector3) -> EntityId {
        ? id = EntityId { value: self.next_id }
        self.next_id += 1
        ? enemy = Enemy::new(id, position)
        self.enemies.insert(id, enemy)
        <- id
    }

    get_player(id: EntityId) -> Option<Player> {
        <- self.players.get(id)
    }

    get_enemy(id: EntityId) -> Option<Enemy> {
        <- self.enemies.get(id)
    }
}

main() {
    ? entities = EntityManager::new()

    // 创建游戏世界
    ? player_id = entities.create_player(Vector3 { x: 0.0, y: 0.0, z: 0.0 })
    ? enemy1_id = entities.create_enemy(Vector3 { x: 8.0, y: 0.0, z: 0.0 })
    ? enemy2_id = entities.create_enemy(Vector3 { x: 12.0, y: 0.0, z: 0.0 })

    // 游戏主循环
    ? turn = 0
    ^(turn < 20) {
        turn += 1

        // 玩家回合
        player_find_and_attack(player_id, entities)

        // 敌人回合
        (enemy1 := entities.get_enemy(enemy1_id)) ? {
            enemy1.update_ai(entities)
        }

        (enemy2 := entities.get_enemy(enemy2_id)) ? {
            enemy2.update_ai(entities)
        }

        // 检查游戏结束
        (entities.enemies.is_empty()) ? {
            print("玩家获胜！")
            break
        }

        (player := entities.get_player(player_id)) ? {
            (!player.is_alive()) ? {
                print("游戏结束！")
                break
            }
        }
    }
}
```

## 🎯 符号语法的优势

### 1. **极简语法**
```rust
// 函数定义更简洁
func calculate_damage() {}  // 传统 (23 字符)
calculate_damage() {}       // polyglot (18 字符)

// 结构定义更简洁
struct Player {}            // 传统 (16 字符)
@ Player {}                 // polyglot (11 字符)
```

### 2. **视觉清晰**
- `>>` 看起来像导入箭头
- `?` 像是询问/获取变量
- `@` 像是定义标签
- `&` 像是连接符
- `%` 像是规范模板
- `#` 像是分类编号

### 3. **学习友好**
- 符号直观易记
- 减少拼写错误
- 新手不需要记住复杂单词

### 4. **跨语言兼容**
- 符号在任何语言环境都通用
- 便于自动代码生成
- IDE支持更简单

这就是 polyglot 语言的完整符号化语法！简洁、直观、强大！