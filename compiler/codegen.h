#pragma once

#include "ast.h"
#include <string>

class CodeGenerator {
public:
    std::string generate(const std::unique_ptr<Program>& program);
};