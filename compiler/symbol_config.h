#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <iostream>
#include "token_types.h"

class SymbolConfigLoader {
private:
    std::string configFile;
    std::unordered_map<std::string, std::string> unicodeMappings;
    std::unordered_map<std::string, TokenType> symbolToTokenType;
    std::unordered_map<std::string, std::string> chineseKeywords;
    std::unordered_map<std::string, std::vector<std::string>> builtinFunctions;

    // JSON解析辅助方法
    void parseSymbolMappings(const std::string& content);
    void parseSymbolSection(const std::string& content, const std::string& sectionName);
    void parseKeywords(const std::string& content);
    void parseBuiltinFunctions(const std::string& content);

    size_t findMatchingBrace(const std::string& content, size_t startPos);
    std::vector<std::string> parseArray(const std::string& arrayContent);
    TokenType getTokenTypeFromKey(const std::string& key);

    // 原有辅助方法
    std::string trim(const std::string& str);
    std::string extractStringValue(const std::string& line);
    std::vector<std::string> extractArrayValues(const std::string& line);

public:
    SymbolConfigLoader(const std::string& configPath = "symbol_mapping.json");

    bool loadConfig();
    std::string getTokenTypeForUnicode(const std::string& unicodeChar);
    bool isKnownUnicodeSymbol(const std::string& unicodeChar);
    void printConfig();

    // 新增访问方法
    TokenType getSymbolTokenType(const std::string& symbol) {
        auto it = symbolToTokenType.find(symbol);
        return (it != symbolToTokenType.end()) ? it->second : TokenType::UNKNOWN;
    }

    // 提供整张符号映射表（用于覆盖Lexer内部symbolMap）
    const std::unordered_map<std::string, TokenType>& getAllSymbolTokenTypes() const {
        return symbolToTokenType;
    }

    bool isChineseKeyword(const std::string& word) {
        return chineseKeywords.find(word) != chineseKeywords.end();
    }

    std::string getChineseKeywordType(const std::string& word) {
        auto it = chineseKeywords.find(word);
        return (it != chineseKeywords.end()) ? it->second : "";
    }

    bool isBuiltinFunction(const std::string& name) {
        for (const auto& func : builtinFunctions) {
            for (const auto& variant : func.second) {
                if (variant == name) return true;
            }
        }
        return false;
    }

    std::vector<std::string> getSymbolVariants(const std::string& symbolName);
};

// 全局转换函数
TokenType stringToTokenType(const std::string& typeName);
std::string tokenTypeToString(TokenType type);
