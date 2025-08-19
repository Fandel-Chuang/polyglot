use std::fmt;

#[derive(Debug, Clone)]
pub enum CompilerError {
    LexerError {
        message: String,
        line: usize,
        column: usize,
    },
    ParserError {
        message: String,
        line: usize,
        column: usize,
    },
    SemanticError {
        message: String,
        line: usize,
        column: usize,
    },
    CodeGenError {
        message: String,
    },
    IOError {
        message: String,
    },
}

impl fmt::Display for CompilerError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self {
            CompilerError::LexerError { message, line, column } => {
                write!(f, "词法错误 (行{}:列{}): {}", line, column, message)
            }
            CompilerError::ParserError { message, line, column } => {
                write!(f, "语法错误 (行{}:列{}): {}", line, column, message)
            }
            CompilerError::SemanticError { message, line, column } => {
                write!(f, "语义错误 (行{}:列{}): {}", line, column, message)
            }
            CompilerError::CodeGenError { message } => {
                write!(f, "代码生成错误: {}", message)
            }
            CompilerError::IOError { message } => {
                write!(f, "IO错误: {}", message)
            }
        }
    }
}

impl std::error::Error for CompilerError {}

impl From<std::io::Error> for CompilerError {
    fn from(err: std::io::Error) -> Self {
        CompilerError::IOError {
            message: err.to_string(),
        }
    }
}