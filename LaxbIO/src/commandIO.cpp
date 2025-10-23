// ======================================================================
// command -opt :varn ::varn {a b c} [a b c; d e f];
// ======================================================================

#include "commandIO.hpp"
#include "function_manager.hpp"
#include "utils/output.hpp"
#include <iostream>
#include <string>
#include <vector>

namespace laxb {

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

    /* 简单检查正确性，并转换为 tokens */
    std::vector<std::string> tokens;
    std::size_t endPos{0}, begPos{0}; // 结束位置下标

    while (!cmdLine.empty() && *(cmdLine.end() - 1) == ' ') // 删掉末尾所有空格
        cmdLine.pop_back();
    while (!cmdLine.empty() && *cmdLine.begin() == ' ') { // 删掉开头所有空格
        auto begIt = cmdLine.begin();
        cmdLine.erase(begIt);
    }
    if (cmdLine.empty()) // 什么都不做，等待重新输入
        return {};

    /* 最后一个字符为 ';' 时，在它之前插入一个空格，以分出 token */
    auto tmpIt = cmdLine.end();
    if (*(--tmpIt) == ';')
        cmdLine.insert(tmpIt, ' ');

    std::size_t cmdlSize = cmdLine.size();
    while (true) {
        /* 普通字符起点 */
        if (cmdLine[begPos] != '{' && cmdLine[begPos] != '[')
            endPos = cmdLine.find(" ", begPos);

        /* 括号起点 */
        if (cmdLine[begPos] == '{' || cmdLine[begPos] == '[') {
            if (cmdLine[begPos] == '{')
                endPos = cmdLine.find("}", begPos);
            else if (cmdLine[begPos] == '[')
                endPos = cmdLine.find("] ", begPos);

            if (endPos != std::string::npos) { // 找到反括号时
                if (endPos == cmdlSize - 1) // 最后一个字符为括号时
                    break;

                ++endPos; // 不为最后一个字符则递增

                if (cmdLine[endPos] != ' ') { // 递增之后，当前位置应该为空格（最后一个为括号例外）
                    std::cout << util::oup::ERS() + "CmdParser::in(): Missing spaces after brackests\n";
                    return {};
                }
            }
            else if (endPos == std::string::npos) { // 未找到反括号时，处理找不到反括号的情况
                std::cout << util::oup::ERS() + "CmdParser::in(): Missing a backbrackets\n";
                return {};
            }
        }

        if (endPos == std::string::npos) // 循环结束标志，处理找不到空格的情况
            break;

        /* 此时为多个空格（begPos开始查找第一个空格，而第一个空格是自身） */
        if (begPos == endPos && endPos != cmdlSize - 1) {
            ++begPos;
            continue;
        }

        tokens.push_back(cmdLine.substr(begPos, endPos - begPos));
        begPos = endPos + 1; // 上段保证前后无空格，此处 endPos 右边至少有一个非空格字符，故可以不检查而直接赋值
    }
    tokens.push_back(cmdLine.substr(begPos, cmdlSize - begPos)); // 最后一个字符串

    /* 逐项 tokens 检查 */
    if (tksCheck(tokens) == false)
        return {};

    return tokens;
}

/* 检查 tokens，返回布尔值 */
bool tksCheck(std::vector<std::string> tks) {
    bool allow{true}; // 一次性，此处只检查第一个命令
    std::vector<std::string> tksNoid; // 去除标识符的 tokens

    auto it = tks.end() - 1; // 此处一定为非空，可以直接计算
    if (*it == ";") // 删掉末尾 ;
        tks.erase(it);

    for (auto& tk : tks) {
        if (tk.size() < 2) { // 所有命令的参数（包括命令）都至少有两位或以上（分号除外）
            std::cout << util::oup::ERS() + "CmdParser::in(): Argument(s) error\n";
            return false;
        }

        /* 命令检查 */
        if (allow) {
            if (fnMgr.fnFind(tk) == false) {
                std::cout << util::oup::ERS() + "CmdParser::in(): Command '" + tk + "' not found\n";
                return false;
            }
            allow = false;
            continue; // 结束命令检查，接下来为参数
        }

        /* 参数检查，已经保证 [0][1] 可以访问 */
        std::size_t inx{1}; // 子串写入位置
        if (tk[0] == '-') // -p
            inx = 1;
        else if (tk[0] == ':' && tk[1] != ':') // :varn
            inx = 1;
        else if (tk[0] == ':' && tk[1] == ':') { // ::varn
            if (tk.size() < 3) {
                std::cout << util::oup::ERS() + "tksCheck(): Argument error after identifier \"::\"\n";
                return false;
            }
            inx = 2;
        }
        else if (tk[0] == '{') { // {a b ...}
            if (tk[tk.size() - 1] != '}') {
                std::cout << util::oup::ERS() + "tksCheck(): Missing a backbrackets '}'\n";
                return false;
            }
            inx = 1;
        }
        else if (tk[0] == '[') { // [a b; c d; ...]
            if (tk[tk.size() - 1] != ']') {
                std::cout << util::oup::ERS() + "tksCheck(): Missing a backbrackets ']'\n";
                return false;
            }
            inx = 1;
        }
        else {
            std::cout << util::oup::ERS() + "tksCheck(): Unknown argument(s)\n";
            return false;
        }
        tksNoid.push_back(tk.substr(inx));
    }

    const std::string legalCh{"1234567890abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ"}; // 这些字符之外的字符都为非法，禁止出现在除去标识符的参数中
    for (const auto& tkNoid : tksNoid) {
        if (tkNoid.find_first_not_of(legalCh) != std::string::npos) { // 找到非法字符时
            std::cout << util::oup::ERS() + "tksCheck(): Illegal argument(s) in \"" + tkNoid + "\"\n";
            return false;
        }
    }

    return true; // 都通过时
}

} // namespace laxb