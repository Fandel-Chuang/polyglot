#pragma once

#include "ast.h"

class SemanticAnalyzer {
public:
    void analyze(const std::unique_ptr<Program>& program);
};