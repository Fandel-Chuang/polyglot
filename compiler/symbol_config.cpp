#include "symbol_config.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

SymbolConfigLoader::SymbolConfigLoader(const std::string& configPath)
    : configFile(configPath) {}

bool SymbolConfigLoader::loadConfig() {
    std::ifstream file(configFile);
    if (!file.is_open()) {
        std::cerr << "❌ 无法打开符号配置文件: " << configFile << std::endl;
        return false;
    }

    std::string line;
    bool inUnicodeSection = false;
    bool inChineseKeywordsSection = false;

    while (std::getline(file, line)) {
        line = trim(line);

        // 跳过空行和注释
        if (line.empty() || line[0] == '/' || line[0] == '{' || line[0] == '}') {
            continue;
        }

        // 检测Unicode映射区域
        if (line.find("\"full_width_symbols\"") != std::string::npos) {
            inUnicodeSection = true;
            inChineseKeywordsSection = false;
            continue;
        }
        else if (line.find("\"chinese_keywords\"") != std::string::npos) {
            inChineseKeywordsSection = true;
            inUnicodeSection = false;
            continue;
        }
        else if (line.find("\"builtin_functions\"") != std::string::npos ||
                 line.find("\"core_symbols\"") != std::string::npos ||
                 line.find("\"operators\"") != std::string::npos ||
                 line.find("\"punctuation\"") != std::string::npos) {
            inUnicodeSection = false;
            inChineseKeywordsSection = false;
            continue;
        }

        // 解析Unicode映射
        if (inUnicodeSection && line.find(":") != std::string::npos) {
            size_t colonPos = line.find(":");
            if (colonPos != std::string::npos) {
                std::string unicodeChar = extractStringValue(line.substr(0, colonPos));
                std::string tokenType = extractStringValue(line.substr(colonPos + 1));

                if (!unicodeChar.empty() && !tokenType.empty()) {
                    unicodeMappings[unicodeChar] = tokenType;
                }
            }
        }

        // 解析中文关键字映射
        if (inChineseKeywordsSection && line.find(":") != std::string::npos) {
            size_t colonPos = line.find(":");
            if (colonPos != std::string::npos) {
                std::string chineseKeyword = extractStringValue(line.substr(0, colonPos));
                std::string tokenType = extractStringValue(line.substr(colonPos + 1));

                if (!chineseKeyword.empty() && !tokenType.empty()) {
                    unicodeMappings[chineseKeyword] = tokenType;
                }
            }
        }
    }

    file.close();

    std::cout << "✅ 成功加载符号配置，共 " << unicodeMappings.size() << " 个映射" << std::endl;
    return true;
}

std::string SymbolConfigLoader::getTokenTypeForUnicode(const std::string& unicodeChar) {
    auto it = unicodeMappings.find(unicodeChar);
    if (it != unicodeMappings.end()) {
        return it->second;
    }
    return "";
}

bool SymbolConfigLoader::isKnownUnicodeSymbol(const std::string& unicodeChar) {
    return unicodeMappings.find(unicodeChar) != unicodeMappings.end();
}

void SymbolConfigLoader::printConfig() {
    std::cout << "\n=== 符号映射配置 ===" << std::endl;
    for (const auto& pair : unicodeMappings) {
        std::cout << "'" << pair.first << "' -> " << pair.second << std::endl;
    }
}

std::string SymbolConfigLoader::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";

    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string SymbolConfigLoader::extractStringValue(const std::string& line) {
    std::string trimmed = trim(line);

    // 移除双引号
    size_t start = trimmed.find('"');
    if (start == std::string::npos) return "";

    size_t end = trimmed.find('"', start + 1);
    if (end == std::string::npos) return "";

    return trimmed.substr(start + 1, end - start - 1);
}

std::vector<std::string> SymbolConfigLoader::extractArrayValues(const std::string& line) {
    std::vector<std::string> values;
    // 简化实现，暂不需要
    return values;
}

std::vector<std::string> SymbolConfigLoader::getSymbolVariants(const std::string& symbolName) {
    std::vector<std::string> variants;
    // 简化实现，暂不需要
    return variants;
}

// Token类型转换函数
TokenType stringToTokenType(const std::string& typeName) {
    static std::unordered_map<std::string, TokenType> typeMap = {
        {"LEFT_PAREN", TokenType::LEFT_PAREN},
        {"RIGHT_PAREN", TokenType::RIGHT_PAREN},
        {"LEFT_BRACE", TokenType::LEFT_BRACE},
        {"RIGHT_BRACE", TokenType::RIGHT_BRACE},
        {"LEFT_BRACKET", TokenType::LEFT_BRACKET},
        {"RIGHT_BRACKET", TokenType::RIGHT_BRACKET},
        {"COMMA", TokenType::COMMA},
        {"SEMICOLON", TokenType::SEMICOLON},
        {"COLON", TokenType::COLON},
        {"DOT", TokenType::DOT},
        {"GREATER_THAN", TokenType::GREATER_THAN},
        {"LESS_THAN", TokenType::LESS_THAN},
        {"ASSIGN", TokenType::ASSIGN},
        {"PLUS", TokenType::PLUS},
        {"MINUS", TokenType::MINUS},
        {"QUESTION", TokenType::QUESTION},
        {"STRUCT_DEF", TokenType::STRUCT_DEF},
        {"IMPL_DEF", TokenType::IMPL_DEF},
        {"ENUM", TokenType::ENUM},
        {"VARIABLE_PREFIX", TokenType::VARIABLE_PREFIX},
        {"SELF_REF", TokenType::SELF_REF},
        {"CONSTANT", TokenType::CONSTANT},
        {"SLASH", TokenType::SLASH},
        {"INTERFACE", TokenType::INTERFACE},
        {"LOOP", TokenType::LOOP},
        {"LOGICAL_NOT", TokenType::LOGICAL_NOT},
        {"TYPE_I32", TokenType::TYPE_I32},
        {"TYPE_I64", TokenType::TYPE_I64},
        {"TYPE_F32", TokenType::TYPE_F32},
        {"TYPE_F64", TokenType::TYPE_F64},
        {"TYPE_BOOL", TokenType::TYPE_BOOL},
        {"TYPE_STRING", TokenType::TYPE_STRING},
        {"TYPE_CHAR", TokenType::TYPE_CHAR},
        {"TRUE", TokenType::TRUE},
        {"FALSE", TokenType::FALSE}
    };

    auto it = typeMap.find(typeName);
    if (it != typeMap.end()) {
        return it->second;
    }

    return TokenType::UNKNOWN;
}

std::string tokenTypeToString(TokenType type) {
    static std::unordered_map<TokenType, std::string> reverseMap = {
        {TokenType::LEFT_PAREN, "LEFT_PAREN"},
        {TokenType::RIGHT_PAREN, "RIGHT_PAREN"},
        {TokenType::LEFT_BRACE, "LEFT_BRACE"},
        {TokenType::RIGHT_BRACE, "RIGHT_BRACE"},
        // ... 可以根据需要添加更多映射
    };

    auto it = reverseMap.find(type);
    if (it != reverseMap.end()) {
        return it->second;
    }

    return "UNKNOWN";
}