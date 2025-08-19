CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g
TARGET = polyglot
SRCDIR = compiler
SOURCES = $(SRCDIR)/main.cpp $(SRCDIR)/lexer.cpp $(SRCDIR)/parser.cpp $(SRCDIR)/semantic.cpp $(SRCDIR)/codegen.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# 主目标
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

# 编译源文件
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理
clean:
	rm -f $(OBJECTS) $(TARGET)

# 测试编译器
test: $(TARGET)
	./$(TARGET) test.pg

# 显示帮助
help:
	@echo "可用的目标:"
	@echo "  all    - 编译 polyglot 编译器"
	@echo "  clean  - 清理生成的文件"
	@echo "  test   - 使用测试文件运行编译器"
	@echo "  help   - 显示此帮助信息"

.PHONY: all clean test help