#include "Lexer.h"
#include <cctype>
#include <iostream>
#include <algorithm>

Lexer::Lexer(const std::string &inputFile) : pos(0), line(1) {
    // 读整个文件
    std::ifstream ifs(inputFile);
    if (!ifs) {
        std::cerr << "Cannot open input file: " << inputFile << "\n";
        exit(1);
    }
    input.assign((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    initKeywords();
    initDefaultErrorMap();
}

void Lexer::initKeywords() {
    keywords = {
        {"const", TokenType::CONSTTK},
        {"int", TokenType::INTTK},
        {"main", TokenType::MAINTK},
        {"break", TokenType::BREAKTK},
        {"continue", TokenType::CONTINUETK},
        {"if", TokenType::IFTK},
        {"else", TokenType::ELSETK},
        {"for", TokenType::FORTK},
        {"printf", TokenType::PRINTFTK},
        {"return", TokenType::RETURNTK},
        {"void", TokenType::VOIDTK},
        {"static", TokenType::STATICTK}
    };
}

void Lexer::initDefaultErrorMap() {
    // 默认错误码映射（可修改）
    errorCodeMap["single&"] = "a"; // 样例中单独 '&' 对应 a
    errorCodeMap["single|"] = "b";
    errorCodeMap["unterminated_string"] = "c";
    errorCodeMap["illegal_char"] = "d";
}

void Lexer::setErrorCodeFor(const std::string &key, const std::string &code) {
    errorCodeMap[key] = code;
}

char Lexer::peek() const {
    if (pos >= input.size()) return '\0';
    return input[pos];
}
char Lexer::get() {
    if (pos >= input.size()) return '\0';
    char c = input[pos++];
    if (c == '\n') ++line;
    return c;
}
bool Lexer::eof() const { return pos >= input.size(); }

void Lexer::skipWhitespaceAndComments() {
    bool again = true;
    while (again) {
        again = false;
        // 空白
        while (!eof() && std::isspace((unsigned char)peek())) get();

        // 行注释 // .... 到行尾
        if (!eof() && peek() == '/') {
            if (pos + 1 < input.size() && input[pos+1] == '/') {
                get(); get();
                while (!eof() && peek() != '\n') get();
                again = true;
                continue;
            }
            // 块注释 /* ... */
            if (pos + 1 < input.size() && input[pos+1] == '*') {
                get(); get();
                while (!eof()) {
                    char c = get();
                    if (c == '\0') break;
                    if (c == '*' && peek() == '/') { get(); break; }
                }
                again = true;
                continue;
            }
        }
    }
}

void Lexer::tokenize() {
    while (!eof()) {
        skipWhitespaceAndComments();
        if (eof()) break;
        char c = peek();
        if (std::isalpha((unsigned char)c) || c == '_') {
            readIdentifierOrKeyword();
        } else if (std::isdigit((unsigned char)c)) {
            readNumber();
        } else if (c == '"') {
            readString();
        } else {
            readOperatorOrDelimiter();
        }
    }
}

void Lexer::readIdentifierOrKeyword() {
    int startLine = line;
    std::string s;
    while (!eof() && (std::isalnum((unsigned char)peek()) || peek() == '_')) {
        s.push_back(get());
    }
    auto it = keywords.find(s);
    if (it != keywords.end()) {
        tokens.emplace_back(it->second, s, startLine);
    } else {
        tokens.emplace_back(TokenType::IDENFR, s, startLine);
    }
}

void Lexer::readNumber() {
    int startLine = line;
    std::string s;
    while (!eof() && std::isdigit((unsigned char)peek())) s.push_back(get());
    // 题目中只需要整数（INTCON），不做浮点支持
    tokens.emplace_back(TokenType::INTCON, s, startLine);
}

void Lexer::readString() {
    int startLine = line;
    std::string s;
    s.push_back(get()); // consume opening "
    bool closed = false;
    while (!eof()) {
        char c = get();
        s.push_back(c);
        if (c == '"') { closed = true; break; }
        if (c == '\\' && !eof()) {
            // 支持简单转义，保留原样
            char next = get();
            s.push_back(next);
            continue;
        }
    }
    if (!closed) {
        // 未闭合字符串 -> 记录错误，但仍记录 token as STRCON with what we have (题目可能期望只报错)
        recordError(startLine, errorCodeMap["unterminated_string"]);
        tokens.emplace_back(TokenType::STRCON, s, startLine);
    } else {
        tokens.emplace_back(TokenType::STRCON, s, startLine);
    }
}

void Lexer::readOperatorOrDelimiter() {
    int startLine = line;
    char c = peek();

    // two-char ops first
    if (c == '&') {
        get();
        if (peek() == '&') { get(); tokens.emplace_back(TokenType::AND, "&&", startLine); }
        else { // 单独 '&' -> 词法错误 (a)
            recordError(startLine, errorCodeMap["single&"]);
            // we still add unknown token for completeness
            std::string s = "&";
            tokens.emplace_back(TokenType::UNKNOWN, s, startLine);
        }
        return;
    }
    if (c == '|') {
        get();
        if (peek() == '|') { get(); tokens.emplace_back(TokenType::OR, "||", startLine); }
        else { recordError(startLine, errorCodeMap["single|"]); std::string s="|"; tokens.emplace_back(TokenType::UNKNOWN,s,startLine); }
        return;
    }
    if (c == '=') {
        get();
        if (peek() == '=') { get(); tokens.emplace_back(TokenType::EQL, "==", startLine); }
        else { tokens.emplace_back(TokenType::ASSIGN, "=", startLine); }
        return;
    }
    if (c == '!') {
        get();
        if (peek() == '=') { get(); tokens.emplace_back(TokenType::NEQ, "!=", startLine); }
        else { tokens.emplace_back(TokenType::NOT, "!", startLine); }
        return;
    }
    if (c == '<') {
        get();
        if (peek() == '=') { get(); tokens.emplace_back(TokenType::LEQ, "<=", startLine); }
        else { tokens.emplace_back(TokenType::LSS, "<", startLine); }
        return;
    }
    if (c == '>') {
        get();
        if (peek() == '=') { get(); tokens.emplace_back(TokenType::GEQ, ">=", startLine); }
        else { tokens.emplace_back(TokenType::GRE, ">", startLine); }
        return;
    }

    // single-character tokens
    get(); // consume c
    std::string s(1, c);
    switch (c) {
        case '+': tokens.emplace_back(TokenType::PLUS, s, startLine); return;
        case '-': tokens.emplace_back(TokenType::MINU, s, startLine); return;
        case '*': tokens.emplace_back(TokenType::MULT, s, startLine); return;
        case '/': tokens.emplace_back(TokenType::DIV, s, startLine); return;
        case '%': tokens.emplace_back(TokenType::MOD, s, startLine); return;
        case ';': tokens.emplace_back(TokenType::SEMICN, s, startLine); return;
        case ',': tokens.emplace_back(TokenType::COMMA, s, startLine); return;
        case '(': tokens.emplace_back(TokenType::LPARENT, s, startLine); return;
        case ')': tokens.emplace_back(TokenType::RPARENT, s, startLine); return;
        case '[': tokens.emplace_back(TokenType::LBRACK, s, startLine); return;
        case ']': tokens.emplace_back(TokenType::RBRACK, s, startLine); return;
        case '{': tokens.emplace_back(TokenType::LBRACE, s, startLine); return;
        case '}': tokens.emplace_back(TokenType::RBRACE, s, startLine); return;
        default:
            // 非法字符
            recordError(startLine, errorCodeMap["illegal_char"]);
            tokens.emplace_back(TokenType::UNKNOWN, s, startLine);
            return;
    }
}

void Lexer::recordError(int lineNo, const std::string &code) {
    errors.emplace_back(lineNo, code);
}

void Lexer::writeOutputs(const std::string &lexerFile, const std::string &errorFile) {
    if (!errors.empty()) {
        // 按行号从小到大输出错误（题目要求）
        std::sort(errors.begin(), errors.end(), [](auto &a, auto &b){
            if (a.first != b.first) return a.first < b.first;
            return a.second < b.second;
        });
        std::ofstream ofs(errorFile);
        for (auto &e : errors) {
            ofs << e.first << " " << e.second << "\n";
        }
        ofs.close();
    } else {
        // 输出 tokens 按读取顺序
        std::ofstream ofs(lexerFile);
        for (auto &t : tokens) {
            ofs << tokenTypeToString(t.type) << " " << t.lexeme << "\n";
        }
        ofs.close();
    }
}
