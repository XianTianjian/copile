#include "Lexer.h"
#include <iostream>

int main(int argc, char **argv) {
    // 默认读取 testfile.txt，输出 lexer.txt 或 error.txt
    std::string infile = "testfile.txt";
    if (argc >= 2) infile = argv[1];

    Lexer lexer(infile);
    lexer.tokenize();
    lexer.writeOutputs("lexer.txt", "error.txt");

    // 提示（可删除）
    // std::cout << "Lexing finished. ";
    std::ifstream ef("error.txt");
    if (ef.good()) {
        // 如果有 error.txt 内容则告诉用户
        std::cout << "If errors found, see error.txt\n";
    } else {
        // std::cout << "see lexer.txt\n";
    }
    return 0;
}
