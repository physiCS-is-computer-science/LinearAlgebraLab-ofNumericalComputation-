// ======================================================================
// command -opt :varn ::varn [a b c; d e f] {a b c};
// ======================================================================

#include "commandIO.hpp"
#include "function_manager.hpp"
#include <iostream>
#include <string>
#include <vector>

CmdParser cmdpr;

/* 唯一命令输入函数，内部调用 getline()
 * - 仅简单检查命令语法正确与否，命令是否存在等
 * - 若成功则返回分解完毕的命令 tokens，若失败则返回空 vector */
std::vector<std::string> CmdParser::in() {
    /* input command string */
    std::string cmdLine{""};
    getline(std::cin, cmdLine);
    if (cmdLine.empty()) // 确保不为空
        return {};

    /* 检查正确性，并转换为 tokens */
    std::vector<std::string> tokens;
    std::size_t endPos{0}, begPos{0}; // 结束位置下标
    std::size_t cmdlSize = cmdLine.size();

    while (!cmdLine.empty() && *(cmdLine.end() - 1) == ' ') // 删掉末尾所有空格
        cmdLine.pop_back();
    while (!cmdLine.empty() && *cmdLine.begin() == ' ') { // 删掉开头所有空格
        auto begIt = cmdLine.begin();
        cmdLine.erase(begIt);
    }
    if (cmdLine.empty()) // 什么都不做，等待重新输入
        return {};

    while (true) {
        if (cmdLine[begPos] != '{' && cmdLine[begPos] != '[')
            endPos = cmdLine.find(" ", begPos);
        else if (cmdLine[begPos] == '{') {
            endPos = cmdLine.find("} ", begPos); // 括号后跟着空格的子串
            if (endPos != std::string::npos)
                ++endPos;
        }
        else if (cmdLine[begPos] == '[') {
            endPos = cmdLine.find("] ", begPos); // 括号后跟着空格的子串
            if (endPos != std::string::npos)
                ++endPos;
        }

        if (endPos == std::string::npos)
            break;

        if (begPos == endPos && endPos != cmdlSize - 1) { // 此时为多个空格（begPos开始查找第一个空格，而第一个空格是自身）
            ++begPos;
            continue;
        }

        tokens.push_back(cmdLine.substr(begPos, endPos - begPos));
        begPos = endPos + 1; // 上段保证前后无空格，此处 endPos 右边至少有一个非空格字符，故可以不检查而直接赋值
    }
    tokens.push_back(cmdLine.substr(begPos, cmdlSize - begPos)); // 最后一个字符串

    return tokens;
}
