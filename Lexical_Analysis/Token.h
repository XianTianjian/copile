#pragma once
#include <string>

enum class TokenType {
    // 基本类别（按题目类别码名称）
    IDENFR, INTCON, STRCON,
    CONSTTK, INTTK, MAINTK, BREAKTK, CONTINUETK, IFTK, ELSETK,
    FORTK, PRINTFTK, RETURNTK, VOIDTK, STATICTK,
    // 运算符/界符
    PLUS, MINU, MULT, DIV, MOD,
    LSS, LEQ, GRE, GEQ, EQL, NEQ, ASSIGN,
    NOT, AND, OR,
    SEMICN, COMMA, LPARENT, RPARENT, LBRACK, RBRACK, LBRACE, RBRACE,
    UNKNOWN
};

inline std::string tokenTypeToString(TokenType t) {
    switch (t) {
        case TokenType::IDENFR: return "IDENFR";
        case TokenType::INTCON: return "INTCON";
        case TokenType::STRCON: return "STRCON";
        case TokenType::CONSTTK: return "CONSTTK";
        case TokenType::INTTK: return "INTTK";
        case TokenType::MAINTK: return "MAINTK";
        case TokenType::BREAKTK: return "BREAKTK";
        case TokenType::CONTINUETK: return "CONTINUETK";
        case TokenType::IFTK: return "IFTK";
        case TokenType::ELSETK: return "ELSETK";
        case TokenType::FORTK: return "FORTK";
        case TokenType::PRINTFTK: return "PRINTFTK";
        case TokenType::RETURNTK: return "RETURNTK";
        case TokenType::VOIDTK: return "VOIDTK";
        case TokenType::STATICTK: return "STATICTK";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINU: return "MINU";
        case TokenType::MULT: return "MULT";
        case TokenType::DIV: return "DIV";
        case TokenType::MOD: return "MOD";
        case TokenType::LSS: return "LSS";
        case TokenType::LEQ: return "LEQ";
        case TokenType::GRE: return "GRE";
        case TokenType::GEQ: return "GEQ";
        case TokenType::EQL: return "EQL";
        case TokenType::NEQ: return "NEQ";
        case TokenType::ASSIGN: return "ASSIGN";
        case TokenType::NOT: return "NOT";
        case TokenType::AND: return "AND";
        case TokenType::OR: return "OR";
        case TokenType::SEMICN: return "SEMICN";
        case TokenType::COMMA: return "COMMA";
        case TokenType::LPARENT: return "LPARENT";
        case TokenType::RPARENT: return "RPARENT";
        case TokenType::LBRACK: return "LBRACK";
        case TokenType::RBRACK: return "RBRACK";
        case TokenType::LBRACE: return "LBRACE";
        case TokenType::RBRACE: return "RBRACE";
        default: return "UNKNOWN";
    }
}

struct Token {
    TokenType type;
    std::string lexeme; // 原样字符串（例如数字的原始字符、字符串要含双引号）
    int line;
    Token(TokenType t = TokenType::UNKNOWN, const std::string &s = "", int l = 1)
        : type(t), lexeme(s), line(l) {}
};
