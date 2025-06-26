#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <map>
#include <iomanip>

// 单词符号类型枚举
enum class TokenType {
    KEYWORD, IDENTIFIER,
    INTEGER_CONST, FLOAT_CONST, CHAR_CONST, STRING_LITERAL,
    OPERATOR, DELIMITER,
    COMMENT, PREPROCESSOR,
    ERROR, END_OF_FILE
};

// 将TokenType转换为字符串以便打印
std::map<TokenType, std::string> token_type_names = {
    {TokenType::KEYWORD,         "关键字"},
    {TokenType::IDENTIFIER,      "标识符"},
    {TokenType::INTEGER_CONST,   "整型常量"},
    {TokenType::FLOAT_CONST,     "浮点型常量"},
    {TokenType::CHAR_CONST,      "字符常量"},
    {TokenType::STRING_LITERAL,  "字符串字面量"},
    {TokenType::OPERATOR,        "运算符"},
    {TokenType::DELIMITER,       "分隔符"},
    {TokenType::COMMENT,         "注释"},
    {TokenType::PREPROCESSOR,    "预处理器指令"},
    {TokenType::ERROR,           "错误"},
    {TokenType::END_OF_FILE,     "文件结束符"}
};

// Token结构体，用于存储单词信息
struct Token {
    TokenType type;
    std::string value;
    int line;
};

// C语言关键字集合
const std::map<std::string, TokenType> keywords = {
    {"auto", TokenType::KEYWORD}, {"break", TokenType::KEYWORD}, {"case", TokenType::KEYWORD},
    {"char", TokenType::KEYWORD}, {"const", TokenType::KEYWORD}, {"continue", TokenType::KEYWORD},
    {"default", TokenType::KEYWORD}, {"do", TokenType::KEYWORD}, {"double", TokenType::KEYWORD},
    {"else", TokenType::KEYWORD}, {"enum", TokenType::KEYWORD}, {"extern", TokenType::KEYWORD},
    {"float", TokenType::KEYWORD}, {"for", TokenType::KEYWORD}, {"goto", TokenType::KEYWORD},
    {"if", TokenType::KEYWORD}, {"int", TokenType::KEYWORD}, {"long", TokenType::KEYWORD},
    {"register", TokenType::KEYWORD}, {"return", TokenType::KEYWORD}, {"short", TokenType::KEYWORD},
    {"signed", TokenType::KEYWORD}, {"sizeof", TokenType::KEYWORD}, {"static", TokenType::KEYWORD},
    {"struct", TokenType::KEYWORD}, {"switch", TokenType::KEYWORD}, {"typedef", TokenType::KEYWORD},
    {"union", TokenType::KEYWORD}, {"unsigned", TokenType::KEYWORD}, {"void", TokenType::KEYWORD},
    {"volatile", TokenType::KEYWORD}, {"while", TokenType::KEYWORD}
};

class Lexer {
public:
    Lexer(const std::string& source) : source_(source), pos_(0), line_(1), char_count_(source.length()) {}

    std::vector<Token> analyze() {
        std::vector<Token> tokens;
        while (pos_ < source_.length()) {
            char current_char = source_[pos_];

            if (isspace(current_char)) {
                if (current_char == '\n') line_++;
                pos_++;
                continue;
            }

            if (current_char == '/' && peek() == '/') {
                tokens.push_back(skip_line_comment());
                continue;
            }

            if (current_char == '/' && peek() == '*') {
                tokens.push_back(skip_block_comment());
                continue;
            }
            
            if (current_char == '#') {
                tokens.push_back(get_preprocessor());
                continue;
            }

            if (isalpha(current_char) || current_char == '_') {
                tokens.push_back(get_identifier_or_keyword());
                continue;
            }

            if (isdigit(current_char)) {
                tokens.push_back(get_number());
                continue;
            }

            if (current_char == '\"') {
                tokens.push_back(get_string_literal());
                continue;
            }

            if (current_char == '\'') {
                tokens.push_back(get_char_literal());
                continue;
            }

            if (is_operator_or_delimiter(current_char)) {
                tokens.push_back(get_operator_or_delimiter());
                continue;
            }

            // 错误处理
            std::string error_val;
            error_val += current_char;
            tokens.push_back({TokenType::ERROR, error_val, line_});
            errors_.push_back("错误在第 " + std::to_string(line_) + " 行: 无效字符 '" + error_val + "'");
            pos_++;
        }
        tokens.push_back({TokenType::END_OF_FILE, "EOF", line_});
        return tokens;
    }

    const std::vector<std::string>& get_errors() const { return errors_; }
    int get_line_count() const { return line_; }
    int get_char_count() const { return char_count_; }

private:
    std::string source_;
    size_t pos_;
    int line_;
    int char_count_;
    std::vector<std::string> errors_;

    char peek() {
        if (pos_ + 1 < source_.length()) {
            return source_[pos_ + 1];
        }
        return '\0';
    }

    Token skip_line_comment() {
        size_t start_pos = pos_;
        while (pos_ < source_.length() && source_[pos_] != '\n') {
            pos_++;
        }
        return {TokenType::COMMENT, source_.substr(start_pos, pos_ - start_pos), line_};
    }

    Token skip_block_comment() {
        size_t start_pos = pos_;
        int start_line = line_;
        pos_ += 2; // Skip '/*'
        while (pos_ + 1 < source_.length() && (source_[pos_] != '*' || source_[pos_ + 1] != '/')) {
            if (source_[pos_] == '\n') line_++;
            pos_++;
        }

        if (pos_ + 1 >= source_.length()) {
            errors_.push_back("错误在第 " + std::to_string(start_line) + " 行: 未结束的块注释。");
            return {TokenType::ERROR, source_.substr(start_pos, pos_ - start_pos), start_line};
        }
        
        pos_ += 2; // Skip '*/'
        return {TokenType::COMMENT, source_.substr(start_pos, pos_ - start_pos), start_line};
    }
    
    Token get_preprocessor() {
        size_t start_pos = pos_;
        while (pos_ < source_.length() && source_[pos_] != '\n') {
            // Handle backslash-newline continuation
            if (source_[pos_] == '\\' && peek() == '\n') {
                pos_ += 2;
                line_++;
            } else {
                pos_++;
            }
        }
        return {TokenType::PREPROCESSOR, source_.substr(start_pos, pos_ - start_pos), line_};
    }

    Token get_identifier_or_keyword() {
        size_t start_pos = pos_;
        while (pos_ < source_.length() && (isalnum(source_[pos_]) || source_[pos_] == '_')) {
            pos_++;
        }
        std::string value = source_.substr(start_pos, pos_ - start_pos);
        if (keywords.count(value)) {
            return {keywords.at(value), value, line_};
        }
        return {TokenType::IDENTIFIER, value, line_};
    }

    Token get_number() {
        size_t start_pos = pos_;
        bool is_float = false;
        while (pos_ < source_.length() && isdigit(source_[pos_])) {
            pos_++;
        }
        if (pos_ < source_.length() && source_[pos_] == '.') {
            is_float = true;
            pos_++;
            while (pos_ < source_.length() && isdigit(source_[pos_])) {
                pos_++;
            }
        }
        if (pos_ < source_.length() && (source_[pos_] == 'e' || source_[pos_] == 'E')) {
            is_float = true;
            pos_++;
            if (pos_ < source_.length() && (source_[pos_] == '+' || source_[pos_] == '-')) {
                pos_++;
            }
            size_t exp_start = pos_;
            while (pos_ < source_.length() && isdigit(source_[pos_])) {
                pos_++;
            }
            if (pos_ == exp_start) {
                 errors_.push_back("错误在第 " + std::to_string(line_) + " 行: 格式错误的数字指数。");
                 return {TokenType::ERROR, source_.substr(start_pos, pos_ - start_pos), line_};
            }
        }

        std::string value = source_.substr(start_pos, pos_ - start_pos);
        if (is_float) {
            return {TokenType::FLOAT_CONST, value, line_};
        }
        return {TokenType::INTEGER_CONST, value, line_};
    }

    Token get_string_literal() {
        size_t start_pos = pos_;
        int start_line = line_;
        pos_++; // Skip opening '"'
        while (pos_ < source_.length() && source_[pos_] != '\"') {
            if (source_[pos_] == '\\') { // Handle escape sequences
                pos_++;
            }
            if (source_[pos_] == '\n') line_++;
            pos_++;
        }
        if (pos_ >= source_.length()) {
            errors_.push_back("错误在第 " + std::to_string(start_line) + " 行: 未结束的字符串字面量。");
            return {TokenType::ERROR, source_.substr(start_pos, pos_ - start_pos), start_line};
        }
        pos_++; // Skip closing '"'
        return {TokenType::STRING_LITERAL, source_.substr(start_pos, pos_ - start_pos), start_line};
    }

    Token get_char_literal() {
        size_t start_pos = pos_;
        int start_line = line_;
        pos_++; // Skip opening '''
        while (pos_ < source_.length() && source_[pos_] != '\'') {
            if (source_[pos_] == '\\') { // Handle escape sequences
                pos_++;
            }
            if (source_[pos_] == '\n') {
                 errors_.push_back("错误在第 " + std::to_string(line_) + " 行: 字符常量中存在换行符。");
                 break;
            }
            pos_++;
        }
        if (pos_ >= source_.length() || source_[pos_] != '\'') {
            errors_.push_back("错误在第 " + std::to_string(start_line) + " 行: 未结束的字符字面量。");
            return {TokenType::ERROR, source_.substr(start_pos, pos_ - start_pos), start_line};
        }
        pos_++; // Skip closing '''
        std::string value = source_.substr(start_pos, pos_ - start_pos);
        if (value.length() > 4 || (value.length() > 3 && value[1] != '\\') || value.length() < 3) {
             errors_.push_back("错误在第 " + std::to_string(start_line) + " 行: 无效的字符字面量 " + value);
        }
        return {TokenType::CHAR_CONST, value, start_line};
    }

    bool is_operator_or_delimiter(char c) {
        std::string chars = "+-*/%<>=!&|~^(){}[];,.:?";
        return chars.find(c) != std::string::npos;
    }

    Token get_operator_or_delimiter() {
        size_t start_pos = pos_;
        char c1 = source_[pos_];
        char c2 = peek();

        // 2-char operators
        std::string op2 = std::string(1, c1) + c2;
        std::vector<std::string> two_char_ops = {"==", "!=", "<=", ">=", "&&", "||", "++", "--", "+=", "-=", "*=", "/=", "%="};
        for (const auto& op : two_char_ops) {
            if (op2 == op) {
                pos_ += 2;
                return {TokenType::OPERATOR, op2, line_};
            }
        }
        
        // 3-char operators (like <<=, >>=) could be added here

        // 1-char operators and delimiters
        pos_++;
        std::string op1(1, c1);
        std::string delimiters = "(){}[];,";
        if (delimiters.find(c1) != std::string::npos) {
            return {TokenType::DELIMITER, op1, line_};
        }
        return {TokenType::OPERATOR, op1, line_};
    }
};

void print_report(const std::vector<Token>& tokens, const Lexer& lexer) {
    std::map<TokenType, int> counts;

    std::cout << "\n--- 词法分析结果 ---\n";
    std::cout << std::left << std::setw(10) << "行号"
              << std::setw(20) << "类型"
              << "值\n";
    std::cout << "--------------------------------------------------\n";

    for (const auto& token : tokens) {
        if (token.type != TokenType::END_OF_FILE) {
            std::cout << std::left << std::setw(10) << token.line
                      << std::setw(20) << token_type_names[token.type]
                      << token.value << "\n";
        }
        if (token.type != TokenType::COMMENT) { // Comments are usually not counted as "words"
             counts[token.type]++;
        }
    }
    
    std::cout << "\n--- 词法错误 ---\n";
    const auto& errors = lexer.get_errors();
    if (errors.empty()) {
        std::cout << "未发现词法错误。\n";
    } else {
        for (const auto& err : errors) {
            std::cout << err << "\n";
        }
    }

    std::cout << "\n--- 统计摘要 ---\n";
    std::cout << "总行数: " << lexer.get_line_count() << "\n";
    std::cout << "总字符数: " << lexer.get_char_count() << "\n";
    std::cout << "总记号数 (不含注释和文件结束符): " << tokens.size() - counts[TokenType::COMMENT] - 1 << "\n";
    std::cout << "\n各类型记号统计:\n";
    for (const auto& pair : counts) {
        if (pair.first != TokenType::END_OF_FILE && pair.first != TokenType::COMMENT) {
            std::cout << "  " << std::left << std::setw(20) << token_type_names[pair.first] << ": " << pair.second << "\n";
        }
    }
}

int main() {
    std::string filename;
    std::cout << "请输入C源文件路径: ";
    std::cin >> filename;

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "错误: 无法打开文件 '" << filename << "'\n";
        return 1;
    }

    std::string source_code((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    std::cout << "正在分析文件: " << filename << "\n";

    Lexer lexer(source_code);
    std::vector<Token> tokens = lexer.analyze();
    
    print_report(tokens, lexer);

    return 0;
}

/*
--- 如何使用 ---
1. 使用C++编译器编译此文件 (main.cpp) (例如: g++ main.cpp -o lexer -std=c++11)。
2. 创建一个C源文件, 例如 'test.c'。
3. 将下面的示例C代码复制到 'test.c' 中。
4. 运行编译后的程序: ./lexer
5. 当提示时, 输入您的C文件路径: test.c

--- 示例 test.c 文件 ---

#include <stdio.h>

// 这是一个单行注释。
int main() {
    int a = 10;
    float b = 20.5;
    char c = 'x';
    
    /* 这是一个块注释
       跨越多行。 */
    
//     if (a > 5) {
//         printf("Hello, World!\\n");
//     }
//
//     // 词法错误
//     int invalid_num = 1.2.3;
//     char* unclosed_str = "hello;
//     int bad$char = 5;
//
//     return 0;
// }

// 未闭合的块注释测试
/*

*/
