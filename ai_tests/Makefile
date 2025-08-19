# polyglot/文达编译器 AI测试 Makefile

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O0
INCLUDES = -I..

# 源文件路径
COMPILER_DIR = ../compiler
SOURCES = $(COMPILER_DIR)/lexer.cpp $(COMPILER_DIR)/error.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# 测试程序
TEST_TARGET = test_lexer
TEST_SOURCE = test_lexer.cpp

# 默认目标
all: $(TEST_TARGET)

# 编译测试程序
$(TEST_TARGET): $(TEST_SOURCE) $(OBJECTS)
	@echo "🔨 编译AI测试程序..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^
	@echo "✅ 编译完成!"

# 编译源文件
$(COMPILER_DIR)/%.o: $(COMPILER_DIR)/%.cpp
	@echo "🔨 编译 $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# 运行测试
test: $(TEST_TARGET)
	@echo "🚀 运行polyglot/文达词法分析器测试..."
	@echo ""
	./$(TEST_TARGET)

# 清理
clean:
	@echo "🧹 清理AI测试编译文件..."
	rm -f $(OBJECTS) $(TEST_TARGET)
	@echo "✅ 清理完成!"

# 重新编译
rebuild: clean all

# 调试编译
debug: CXXFLAGS += -DDEBUG -fsanitize=address
debug: $(TEST_TARGET)

# 显示帮助
help:
	@echo "polyglot/文达编译器 AI测试 Makefile"
	@echo ""
	@echo "可用目标:"
	@echo "  all      - 编译测试程序 (默认)"
	@echo "  test     - 编译并运行词法分析器测试"
	@echo "  clean    - 清理编译文件"
	@echo "  rebuild  - 重新编译"
	@echo "  debug    - 调试模式编译"
	@echo "  help     - 显示此帮助"

.PHONY: all test clean rebuild debug help