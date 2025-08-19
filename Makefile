CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O0
INCLUDES = -I.

# 源文件
SRCDIR = compiler
SOURCES = $(SRCDIR)/lexer.cpp $(SRCDIR)/error.cpp $(SRCDIR)/main.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# 主程序目标
MAIN_TARGET = polyglot

# 默认目标
all: $(MAIN_TARGET)

# 编译主程序
$(MAIN_TARGET): $(OBJECTS)
	@echo "🔨 编译polyglot主程序..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^
	@echo "✅ 主程序编译完成!"

# 编译源文件
%.o: %.cpp
	@echo "🔨 编译 $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# 运行AI测试
test:
	@echo "🚀 运行AI测试..."
	@cd ai_tests && $(MAKE) test

# 构建AI测试
test-build:
	@echo "🔨 构建AI测试..."
	@cd ai_tests && $(MAKE) all

# 清理
clean:
	@echo "🧹 清理编译文件..."
	rm -f $(OBJECTS) $(MAIN_TARGET)
	@cd ai_tests && $(MAKE) clean
	@echo "✅ 清理完成!"

# 重新编译
rebuild: clean all

# 调试编译
debug: CXXFLAGS += -DDEBUG -fsanitize=address
debug: $(MAIN_TARGET)

# 安装依赖检查
deps:
	@echo "📦 检查系统依赖..."
	@echo "需要: C++17编译器 (g++/clang++) 和 UTF-8支持"
	@$(CXX) --version

# 显示帮助
help:
	@echo "polyglot/文达编译器 主Makefile"
	@echo ""
	@echo "可用目标:"
	@echo "  all        - 编译主程序 (默认)"
	@echo "  test       - 运行AI测试"
	@echo "  test-build - 构建AI测试程序"
	@echo "  clean      - 清理所有编译文件"
	@echo "  rebuild    - 重新编译"
	@echo "  debug      - 调试模式编译"
	@echo "  deps       - 检查系统依赖"
	@echo "  help       - 显示此帮助"
	@echo ""
	@echo "AI测试位于: ai_tests/ 目录"

.PHONY: all test test-build clean rebuild debug deps help