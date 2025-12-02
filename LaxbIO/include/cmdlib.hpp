// =================================================
// 存放所有命令语法的 "cmdname": pair<str, str> 对
// =================================================

#pragma once

#include <map>
#include <string>
#include <utility>

namespace laxb {

/* ==== 基本命令 ==== */
const std::string quitMan{
    "-f:    Exit the currently loaded file\n"
    "-e:    Exit the LAL program"
};
const std::string showMan{
    "-a:       Display all variables\n"
    "-l:       Display a list of all variable names\n"
    ":varn:    Display specific variables"
};
const std::string varMan{
    "::varn:    Save the output to this variable\n"
    "{}:        parameter list\n"
    "[]:        Matrix elements list"
};
const std::string delMan{
    "-a:       Delete all variables\n"
    ":varn:    Deleting specific variables"
};
const std::string saveMan{
    "-f:            Forcibly write a file\n"
    "::filename:    Save the current session to the file"
};
const std::string loadMan{
    "-c:           Binding file\n"
    ":filename:    Variables that load this file session"
};
const std::string clsMan{
    "    Clear screen"
};
const std::string helpMan{
    "-a:          Display all commands\n"
    "-l:          Display a list of all command names\n"
    ":cmdname:    Display specific command details"
};

/* ==== 运算 ==== */
const std::string plusMan{
    ":varn:     Accept a variable\n"
    "           :varn :varn\n"
    "::varn:    The result is output to this variable"
};
const std::string minusMan{
    ":varn:     Accept a variable\n"
    "           :varn :varn\n"
    "::varn:    The result is output to this variable"
};
const std::string mtimesMan{
    ":varn:     Accept a variable\n"
    "           :varn :varn\n"
    "::varn:    The result is output to this variable"
};
const std::string timesMan{
    ":varn:     Accept a variable\n"
    "           :varn :varn\n"
    "::varn:    The result is output to this variable"
};
const std::string divideMan{
    ":varn:     Accept a variable\n"
    "           :varn :varn\n"
    "::varn:    The result is output to this variable"
};
const std::string powerMan{
    ":varn:     Accept a variable\n"
    "           :varn :varn\n"
    "::varn:    The result is output to this variable"
};

/* ==== 矩阵创建与操作 ==== */

/* ==== 矩阵分解 ==== */
const std::string luMan{
    "-a:        The output includes all the results of L, U, P\n"
    ":varn:     Input matrix\n"
    "::varn:    Save the output to this variable\n"
    "           ::L ::U\n"
    "           ::L ::U ::P"
};
const std::string rrefMan{
    ":varn:     Input matrix\n"
    "::varn:    Save the output to this variable"
};

const std::map<std::string, std::pair<std::string, std::string>> manual{
    /* ==== 基本命令 ==== */
    {"quit", {"-opt", quitMan}},
    {"show", {"-opt ::varn", showMan}},
    {"var",  {"::varn {a} [a b; c d]", varMan}},
    {"del",  {"-opt :varn", delMan}},
    {"save", {"::filename", saveMan}},
    {"load", {":filename", loadMan}},
    {"cls",  {"", clsMan}},
    {"help", {":cmdname", helpMan}},

    /* ==== 运算 ==== */
    {"plus",   {":varn :varn ::varn", plusMan}},
    {"minus",  {":varn :varn ::varn", minusMan}},
    {"mtimes", {":varn :varn ::varn", mtimesMan}},
    {"times",  {":varn :varn ::varn", timesMan}},
    {"divide", {":varn :varn ::varn", divideMan}},
    {"power",  {":varn :varn ::varn", powerMan}},

    /* ==== 矩阵创建与操作 ==== */

    /* ==== 矩阵分解 ==== */
    {"lu",   {"-a :varn ::varn", luMan}},
    {"rref", {":varn ::varn", rrefMan}},
};

} // namespace laxb