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
    if (cmdLine.empty()) { // 确保不为空
        return false;
    }

    /* 简单检查正确性，并转换为 tokens */
    std::vector<std::string> tokens;
    std::size_t endPos{0}, begPos{0}; // 结束位置下标

    delCh(cmdLine, ' '); // 去除首尾空格
    if (cmdLine.empty()) { // 什么都不做，等待重新输入
        return false;
    }

    /* 最后一个字符为 ';' 时，在它之前插入一个空格，以分出 token */
    auto tmpIt = cmdLine.end();
    if (*(--tmpIt) == ';') {
        cmdLine.insert(tmpIt, ' ');
    }
    std::size_t cmdlSize = cmdLine.size();
    while (true) {
        /* 普通字符起点 */
        if (cmdLine[begPos] != '{' && cmdLine[begPos] != '[') {
            endPos = cmdLine.find(" ", begPos);
        }

        /* 括号起点 */
        if (cmdLine[begPos] == '{' || cmdLine[begPos] == '[') {
            if (cmdLine[begPos] == '{') {
                endPos = cmdLine.find("}", begPos);
            }
            else if (cmdLine[begPos] == '[') {
                endPos = cmdLine.find("]", begPos);
            }

            if (endPos != std::string::npos) { // 找到反括号时
                if (endPos == cmdlSize - 1) { // 最后一个字符为括号时
                    break;
                }
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

        if (endPos == std::string::npos) { // 循环结束标志，处理找不到空格的情况
            break;
        }

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
    if (tksCheck(tokens) == false) {
        return false;
    }

    laxb::cmdhr.setcmdtk(tokens);
    return true;
}

/* 检查 tokens，返回布尔值 */
bool tksCheck(std::vector<std::string> tks) {
    bool allow{true}; // 一次性，此处只检查第一个命令
    std::vector<std::string> tksNoid; // 去除标识符的 tokens

    auto it = tks.end() - 1; // 此处一定为非空，可以直接计算
    if (*it == ";") { // 删掉末尾 ;
        tks.erase(it);
    }

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
        else if (tk[0] == ':' && tk[1] != ':') { // :varn
            inx = 1;
        }
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

    const std::string legalCh{"-/1234567890abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ"}; // 这些字符之外的字符都为非法，禁止出现在除去标识符的参数中
    const std::string legalCurlyBraCh{"-/1234567890. "}; // 花括号合法字符
    const std::string legalSquareBraCh{"-/1234567890;. "}; // 方括号合法字符
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
    if (str[0] == '-') {
        return Cmdt::OPT;
    }
    else if (str[0] == ':' && str[1] != ':') {
        return Cmdt::INID;
    }
    else if (str[0] == ':' && str[1] == ':') {
        return Cmdt::OUTID;
    }
    else if (str[0] == '[') {
        return Cmdt::SB;
    }
    else if (str[0] == '{') {
        return Cmdt::CB;
    }
    else {
        return Cmdt::NOTF;
    }
}

/* 去除首尾 tobedel 字符，无此字符则不操作 */
void delCh(std::string& str, char tobedel) {
    while (!str.empty() && *(str.end() - 1) == tobedel) { // 删掉末尾所有空格
        str.pop_back();
    }
    while (!str.empty() && *str.begin() == tobedel) { // 删掉开头所有空格
        str.erase(str.begin());
    }
}

/* 以分隔符 sep 分割字符串为 tokens
 * - sep 为字符，默认值为 ' '
 * - str 中无 sep 时则返回 str 单独构成的 vector（存于 vector 的原串）
 * - 此函数会主动删除首尾 sep 字符，防止出现未定义行为
 * - str 为空串时不执行循环直接返回空 vector */
std::vector<std::string> splitBych(std::string& str, char sep) {
    delCh(str, sep); // 删除首尾 sep 字符，防止以下循环出现 substr() 访问越界的未定义行为

    std::vector<std::string> tokens{};
    for (std::size_t posbeg = 0, posend = 0; posend < str.size(); ++posbeg) { // str 为空串时不执行循环直接返回空 vector
        posend = str.find(sep, posbeg);
        if (posbeg == posend) { // posbeg 自身为 sep
            continue;
        }

        tokens.push_back(str.substr(posbeg, posend - posbeg)); // 字串范围: [posbeg, posend - 1]（未找到时将 posbeg 之后字符全部写入）
        posbeg = posend;
    }

    return tokens;
}

/* 按照 tplate 模板的顺序与数量检查、排序参数集，二者任意一个不一致则返回 false */
bool CmdHandler::sortToken(std::vector<Cmdt> tplate) {
    if (tplate.size() != cmdToken_.size()) {
        return false;
    }

    std::vector<std::string> aim{}, temp(cmdToken_);
    for (const auto i : tplate) {
        auto it = std::find_if(temp.cbegin(), temp.cend(), [i](std::string s) {if (argtype(s) == i) return true; return false; }); // 找到第一个满足当前类型 i 的迭代器
        if (it == temp.cend()) { // 没找到
            return false;
        }
        aim.push_back(*it);
        temp.erase(it); // 找到在副本中的删掉
    }
    cmdToken_ = aim;

    return true;
}

/* 解析花括号里的数值，存储进 vector 中
 * - 调用此函数的所有地方，都必须保证传递的带花括号字符串内部为合法字符: "1234567890-/."
 * - 必须保证传递的字符串为 "  {...}  " 形式，否则解析错误
 * - 错误时传递空 vector */
std::vector<double> CmdHandler::curlyToken(std::string token) {
    delCh(token, ' '); // 去除首尾空格，暴露花括号
    delCh(token, '{'); // 去掉花括号的子串 { ... }
    delCh(token, '}');
    delCh(token, ' '); // 去除首尾空格

    if (token[0] == '/' || token[token.size() - 1] == '/' || token[token.size() - 1] == '/') { // "/a.." "..a/" "..a-"
        return {};
    }

    std::vector<std::string> originTks = splitBych(token); // 分解为 tokens，得到 "a" "a/" "c"..（splitBych() 默认以空格为 sep）

    /* 更精细的 tokens，得到 "a" "a" "/" "b" "c"... */
    enum class State { // 状态机
        NORM,
        DIV
    } state = State::NORM;
    std::vector<std::string> tks{};
    for (const auto& i : originTks) {
        std::size_t div = i.find("/");

        if (div != std::string::npos && state == State::NORM) { // "/" "a/" "/a" "a/b"
            if (std::count(i.begin(), i.end(), '/') != 1) { // 一个分式不可能两个 '/'
                return {};
            }

            if (i.size() == 1) { // "/"
                tks.push_back("/");
                state = State::DIV; // 分式状态
            }
            else if (i[i.size() - 1] == '/') { // "a/"
                tks.push_back(i.substr(0, div - 0));
                tks.push_back("/");
                state = State::DIV; // 分式状态
            }
            else if (i[0] == '/') { // "/a"
                tks.push_back("/");
                tks.push_back(i.substr(1));
            }
            else if (i.size() > 2 && i[0] != '/' && i[i.size() - 1] != '/') { // "a/b"
                tks.push_back(i.substr(0, div));
                tks.push_back("/");
                tks.push_back(i.substr(div + 1));
            }
            else {
                return {};
            }
        }
        else if (div != std::string::npos && state == State::DIV) { // 分式状态下又找到 '/' 一定为错误
            return {};
        }
        else {
            tks.push_back(i);
            if (state == State::DIV) { // 恢复普通状态
                state = State::NORM;
            }
        }
    }

    /* 正确分数格式匹配 */
    std::vector<double> nums{};
    for (std::size_t i = 0; i < tks.size(); ++i) {
        if (i < tks.size() - 1 && tks[i + 1] != "/") { // "a" "b"...
            nums.push_back(std::stod(tks[i]));
        }
        else if (i < tks.size() - 1 && tks[i + 1] == "/") { // "a" "/"...
            double a = std::stod(tks[i]), b = std::stod(tks[i + 2]);
            if (b == 0) { // "a" "/" "0"
                return {};
            }
            nums.push_back(a / b);
            i = i + 2; // 此时 i 在 a/b 的 b，循环头处自增
        }
        else if (i == tks.size() - 1) { // "a" end
            nums.push_back(std::stod(tks[i]));
        }
        else {
            return {};
        }
    }

    return nums;
}

/* 解析方括号里的矩阵，返回一个实数矩阵
 * - 调用此函数的所有地方，都必须保证传递的带方括号字符串内部为合法字符: "1234567890-/.;"
 * - 必须保证传递的字符串为 "  [...]  " 形式，否则解析错误
 * - 错误时传递空矩阵 */
core::dmtx CmdHandler::squareToken(std::string token) {
    delCh(token, ' ');
    delCh(token, '[');
    delCh(token, ']');

    /* 清除开头末尾的 ' ' 和 ';' 混合干扰字符串 */
    while (!token.empty() && (token[token.size() - 1] == ';' || token[token.size() - 1] == ' ')) { // 末尾的 ' ' 和 ';' 清除干净
        token.pop_back();
    }
    while (!token.empty() && (token[0] == ';' || token[0] == ' ')) { // 开头的 ' ' 和 ';' 清除干净
        token.erase(token.begin());
    }

    std::vector<std::string> rows = splitBych(token, ';'); // 以分号为 sep，一行一个分号，分解出每一行，结果不带 ';'

    /* 调用 curlyToken() 返回行，从而构造矩阵 */
    std::vector<std::vector<double>> numRows{};
    for (auto row : rows) {
        std::vector<double> temp = curlyToken("{" + row + "}");
        if (temp.empty()) {
            return {};
        }
        numRows.push_back(temp);
    }

    /* 构造矩阵 */
    core::dmtx::mtxSizet rowSize = numRows.size(), colSize = numRows[0].size();
    core::dmtx output{rowSize, colSize};
    for (core::dmtx::mtxSizet i = 0; i < rowSize; ++i) { // 以第一行的列数确定矩阵列数
        if (numRows[i].size() != colSize) { // 与第一行的元素个数不一致时
            return {};
        }
        for (core::dmtx::mtxSizet j = 0; j < colSize; ++j) {
            output(i, j) = numRows[i][j];
        }
    }

    return output;
}

/* 检查 cmdToken_ 是否为空
 * - 空返回 true，否则 false
 * - 只有需要参数的函数能调用此检查函数（不需要参数的函调用了此检查函数后，数即使执行成功了也会输出 err_） */
bool CmdHandler::isempty(std::string outStr) {
    if (cmdToken_.empty()) {
        smr::semgr.seterr(outStr + ": Argument(s) is empty");
        return true;
    }
    return false;
}

/* 有分号删除并返回 false，无分号返回 true */
void CmdHandler::semicolonDel() {
    if (cmdToken_.empty()) { // 空参数一定没分号
        outbit_ = true;
        return;
    }

    auto it = cmdToken_.end() - 1;
    if (*it == ";") {
        it = cmdToken_.erase(it); // 删掉末尾分号，刷新 it 为 .end()
    }
    if (it == cmdToken_.end()) { // 存在分号并且被删除时
        outbit_ = false;
        return;
    }

    outbit_ = true; // 无分号时
}

// /* ==== 一组重载函数，根据 outbit_ 判断是否写入参数至 semgr ==== */
// void CmdHandler::outDetermine(double output) {

// }

// void CmdHandler::outDetermine(std::string output) {

// }

// void CmdHandler::outDetermine(char output) {

// }

// void CmdHandler::outDetermine(core::dmtx output) {

// }

// void CmdHandler::outDetermine(core::dvec output) {

// }

} // namespace laxb