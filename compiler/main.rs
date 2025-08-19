mod lexer;
mod parser;
mod ast;
mod semantic;
mod codegen;
mod error;

use std::env;
use std::fs;
use std::process;

use lexer::Lexer;
use parser::Parser;
use semantic::SemanticAnalyzer;
use codegen::CodeGenerator;
use error::CompilerError;

fn main() {
    let args: Vec<String> = env::args().collect();

    if args.len() != 2 {
        eprintln!("用法: polyglot <源文件.pg>");
        process::exit(1);
    }

    let filename = &args[1];

    // 读取源代码文件
    let source_code = match fs::read_to_string(filename) {
        Ok(content) => content,
        Err(err) => {
            eprintln!("错误: 无法读取文件 '{}': {}", filename, err);
            process::exit(1);
        }
    };

    // 编译流程
    match compile(&source_code, filename) {
        Ok(output) => {
            println!("编译成功！");
            println!("生成的代码:");
            println!("{}", output);
        }
        Err(err) => {
            eprintln!("编译错误: {}", err);
            process::exit(1);
        }
    }
}

fn compile(source_code: &str, filename: &str) -> Result<String, CompilerError> {
    println!("🚀 开始编译 polyglot 程序: {}", filename);

    // 1. 词法分析 (Lexical Analysis)
    println!("📝 步骤 1: 词法分析...");
    let mut lexer = Lexer::new(source_code);
    let tokens = lexer.tokenize()?;
    println!("   ✅ 生成了 {} 个token", tokens.len());

    // 2. 语法分析 (Syntax Analysis)
    println!("🔍 步骤 2: 语法分析...");
    let mut parser = Parser::new(tokens);
    let ast = parser.parse()?;
    println!("   ✅ 生成了抽象语法树");

    // 3. 语义分析 (Semantic Analysis)
    println!("🧠 步骤 3: 语义分析...");
    let mut semantic_analyzer = SemanticAnalyzer::new();
    semantic_analyzer.analyze(&ast)?;
    println!("   ✅ 语义检查通过");

    // 4. 代码生成 (Code Generation)
    println!("⚙️  步骤 4: 代码生成...");
    let mut code_generator = CodeGenerator::new();
    let output = code_generator.generate(&ast)?;
    println!("   ✅ 代码生成完成");

    Ok(output)
}