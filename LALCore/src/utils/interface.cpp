// =====================================================================
// 所有命令都有相似的流程，调用史上最伟大的 LaxbIO、SessionManager 模块解析传递数据
// - 每个命令的流程：
//    1.判断 args 数量，不符合则返回 false
//    2.去除末尾分号，防止参数排序分析函数 sortArgs() 返回错误消息
//    3.显示指定参数顺序和种类，调用 sortArgs() 函数分析排序 args
//    4.逐项分析返回的有序 args，使用
//    5.根据 ';' 判断是否写入 smr::semgr::cptoup_
// =====================================================================

#include "commandIO.hpp"
#include "core/matrix.hpp"
#include "session_manager.hpp"
#include <cstdlib> // 为了 system("cls")
#include <iostream>
#include <string>
#include <vector>

/* ==== 交互 ==== */
/* 关闭 */
bool quit(std::vector<std::string> args) {
    if (!args.empty()) {
        smr::semgr.seterr("quit(): This command has no arguments");
        return false;
    }
    smr::semgr.swich();
    return true;
}

/* 显示变量
 * - show -a/l
 * - show :varn */
bool show(std::vector<std::string> args) {
    if (args.empty()) {
        smr::semgr.seterr("show(): Arguments is empty");
        return false;
    }

    /* 末尾分号 */
    auto endIt = args.end() - 1;
    if (*endIt == ";")
        args.erase(endIt);

    /* sortArgs() */
    std::size_t type{0};
    if (laxb::cmdpr.sortArgs(std::vector<laxb::Cmdt>{laxb::Cmdt::OPT}, args) == true)
        type = 1;
    else if (laxb::cmdpr.sortArgs(std::vector<laxb::Cmdt>{laxb::Cmdt::IN}, args) == true)
        type = 2;
    else {
        smr::semgr.seterr("show(): Argument(s) error");
        return false;
    }

    std::size_t cnt{1}; // 计数器
    if (type == 1) {
        if (smr::semgr.dmSpace_.empty() && smr::semgr.realSpace_.empty()) {
            smr::semgr << "NAN";
            return true;
        }

        if (args[0] == "-a") { // 所有变量
            smr::semgr << "\n\n";
            for (const auto& i : smr::semgr.dmSpace_) // 矩阵/向量
                smr::semgr << i.first << ": " << i.second;

            cnt = 0;
            for (const auto& i : smr::semgr.realSpace_) { // 实数
                if (cnt == 1)
                    smr::semgr << "\n";
                smr::semgr << i.first + ": " << i.second << "\t";
                ++cnt;
                if (cnt % 5 == 0)
                    smr::semgr << "\n";
            }
        }
        else if (args[0] == "-l") { // 所有变量名列表
            cnt = 0;
            smr::semgr << "\n\nMatrix:\n";
            for (const auto& i : smr::semgr.dmSpace_) { // 向量和矩阵名列表
                smr::semgr << i.first + "\t";
                ++cnt;
                if (cnt % 5 == 0) // 五个一行
                    smr::semgr << "\n";
            }
            if (cnt % 5 != 0) // 满一行的话已经放过 '\n' 了
                smr::semgr << "\n";

            cnt = 0;
            smr::semgr << "\nReal:\n";
            for (const auto& i : smr::semgr.realSpace_) { // 实数名列表
                smr::semgr << i.first + " ";
                ++cnt;
                if (cnt % 5 == 0) // 五个一行
                    smr::semgr << "\n";
            }
            if (cnt % 5 != 0) // 同上
                smr::semgr << "\n";
        }
        else {
            smr::semgr.seterr("show(): Argument(s) error in \"" + args[0] + "\"");
            return false;
        }
    }
    else if (type == 2) { // 查看特定变量
        std::string name{args[0].substr(1)};
        auto itdm = smr::semgr.dmSpace_.find(name);
        auto itreal = smr::semgr.realSpace_.find(name);
        if (itdm == smr::semgr.dmSpace_.end() && itreal == smr::semgr.realSpace_.end()) {
            smr::semgr << "Variable \"" + name + "\" not exist";
            return true;
        }
        else {
            if (itdm != smr::semgr.dmSpace_.end())
                smr::semgr << "\n"
                           << itdm->second;
            if (itreal != smr::semgr.realSpace_.end())
                smr::semgr << "\n"
                           << itreal->second << "\n";
        }
    }

    return true;
}

/* 创建变脸
 * var ::varn {a}
 * var ::varn [a b; c b] */
bool var(std::vector<std::string> args) {
    
}

/* 清屏 */
bool cls(std::vector<std::string> args) {
    if (!args.empty()) {
        smr::semgr.seterr("quit(): This command has no arguments");
        return false;
    }
    system("cls");
    return true;
}

/* ==== 数学 ==== */
/* - eye {num}
 * - eye ::A {num} */
bool eye(std::vector<std::string> args) {
    if (args.empty()) {
        smr::semgr.seterr("eye(): Args is empty");
        return false;
    }

    /* 末尾分号 */
    auto endIt = args.end() - 1;
    if (*endIt == ";") // 去除末尾分号防止 sortArgs() 函数排序出错
        args.erase(endIt);

    /* 排序 args */
    std::size_t type{0};
    if (laxb::cmdpr.sortArgs(std::vector<laxb::Cmdt>{laxb::Cmdt::CB}, args) == true) // eye {num}
        type = 1;
    else if (laxb::cmdpr.sortArgs(std::vector<laxb::Cmdt>{laxb::Cmdt::CB, laxb::Cmdt::OUT}, args) == true) // eye ::A {num}
        type = 2;
    else {
        smr::semgr.seterr("eye(): Argument(s) error");
        return false;
    }

    /* 分析、使用 */
    core::dmtx output{};
    std::vector<double> dimens = laxb::cmdpr.curlyArgs(args[0]);
    if (dimens.size() != 1) {
        smr::semgr.seterr("eye(): Number of arguments error in \"{" + args[0].substr(1, args[0].size() - 2) + "}\"");
        return false;
    }
    if (dimens[0] == 0) {
        smr::semgr.seterr("eye(): Invalid argument \"{0}\"");
        return false;
    }
    output = core::dmtx(dimens[0]); // 制造矩阵
    for (core::dmtx::mtxSizet i = 0; i < output.getRowSize(); ++i)
        output(i, i) = 1;

    if (type == 2) {
        std::string varn{args[1].substr(2)}; // ::A
        auto it = smr::semgr.finddmtx(varn); // 查找去掉标识符的子串
        if (it != smr::semgr.getdmtxEnd()) {
            smr::semgr.seterr("eye(): Variable \"" + varn + "\" exist");
            return false;
        }
        smr::semgr.adddmtx(varn, output);
    }

    /* 判断是否写入 smr::semgr::cptoup_ */
    if (*endIt != ";")
        smr::semgr << "\n"
                   << output;

    return true;
}

// bool zeros(std::vector<std::string> args) {
// }

// bool ones(std::vector<std::string> args) {
// }

// bool random(std::vector<std::string> args) {
// }

// bool randn(std::vector<std::string> args) {
// }

// bool diag(std::vector<std::string> args) {
// }
