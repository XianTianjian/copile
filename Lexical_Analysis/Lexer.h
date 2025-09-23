#pragma once
#include "Token.h"
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>

class Lexer {
public:
    Lexer(const std::string &inputFile);
    void tokenize(); // 执行词法分析
    void writeOutputs(const std::string &lexerFile, const std::string &errorFile);

    // 修改错误码映射（若你有完整映射可在外部设置）
    void setErrorCodeFor(const std::string &key, const std::string &code);

private:
    std::string input;
    size_t pos;
    int line;
    std::vector<Token> tokens;
    std::vector<std::pair<int,std::string>> errors; // (line, errorCode)

    std::unordered_map<std::string, TokenType> keywords;
    std::unordered_map<std::string, std::string> errorCodeMap;

    void initKeywords();
    void initDefaultErrorMap();

    char peek() const;
    char get();
    bool eof() const;

    void skipWhitespaceAndComments();
    void readIdentifierOrKeyword();
    void readNumber();
    void readString();
    void readOperatorOrDelimiter();
    void recordError(int lineNo, const std::string &code);
};
