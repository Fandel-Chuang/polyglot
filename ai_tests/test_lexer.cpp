#include <iostream>
#include <fstream>
#include <sstream>
#include "../compiler/lexer.h"
#include "../compiler/error.h"

// 测试辅助函数
void testLexer(const std::string& testName, const std::string& sourceCode) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "🧪 测试: " << testName << std::endl;
    std::cout << std::string(60, '=') << std::endl;

    std::cout << "\n📝 源代码:" << std::endl;
    std::cout << sourceCode << std::endl;

    try {
        Lexer lexer(sourceCode);
        auto tokens = lexer.tokenize();

        std::cout << "\n✅ 词法分析成功!" << std::endl;
        std::cout << "📊 共生成 " << tokens.size() << " 个Token" << std::endl;

        // 打印Token详情
        lexer.printTokens();

    } catch (const LexerError& e) {
        std::cout << "\n❌ 词法分析错误: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "\n💥 未知错误: " << e.what() << std::endl;
    }
}

// 从文件读取测试代码
void testFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "❌ 无法打开文件: " << filename << std::endl;
        return;
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    testLexer("文件测试: " + filename, content);
}

int main() {
    std::cout << "🚀 polyglot/文达词法分析器测试程序" << std::endl;
    std::cout << "测试变量命名规范和Unicode支持" << std::endl;

    // 测试1: 英文版基本变量命名
    testLexer("英文版变量命名", R"(
? $name = "Alice"
? $age: i32 = 25
? $health: f32 = 100.0
)");

    // 测试2: 中文版基本变量命名
    testLexer("中文版变量命名", R"(
？ ＄姓名 ＝ "爱丽丝"
？ ＄年龄： 整数 ＝ 25
？ ＄生命值： 浮点数 ＝ 100.0
)");

    // 测试3: 英文版结构体定义
    testLexer("英文版结构体", R"(
@ Player {
    $name: string,
    $health: f32,
    $position: Vector3
}
)");

    // 测试4: 中文版结构体定义
    testLexer("中文版结构体", R"(
＠ 玩家 ｛
    ＄姓名： 字符串，
    ＄生命值： 浮点数，
    ＄位置： 三维向量
｝
)");

    // 测试5: 英文版方法实现
    testLexer("英文版方法", R"(
& Player {
    take_damage($amount: f32) {
        _.$health -= $amount
    }
}
)");

    // 测试6: 中文版方法实现
    testLexer("中文版方法", R"(
＆ 玩家 ｛
    受到伤害（＄伤害量： 浮点数） ｛
        ＿．＄生命值 －＝ ＄伤害量
    ｝
｝
)");

    // 测试7: 混合符号检测
    testLexer("混合符号模式", R"(
? $name = "Test"
？ ＄姓名 ＝ "测试"
)");

    // 测试8: 复杂表达式
    testLexer("复杂表达式", R"(
($player.$health > 0.0) ? {
    print("Player alive: " + $player.$name)
} : {
    print("Player dead")
}
)");

    // 测试9: 从文件读取
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "📁 从文件读取测试" << std::endl;
    std::cout << std::string(60, '=') << std::endl;

    testFromFile("../test_english.pg");
    testFromFile("../test_chinese.文达");

    // 测试10: 错误处理
    testLexer("错误处理测试", R"(
? $name = "未结束的字符串
? $invalid@ = 123
)");

    std::cout << "\n🎉 测试完成!" << std::endl;
    return 0;
}