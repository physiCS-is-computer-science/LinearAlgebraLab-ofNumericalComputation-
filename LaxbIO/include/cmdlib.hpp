// =================================================
// 存放所有命令语法的 "cmdname": pair<str, str> 对
// =================================================

#pragma once

#include <map>
#include <string>
#include <utility>

namespace laxb {

/* ==== 基本命令 ==== */
const std::string quitMan{"-f:\tExit the currently loaded file\n"
                          "-e:\tExit the LAL program"};
const std::string showMan{"-a:\tDisplay all variables\n"
                          "-l:\tDisplay a list of all variable names\n"
                          ":varn:\tDisplay specific variables"};
const std::string varMan{"::varn:\tSave the output to this variable\n"
                         "{}:\tparameter list\n"
                         "[]:\tMatrix elements list"};
const std::string delMan{"-a:\tDelete all variables\n"
                         ":varn:\tDeleting specific variables"};
const std::string saveMan{"-f:\t\tForcibly write a file\n"
                          "::filename:\tSave the current session to the file"};
const std::string loadMan{"-c:\t\tBinding file\n"
                          ":filename:\tVariables that load this file session"};
const std::string clsMan{"\tClear screen"};
const std::string helpMan{"-a:\tDisplay all commands\n"
                          "-l:\tDisplay a list of all command names\n"
                          ":cmdname:\tDisplay specific command details"};

/* ==== 运算 ==== */
const std::string plusMan{":varn:\tAccept a variable\n"
                          "\t:varn :varn\n"
                          "::varn:\tThe result is output to this variable"};
const std::string minusMan{":varn:\tAccept a variable\n"
                           "\t:varn :varn\n"
                           "::varn:\tThe result is output to this variable"};
const std::string mtimesMan{":varn:\tAccept a variable\n"
                            "\t:varn :varn\n"
                            "::varn:\tThe result is output to this variable"};
const std::string timesMan{":varn:\tAccept a variable\n"
                           "\t:varn :varn\n"
                           "::varn:\tThe result is output to this variable"};
const std::string divideMan{":varn:\tAccept a variable\n"
                            "\t:varn :varn\n"
                            "::varn:\tThe result is output to this variable"};
const std::string powerMan{":varn:\tAccept a variable\n"
                           "\t:varn :varn\n"
                           "::varn:\tThe result is output to this variable"};

/* ==== 矩阵创建与操作 ==== */

/* ==== 矩阵分解 ==== */
const std::string luMan{"-a:\tThe output includes all the results of L, U, P\n"
                        ":varn:\tInput matrix\n"
                        "::varn:\tSave the output to this variable\n"
                        "\t::L ::U\n"
                        "t::L ::U ::P"};
const std::string rrefMan{":varn:\tInput matrix\n"
                          "::varn:\tSave the output to this variable"};

const std::map<std::string, std::pair<std::string, std::string>> manual{
    /* ==== 基本命令 ==== */
    {"quit", {"-opt", quitMan}},
    {"show", {"-opt ::varn", showMan}},
    {"var", {"::varn {a} [a b; c d]", varMan}},
    {"del", {"-opt :varn", delMan}},
    {"save", {"::filename", saveMan}},
    {"load", {":filename", loadMan}},
    {"cls", {"", clsMan}},
    {"help", {":cmdname", helpMan}},

    /* ==== 运算 ==== */
    {"plus", {":varn :varn ::varn", plusMan}},
    {"minus", {":varn :varn ::varn", minusMan}},
    {"mtimes", {":varn :varn ::varn", mtimesMan}},
    {"times", {":varn :varn ::varn", timesMan}},
    {"divide", {":varn :varn ::varn", divideMan}},
    {"power", {":varn :varn ::varn", powerMan}},

    /* ==== 矩阵创建与操作 ==== */

    /* ==== 矩阵分解 ==== */
    {"lu", {"-a :varn ::varn", luMan}},
    {"rref", {":varn ::varn", rrefMan}},
};

} // namespace laxb