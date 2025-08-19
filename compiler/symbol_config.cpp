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
        std::cout << "⚠️  警告：无法打开符号配置文件: " << configFile << std::endl;
        std::cout << "⚠️  将使用内置符号映射继续编译" << std::endl;
        return true;  // 返回true让程序继续运行
    }

    // 读取整个文件内容
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();

    if (content.empty()) {
        std::cout << "⚠️  警告：配置文件为空: " << configFile << std::endl;
        std::cout << "⚠️  将使用内置符号映射继续编译" << std::endl;
        return true;  // 返回true让程序继续运行
    }

    try {
        // 解析JSON结构的符号映射
        parseSymbolMappings(content);
        parseKeywords(content);
        parseBuiltinFunctions(content);

        std::cout << "✅ 成功加载符号配置，共 " << unicodeMappings.size() << " 个符号映射" << std::endl;
        std::cout << "✅ 加载 " << chineseKeywords.size() << " 个中文关键字" << std::endl;
        std::cout << "✅ 加载 " << builtinFunctions.size() << " 个内置函数" << std::endl;

        return true;
    } catch (const std::exception& e) {
        std::cout << "⚠️  警告：解析配置文件失败: " << e.what() << std::endl;
        std::cout << "⚠️  将使用内置符号映射继续编译" << std::endl;
        return true;  // 返回true让程序继续运行
    }
}

void SymbolConfigLoader::parseSymbolMappings(const std::string& content) {
    // 解析 core_symbols 部分
    parseSymbolSection(content, "core_symbols");

    // 解析 operators 部分
    parseSymbolSection(content, "operators");

    // 解析 punctuation 部分
    parseSymbolSection(content, "punctuation");
}

void SymbolConfigLoader::parseSymbolSection(const std::string& content, const std::string& sectionName) {
    std::string sectionStart = "\"" + sectionName + "\"";
    size_t sectionPos = content.find(sectionStart);
    if (sectionPos == std::string::npos) {
        return;
    }

    // 找到该部分的开始和结束
    size_t braceStart = content.find("{", sectionPos);
    if (braceStart == std::string::npos) return;

    size_t braceEnd = findMatchingBrace(content, braceStart);
    if (braceEnd == std::string::npos) return;

    std::string sectionContent = content.substr(braceStart + 1, braceEnd - braceStart - 1);

    // 解析每个符号映射
    size_t pos = 0;
    while (pos < sectionContent.length()) {
        size_t keyStart = sectionContent.find("\"", pos);
        if (keyStart == std::string::npos) break;

        size_t keyEnd = sectionContent.find("\"", keyStart + 1);
        if (keyEnd == std::string::npos) break;

        std::string key = sectionContent.substr(keyStart + 1, keyEnd - keyStart - 1);

        // 查找数组开始
        size_t arrayStart = sectionContent.find("[", keyEnd);
        if (arrayStart == std::string::npos) break;

        size_t arrayEnd = sectionContent.find("]", arrayStart);
        if (arrayEnd == std::string::npos) break;

        std::string arrayContent = sectionContent.substr(arrayStart + 1, arrayEnd - arrayStart - 1);

        // 解析数组中的符号
        std::vector<std::string> symbols = parseArray(arrayContent);

        // 映射符号到对应的TokenType
        TokenType tokenType = getTokenTypeFromKey(key);
        if (tokenType != TokenType::UNKNOWN) {
            for (const auto& symbol : symbols) {
                if (!symbol.empty()) {
                    symbolToTokenType[symbol] = tokenType;
                    unicodeMappings[symbol] = tokenTypeToString(tokenType);
                }
            }
        }

        pos = arrayEnd + 1;
    }
}

void SymbolConfigLoader::parseKeywords(const std::string& content) {
    std::string keywordSection = "\"keywords\"";
    size_t sectionPos = content.find(keywordSection);
    if (sectionPos == std::string::npos) {
        return;
    }

    size_t braceStart = content.find("{", sectionPos);
    if (braceStart == std::string::npos) return;

    size_t braceEnd = findMatchingBrace(content, braceStart);
    if (braceEnd == std::string::npos) return;

    std::string sectionContent = content.substr(braceStart + 1, braceEnd - braceStart - 1);

    // 解析关键字映射
    size_t pos = 0;
    while (pos < sectionContent.length()) {
        size_t keyStart = sectionContent.find("\"", pos);
        if (keyStart == std::string::npos) break;

        size_t keyEnd = sectionContent.find("\"", keyStart + 1);
        if (keyEnd == std::string::npos) break;

        std::string chineseWord = sectionContent.substr(keyStart + 1, keyEnd - keyStart - 1);

        size_t valueStart = sectionContent.find("\"", keyEnd + 1);
        if (valueStart == std::string::npos) break;

        size_t valueEnd = sectionContent.find("\"", valueStart + 1);
        if (valueEnd == std::string::npos) break;

        std::string tokenTypeName = sectionContent.substr(valueStart + 1, valueEnd - valueStart - 1);

        // 存储中文关键字映射
        chineseKeywords[chineseWord] = tokenTypeName;
        unicodeMappings[chineseWord] = tokenTypeName;

        pos = valueEnd + 1;
    }
}

void SymbolConfigLoader::parseBuiltinFunctions(const std::string& content) {
    std::string functionSection = "\"builtin_functions\"";
    size_t sectionPos = content.find(functionSection);
    if (sectionPos == std::string::npos) {
        return;
    }

    size_t braceStart = content.find("{", sectionPos);
    if (braceStart == std::string::npos) return;

    size_t braceEnd = findMatchingBrace(content, braceStart);
    if (braceEnd == std::string::npos) return;

    std::string sectionContent = content.substr(braceStart + 1, braceEnd - braceStart - 1);

    // 解析内置函数映射
    size_t pos = 0;
    while (pos < sectionContent.length()) {
        size_t keyStart = sectionContent.find("\"", pos);
        if (keyStart == std::string::npos) break;

        size_t keyEnd = sectionContent.find("\"", keyStart + 1);
        if (keyEnd == std::string::npos) break;

        std::string functionName = sectionContent.substr(keyStart + 1, keyEnd - keyStart - 1);

        // 查找数组开始
        size_t arrayStart = sectionContent.find("[", keyEnd);
        if (arrayStart == std::string::npos) break;

        size_t arrayEnd = sectionContent.find("]", arrayStart);
        if (arrayEnd == std::string::npos) break;

        std::string arrayContent = sectionContent.substr(arrayStart + 1, arrayEnd - arrayStart - 1);

        // 解析数组中的函数名变体
        std::vector<std::string> variants = parseArray(arrayContent);

        builtinFunctions[functionName] = variants;

        pos = arrayEnd + 1;
    }
}

size_t SymbolConfigLoader::findMatchingBrace(const std::string& content, size_t startPos) {
    int braceCount = 1;
    size_t pos = startPos + 1;

    while (pos < content.length() && braceCount > 0) {
        if (content[pos] == '{') {
            braceCount++;
        } else if (content[pos] == '}') {
            braceCount--;
        }
        pos++;
    }

    return (braceCount == 0) ? pos - 1 : std::string::npos;
}

std::vector<std::string> SymbolConfigLoader::parseArray(const std::string& arrayContent) {
    std::vector<std::string> result;

    size_t pos = 0;
    while (pos < arrayContent.length()) {
        size_t start = arrayContent.find("\"", pos);
        if (start == std::string::npos) break;

        size_t end = arrayContent.find("\"", start + 1);
        if (end == std::string::npos) break;

        std::string item = arrayContent.substr(start + 1, end - start - 1);
        if (!item.empty()) {
            result.push_back(item);
        }

        pos = end + 1;
    }

    return result;
}

TokenType SymbolConfigLoader::getTokenTypeFromKey(const std::string& key) {
    static std::unordered_map<std::string, TokenType> keyToTokenType = {
        // core_symbols
        {"import", TokenType::IMPORT},
        {"variable_declaration", TokenType::QUESTION},
        {"conditional_assign", TokenType::CONDITIONAL_ASSIGN},
        {"return_arrow", TokenType::RETURN_ARROW},
        {"struct_def", TokenType::STRUCT_DEF},
        {"impl_def", TokenType::IMPL_DEF},
        {"arrow", TokenType::ARROW},
        {"interface", TokenType::INTERFACE},
        {"enum", TokenType::ENUM},
        {"loop", TokenType::LOOP},
        {"constant", TokenType::CONSTANT},
        {"break_stmt", TokenType::BREAK_STMT},
        {"self_ref", TokenType::SELF_REF},
        {"variable_prefix", TokenType::VARIABLE_PREFIX},

        // operators
        {"assign", TokenType::ASSIGN},
        {"plus", TokenType::PLUS},
        {"minus", TokenType::MINUS},
        {"star", TokenType::STAR},
        {"slash", TokenType::SLASH},
        {"equal", TokenType::EQUAL},
        {"not_equal", TokenType::NOT_EQUAL},
        {"less_than", TokenType::LESS_THAN},
        {"greater_than", TokenType::GREATER_THAN},
        {"less_equal", TokenType::LESS_EQUAL},
        {"greater_equal", TokenType::GREATER_EQUAL},
        {"logical_and", TokenType::LOGICAL_AND},
        {"logical_or", TokenType::LOGICAL_OR},
        {"logical_not", TokenType::LOGICAL_NOT},
        {"plus_assign", TokenType::PLUS_ASSIGN},
        {"minus_assign", TokenType::MINUS_ASSIGN},

        // punctuation
        {"left_paren", TokenType::LEFT_PAREN},
        {"right_paren", TokenType::RIGHT_PAREN},
        {"left_brace", TokenType::LEFT_BRACE},
        {"right_brace", TokenType::RIGHT_BRACE},
        {"left_bracket", TokenType::LEFT_BRACKET},
        {"right_bracket", TokenType::RIGHT_BRACKET},
        {"comma", TokenType::COMMA},
        {"semicolon", TokenType::SEMICOLON},
        {"colon", TokenType::COLON},
        {"dot", TokenType::DOT},
        {"question", TokenType::QUESTION}
    };

    auto it = keyToTokenType.find(key);
    return (it != keyToTokenType.end()) ? it->second : TokenType::UNKNOWN;
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