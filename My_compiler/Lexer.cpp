// Lexer.cpp
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
    // 词法分析阶段唯一的错误类型：非法符号 & 或 |
    errorCodeMap["single&"] = "a";
    errorCodeMap["single|"] = "a";
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
            char next = get();
            s.push_back(next);
            continue;
        }
    }
    if (!closed) {
        // 字符串未闭合错误，按题目要求，词法阶段不处理 → 可忽略
        tokens.emplace_back(TokenType::STRCON, s, startLine);
    } else {
        tokens.emplace_back(TokenType::STRCON, s, startLine);
    }
}

void Lexer::readOperatorOrDelimiter() {
    int startLine = line;
    char c = peek();

    // 特殊处理 & 和 |
    if (c == '&') {
        get();
        if (peek() == '&') {
            get(); tokens.emplace_back(TokenType::AND, "&&", startLine);
        } else {
            recordError(startLine, errorCodeMap["single&"]);
            tokens.emplace_back(TokenType::UNKNOWN, "&", startLine);
        }
        return;
    }
    if (c == '|') {
        get();
        if (peek() == '|') {
            get(); tokens.emplace_back(TokenType::OR, "||", startLine);
        } else {
            recordError(startLine, errorCodeMap["single|"]);
            tokens.emplace_back(TokenType::UNKNOWN, "|", startLine);
        }
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

    // 单字符符号
    get();
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
            // 其他非法字符 → 词法阶段忽略错误，不记录
            tokens.emplace_back(TokenType::UNKNOWN, s, startLine);
            return;
    }
}

void Lexer::recordError(int lineNo, const std::string &code) {
    errors.emplace_back(lineNo, code);
}

void Lexer::writeOutputs(const std::string &lexerFile, const std::string &errorFile) {
    if (!errors.empty()) {
        std::sort(errors.begin(), errors.end(), [](auto &a, auto &b){
            if (a.first != b.first) return a.first < b.first;
            return a.second < b.second;
        });
        std::ofstream ofs(errorFile);
        for (size_t i = 0; i < errors.size(); ++i) {
            ofs << errors[i].first << " " << errors[i].second;
            if (i != errors.size() - 1) ofs << "\n";
        }
        ofs.close();
    } else {
        std::ofstream ofs(lexerFile);
        for (size_t i = 0; i < tokens.size(); ++i) {
            ofs << tokenTypeToString(tokens[i].type) << " " << tokens[i].lexeme;
            if (i != tokens.size() - 1) ofs << "\n";
        }
        ofs.close();
    }
}
