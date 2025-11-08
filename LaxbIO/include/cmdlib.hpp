// =================================================
// 存放所有命令语法的 "cmdname": pair<str, str> 对
// =================================================

#pragma once

#include <string>
#include <map>
#include <utility>

namespace laxb {

/* ==== 基本命令 ==== */
const std::string quitMan{"-f:\tExit the currently loaded file\n-e:\tExit the LAL program"};
const std::string showMan{"-a:\tDisplay all variables\n-l:\tDisplay a list of all variable names\n:varn:\tDisplay specific variables"};
const std::string varMan{"::varn:\tSave the output to this variable\n{}:\tparameter list\n[]:\tMatrix elements list"};
const std::string delMan{"-a:\tDelete all variables\n:varn:\tDeleting specific variables"};
const std::string saveMan{"-f:\t\tForcibly write a file\n::filename:\tSave the current session to the file"};
const std::string loadMan{"-c:\t\tBinding file\n:filename:\tVariables that load this file session"};
const std::string clsMan{"\tClear screen"};
const std::string helpMan{"-a:\tDisplay all commands\n-l:\tDisplay a list of all command names\n:cmdname:\tDisplay specific command details"};

/* ==== 运算 ==== */
//

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
};

} // namespace laxb