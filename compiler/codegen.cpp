#include "codegen.h"
#include <iostream>

std::string CodeGenerator::generate(const std::unique_ptr<Program>& program) {
    std::cout << "   ⚙️ 代码生成器暂时返回示例C++代码" << std::endl;

    // 占位符实现 - 生成简单的C++代码
    std::string code = R"(#include <iostream>
#include <string>

// 生成的 polyglot 程序
int main() {
    std::cout << "Hello from polyglot!" << std::endl;
    return 0;
}
)";

    return code;
}