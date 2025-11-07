// =================================================
// 存放所有命令语法的 "cmdname": pair<str, str> 对
// =================================================

#pragma once

#include <string>
#include <unordered_map>
#include <utility>

namespace laxb {

/* ==== 基本命令 ==== */
const std::string quitMan{"-f: Exit the currently loaded file\n-e: Exit the LAL program"};
const std::string showMan{"-a: Display all variables\n-l: Display a list of all variable names\n:varn: Display specific variables"};
const std::string varMan{"::varn: Save the output to this variable\n{}: parameter list\n[]: Matrix elements list"};
// const std::string modifyMan{""};
const std::string delMan{"-a: Delete all variables\n:varn: Deleting specific variables"};
const std::string saveMan{"-f: Forcibly write a file\n::filename: Save the current session to the file"};
const std::string loadMan{"-c: Binding file\n:filename: Variables that load this file session"};
const std::string clsMan{"Clear screen"};
const std::string helpMan{"-a: Display all commands\n-l: Display a list of all command names\n:cmdname: Display specific command details"};

/* ==== 运算 ==== */
//

const std::unordered_map<std::string, std::pair<std::string, std::string>> manual{
    /* ==== 基本命令 ==== */
    {"quit", {"-opt", quitMan}},
    {"show", {"-opt ::varn", showMan}},
    {"var", {"::varn {a} [a b; c d]", varMan}},
    // {"modify", {"", modifyMan}},
    {"del", {"-opt :varn", delMan}},
    {"save", {"::filename", saveMan}},
    {"load", {":filename", loadMan}},
    {"cls", {"", clsMan}},
    {"help", {":cmdname", helpMan}},

    /* ==== 运算 ==== */
};

} // namespace laxb