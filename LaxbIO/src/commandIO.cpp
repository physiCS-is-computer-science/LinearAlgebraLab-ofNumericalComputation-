// ======================================================================
// command -opt :varn ::varn {a b c} [a b c; d e f];
// ======================================================================

#include "commandIO.hpp"
#include "function_manager.hpp"
#include "utils/output.hpp"
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

namespace laxb {

CmdHandler cmdhr;

/* 唯一命令输入函数，内部调用 getline()
 * - 仅简单检查命令语法正确与否，命令是否存在等
 * - 若成功则返回分解完毕的命令 tokens，若失败则返回空 vector */
bool CmdHandler::in() {
    /* input command string */
    std::string cmdLine{""};
    getline(std::cin, cmdLine);
    if (cmdLine.empty()) // 确保不为空
        return false;

    /* 简单检查正确性，并转换为 tokens */
    std::vector<std::string> tokens;
    std::size_t endPos{0}, begPos{0}; // 结束位置下标

    delSpace(cmdLine); // 去除首尾空格
    if (cmdLine.empty()) // 什么都不做，等待重新输入
        return false;

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
                endPos = cmdLine.find("]", begPos);

            if (endPos != std::string::npos) { // 找到反括号时
                if (endPos == cmdlSize - 1) // 最后一个字符为括号时
                    break;

                ++endPos; // 不为最后一个字符则递增

                if (cmdLine[endPos] != ' ') { // 递增之后，当前位置应该为空格（最后一个为括号例外）
                    std::cout << util::ERS(smr::semgr.getpath()) + "CmdParser::in(): Missing spaces after brackests\n";
                    return false;
                }
            }
            else if (endPos == std::string::npos) { // 未找到反括号时，处理找不到反括号的情况
                std::cout << util::ERS(smr::semgr.getpath()) + "CmdParser::in(): Missing a backbrackets\n";
                return false;
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
        return false;

    laxb::cmdhr.setCmdtoken(tokens);
    return true;
}

/* 检查 tokens，返回布尔值 */
bool tksCheck(std::vector<std::string> tks) {
    bool allow{true}; // 一次性，此处只检查第一个命令
    std::vector<std::string> tksNoid; // 去除标识符的 tokens

    auto it = tks.end() - 1; // 此处一定为非空，可以直接计算
    if (*it == ";") // 删掉末尾 ;
        tks.erase(it);

    std::vector<std::string> curlyBracketTks{}, squareBracketTks{}; // 括号内容单独存储
    for (auto& tk : tks) {
        if (tk.size() < 2) { // 所有命令的参数（包括命令）都至少有两位或以上（分号除外）
            std::cout << util::ERS() + "CmdParser::in(): Argument(s) error\n";
            return false;
        }

        /* 命令检查 */
        if (allow) {
            if (fnmgr.fnFind(tk) == false) {
                std::cout << util::ERS() + "CmdParser::in(): Command '" + tk + "' not found\n";
                return false;
            }
            allow = false;
            continue; // 结束命令检查，接下来为参数
        }

        /* 参数检查，已经保证 [0][1] 可以访问 */
        std::size_t inx{1}, len{std::string::npos}; // 子串写入位置
        if (tk[0] == '-') { // -p
            if (tk.size() != 2) {
                std::cout << util::ERS() + "tksCheck(): Too many arguments in \"" + tk + "\"\n";
                return false;
            }
            inx = 1;
        }
        else if (tk[0] == ':' && tk[1] != ':') // :varn
            inx = 1;
        else if (tk[0] == ':' && tk[1] == ':') { // ::varn
            if (tk.size() < 3) {
                std::cout << util::ERS() + "tksCheck(): Argument error after identifier \"::\"\n";
                return false;
            }
            inx = 2;
        }
        else if (tk[0] == '{') { // {a b ...}
            if (tk[tk.size() - 1] != '}') {
                std::cout << util::ERS() + "tksCheck(): Missing a backbrackets '}'\n";
                return false;
            }
            inx = 1;
            len = tk.size() - 2; // 不写入最后的括号
            curlyBracketTks.push_back(tk.substr(inx, len)); // 花括号单独存储
            continue; // 不存到普通 tokens
        }
        else if (tk[0] == '[') { // [a b; c d; ...]
            if (tk[tk.size() - 1] != ']') {
                std::cout << util::ERS() + "tksCheck(): Missing a backbrackets ']'\n";
                return false;
            }
            inx = 1;
            len = tk.size() - 2; // 不写入最后的括号
            squareBracketTks.push_back(tk.substr(inx, len)); // 方括号单独存储
            continue; // 不存到普通 tokens
        }
        else {
            std::cout << util::ERS() + "tksCheck(): Unknown argument(s)\n";
            return false;
        }
        tksNoid.push_back(tk.substr(inx, len));
    }

    const std::string legalCh{"1234567890abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ"}; // 这些字符之外的字符都为非法，禁止出现在除去标识符的参数中
    const std::string legalCurlyBraCh{"1234567890. "}; // 花括号合法字符
    const std::string legalSquareBraCh{"1234567890;. "}; // 方括号合法字符
    for (const auto& tkNoid : tksNoid) {
        if (tkNoid.find_first_not_of(legalCh) != std::string::npos) { // 找到非法字符时
            std::cout << util::ERS(smr::semgr.getpath()) + "tksCheck(): Illegal argument(s) in \"" + tkNoid + "\"\n";
            return false;
        }
    }
    for (const auto& bTk : curlyBracketTks) { // 花括号内容检查
        if (bTk.empty()) {
            std::cout << util::ERS(smr::semgr.getpath()) + "tksCheck(): \"{}\" is empty\n";
            return false;
        }
        if (bTk.find_first_not_of(legalCurlyBraCh) != std::string::npos) {
            std::cout << util::ERS(smr::semgr.getpath()) + "tksCheck(): Illegal argument(s) in \"" + bTk + "\"\n";
            return false;
        }

        /* 到达此处说明所有字符均合法，若没有数字则括号参数有问题 */
        if (bTk.find_first_of("1234567890") == std::string::npos) {
            std::cout << util::ERS(smr::semgr.getpath()) + "tksCheck(): Number(s) not found in \"" + bTk + "\"\n";
            return false;
        }
    }
    for (const auto& bTk : squareBracketTks) { // 方括号内容检查
        if (bTk.empty()) {
            std::cout << util::ERS(smr::semgr.getpath()) + "tksCheck(): \"[]\" is empty\n";
            return false;
        }
        if (bTk.find_first_not_of(legalSquareBraCh) != std::string::npos) {
            std::cout << util::ERS(smr::semgr.getpath()) + "tksCheck(): Illegal argument(s) in \"" + bTk + "\"\n";
            return false;
        }

        /* 到达此处说明所有字符均合法，若没有数字则括号参数有问题 */
        if (bTk.find_first_of("1234567890") == std::string::npos) {
            std::cout << util::ERS(smr::semgr.getpath()) + "tksCheck(): Number(s) not found in \"" + bTk + "\"\n";
            return false;
        }
    }

    return true; // 都通过时
}

/* 返回单个 str 参数类型 */
Cmdt argtype(std::string str) {
    if (str[0] == '-')
        return Cmdt::OPT;
    else if (str[0] == ':' && str[1] != ':')
        return Cmdt::IN;
    else if (str[0] == ':' && str[1] == ':')
        return Cmdt::OUT;
    else if (str[0] == '[')
        return Cmdt::SB;
    else if (str[0] == '{')
        return Cmdt::CB;
    else
        return Cmdt::NOTF;
}

/* 去除首尾空格，无空格则不操作 */
void delSpace(std::string& str) {
    while (!str.empty() && *(str.end() - 1) == ' ') // 删掉末尾所有空格
        str.pop_back();
    while (!str.empty() && *str.begin() == ' ') // 删掉开头所有空格
        str.erase(str.begin());
}

/* 按照 tplate 模板的顺序与数量检查、排序参数集，二者任意一个不一致则返回 false */
bool CmdHandler::sortToken(std::vector<Cmdt> tplate) {
    if (tplate.size() != cmdToken_.size())
        return false;

    std::vector<std::string> aim{}, temp(cmdToken_);
    for (const auto i : tplate) {
        auto it = std::find_if(temp.cbegin(), temp.cend(), [i](std::string s) {if (argtype(s) == i) return true; return false; }); // 找到第一个满足当前类型 i 的迭代器
        if (it == temp.cend()) // 没找到
            return false;
        aim.push_back(*it);
        temp.erase(it); // 找到在副本中的删掉
    }
    cmdToken_ = aim;

    return true;
}

/* 解析花括号里的数值，确保传递的是花括号（没有错误检查） */
std::vector<double> CmdHandler::curlyToken(std::string token) {
    token = (token.substr(1, token.size() - 2)); // 去掉花括号的子串
    std::vector<double> nums{};
    double temp{0};
    for (std::size_t pos{0}; pos < token.size();) {
        temp = std::stod(token, &pos);
        if (pos == 0) { // 没找到数字时
            ++pos;
            continue;
        }
        nums.push_back(temp);
        if (pos < token.size()) { // 找到末尾则不拆解子串
            token = token.substr(++pos); // 此处 pos 自增前为非数字字符
            pos = 0;
        }
    }
    return nums;
}

/* 解析方括号里的矩阵，返回一个实数矩阵（无错误检查） */
core::dmtx CmdHandler::squareToken(std::string token) {
    token = (token.substr(1, token.size() - 2)); // 去掉方括号的子串

    delSpace(token); // 去除首尾空格

    /* 分解出每一行 */
    std::vector<std::string> rows{}; // 以分号为单位，一行一个分号
    for (std::size_t begPos{0}, endPos{0}; endPos < token.size();) {
        endPos = token.find(';', begPos);
        rows.push_back(token.substr(begPos, endPos - begPos)); // 从 begPos 开始，endPos 前一个结束的子串
        if (endPos != std::string::npos) { // 找到时
            if (endPos < token.size() - 1) // 不是最后一个字符
                begPos = endPos + 1;
            else if (endPos >= token.size() - 1) // 是最后一个字符，即最后一个字符是 ';'
                break;
        }
    }

    /* 解析数字 */
    std::vector<std::vector<double>> numRows{};
    for (auto row : rows) {
        delSpace(row);
        std::vector<double> temp = curlyToken("{" + row + "}");
        numRows.push_back(temp);
    }

    /* 构造矩阵 */
    std::size_t colSize = numRows.begin()->size(), rowSize = rows.size();
    core::dmtx output(rowSize, colSize); // 第一行元素个数确定列数
    for (std::size_t i{0}; i < rowSize; ++i) {
        if (numRows[i].size() != colSize) // 与第一行个数不等时
            return {};
        for (std::size_t j{0}; j < colSize; ++j)
            output(i, j) = numRows[i][j];
    }

    return output;
}

/* 检查 cmdToken_ 是否为空
 * - 空返回 true，否则 false
 * - 只有需要参数的函数能调用此检查函数（不需要参数的函调用了此检查函数后，数即使执行成功了也会输出 err_） */
bool CmdHandler::isempty(std::string outStr) {
    if (cmdToken_.empty()) {
        smr::semgr.seterr(outStr + "(): Argument(s) is empty");
        return true;
    }
    return false;
}

/* 有分号删除并返回 false，无分号返回 true */
bool CmdHandler::semicolonDel() {
    if (cmdToken_.empty()) // 空参数一定没分号
        return true;

    auto it = cmdToken_.end() - 1;
    if (*it == ";")
        it = cmdToken_.erase(it); // 删掉末尾分号
    if (it == cmdToken_.end()) // 存在分号并且被删除时
        return false;

    return true; // 无分号时
}

} // namespace laxb