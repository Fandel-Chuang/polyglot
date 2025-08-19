#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <iostream>
#include "token_types.h"

// 简单的JSON解析器，专门用于符号映射配置
class SymbolConfigLoader {
private:
    std::string configFile;
    std::unordered_map<std::string, std::string> unicodeMappings;
    std::unordered_map<std::string, std::vector<std::string>> symbolGroups;

public:
    SymbolConfigLoader(const std::string& configPath = "symbol_mapping.json");

    // 加载配置文件
    bool loadConfig();

    // 获取Unicode字符对应的Token类型
    std::string getTokenTypeForUnicode(const std::string& unicodeChar);

    // 获取符号组的所有变体
    std::vector<std::string> getSymbolVariants(const std::string& symbolName);

    // 检查是否是已知的Unicode符号
    bool isKnownUnicodeSymbol(const std::string& unicodeChar);

    // 打印配置信息（用于调试）
    void printConfig();

private:
    // 简单的JSON解析辅助函数
    std::string trim(const std::string& str);
    std::string extractStringValue(const std::string& line);
    std::vector<std::string> extractArrayValues(const std::string& line);
};

// Token类型名称到枚举值的映射
TokenType stringToTokenType(const std::string& typeName);
std::string tokenTypeToString(TokenType type);
