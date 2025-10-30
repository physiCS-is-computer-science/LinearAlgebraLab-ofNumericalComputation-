// =====================================================================
// 所有命令都有相似的流程，调用史上最伟大的 LaxbIO、SessionManager 模块解析传递数据
// - 每个命令的流程：
//    1.去除末尾分号（记录输出标志），防止参数排序分析函数 sortToken() 返回错误消息（分号处理之后再 cmdhr.isempty("") 检查）
//    2.判断 cmdToken_ 数量，不符合则返回 false
//    3.显示指定参数顺序和种类，调用 sortToken() 函数分析排序 args
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
bool quit() {
    laxb::cmdhr.semicolonDel(); // 有分号就删掉

    if (!laxb::cmdhr.getCmdtoken().empty()) {
        smr::semgr.seterr("quit(): This command has no arguments");
        return false;
    }
    smr::semgr.swich();
    return true;
}

/* 显示变量，无论有无分号都输出
 * - show -a/l
 * - show :varn */
bool show() {
    laxb::cmdhr.semicolonDel(); // 有分号就删掉

    if (laxb::cmdhr.isempty("show"))
        return false;

    /* sortToken() */
    std::size_t type{0};
    if (laxb::cmdhr.sortToken(std::vector<laxb::Cmdt>{laxb::Cmdt::OPT}) == true)
        type = 1;
    else if (laxb::cmdhr.sortToken(std::vector<laxb::Cmdt>{laxb::Cmdt::IN}) == true)
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

        if (laxb::cmdhr.getCmdtoken()[0] == "-a") { // 所有变量
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
        else if (laxb::cmdhr.getCmdtoken()[0] == "-l") { // 所有变量名列表
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
            smr::semgr.seterr("show(): Argument(s) error in \"" + laxb::cmdhr.getCmdtoken()[0] + "\"");
            return false;
        }
    }
    else if (type == 2) { // 查看特定变量
        std::string name(laxb::cmdhr.getCmdtoken()[0].substr(1));
        auto itdmtx = smr::semgr.dmSpace_.find(name);
        auto itreal = smr::semgr.realSpace_.find(name);
        if (itdmtx == smr::semgr.dmSpace_.end() && itreal == smr::semgr.realSpace_.end()) {
            smr::semgr << "Variable \"" + name + "\" not exist";
            return true;
        }
        else {
            if (itdmtx != smr::semgr.dmSpace_.end())
                smr::semgr << "\n"
                           << itdmtx->second;
            if (itreal != smr::semgr.realSpace_.end())
                smr::semgr << "\n"
                           << itreal->second << "\n";
        }
    }

    return true;
}

/* 创建变量
 * var ::varn {a}
 * var ::varn [a b; c b] */
bool var() {
    bool shouldOut = laxb::cmdhr.semicolonDel();

    if (laxb::cmdhr.isempty("var"))
        return false;

    std::size_t type{0};
    if (laxb::cmdhr.sortToken(std::vector<laxb::Cmdt>{laxb::Cmdt::OUT, laxb::Cmdt::CB}) == true) // ::varn {a}
        type = 1;
    else if (laxb::cmdhr.sortToken(std::vector<laxb::Cmdt>{laxb::Cmdt::OUT, laxb::Cmdt::SB}) == true) // ::varn [a b; c b]
        type = 2;
    else {
        smr::semgr.seterr("var(): Argument(s) error");
        return false;
    }

    std::string varn(laxb::cmdhr.getCmdtoken()[0].substr(2)); // 去除标识符的变量/矩阵名

    /* 查找是否存在该变量 */
    auto dmtxIt = smr::semgr.finddmtx(varn);
    auto realIt = smr::semgr.findreal(varn);
    if (dmtxIt != smr::semgr.getdmtxEnd() || realIt != smr::semgr.getrealEnd()) { // 找到任何一个都不行
        smr::semgr.seterr("var(): Variable \"" + varn + "\" exist");
        return false;
    }

    if (type == 1) { // ::varn {a}
        std::vector<double> temp = laxb::cmdhr.curlyToken(laxb::cmdhr.getCmdtoken()[1]); // {a}
        if (temp.size() != 1) { // 参数太多了
            smr::semgr.seterr("var(): Too many arguments in \"" + laxb::cmdhr.getCmdtoken()[1] + "\"");
            return false;
        }

        smr::semgr.addreal(varn, temp[0]);
        if (shouldOut)
            smr::semgr << temp[0];
    }
    else if (type == 2) { // ::varn [a b; c d]
        core::dmtx mtx(laxb::cmdhr.squareToken(laxb::cmdhr.getCmdtoken()[1])); // 矩阵输入错误返回的是空矩阵
        if (mtx.isEmpty()) {
            smr::semgr.seterr("var(): Error in matrix \"" + laxb::cmdhr.getCmdtoken()[1] + "\"");
            return false;
        }

        smr::semgr.adddmtx(varn, mtx); // 添加矩阵
        if (shouldOut)
            smr::semgr << "\n"
                       << mtx;
    }

    // 此处本应为 shouldOut 检查，不过移到以上两个 if 选择之内了

    return true;
}

/* 删除变量（laxb::cmdhr 的友元）
 * del :varn */
bool del() {
    bool shouldOut = laxb::cmdhr.semicolonDel();

    if (laxb::cmdhr.isempty("del()"))
        return false;

    if (laxb::cmdhr.sortToken(std::vector<laxb::Cmdt>{laxb::Cmdt::IN}) == false) { // :varn
        smr::semgr.seterr("del(): Argument(s) error");
        return false;
    }

    std::string varn(laxb::cmdhr.getCmdtoken()[0].substr(1)); // :varn

    /* 查找变量是否存在 */
    auto dmtxIt = smr::semgr.dmSpace_.find(varn);
    auto realIt = smr::semgr.realSpace_.find(varn);
    if (dmtxIt == smr::semgr.dmSpace_.end() && realIt == smr::semgr.realSpace_.end()) {
        smr::semgr.seterr("del(): Variable \"" + varn + "\" not exist");
        return false;
    }

    /* 删除对应变量 */
    if (dmtxIt != smr::semgr.dmSpace_.end()) {
        if (shouldOut)
            smr::semgr << "\n"
                       << dmtxIt->second;
        smr::semgr.dmSpace_.erase(dmtxIt);
    }
    else {
        if (shouldOut)
            smr::semgr << realIt->second;
        smr::semgr.realSpace_.erase(realIt);
    }

    return true;
}

/* 清屏 */
bool cls() {
    laxb::cmdhr.semicolonDel();

    if (!laxb::cmdhr.getCmdtoken().empty()) {
        smr::semgr.seterr("cls(): This command has no arguments");
        return false;
    }
    system("cls");
    return true;
}

/* ==== 数学 ==== */
/* - eye {num}
 * - eye ::A {num} */
bool eye() {
    bool shouldOut = laxb::cmdhr.semicolonDel(); // 末尾分号

    if (laxb::cmdhr.isempty("eye"))
        return false;

    /* 排序 args */
    std::size_t type{0};
    if (laxb::cmdhr.sortToken(std::vector<laxb::Cmdt>{laxb::Cmdt::CB}) == true) // eye {num}
        type = 1;
    else if (laxb::cmdhr.sortToken(std::vector<laxb::Cmdt>{laxb::Cmdt::CB, laxb::Cmdt::OUT}) == true) // eye ::A {num}
        type = 2;
    else {
        smr::semgr.seterr("eye(): Argument(s) error");
        return false;
    }

    /* 分析、使用 */
    core::dmtx output{};
    std::vector<double> dimens = laxb::cmdhr.curlyToken(laxb::cmdhr.getCmdtoken()[0]);
    if (dimens.size() != 1) {
        smr::semgr.seterr("eye(): Number of arguments error in \"" + laxb::cmdhr.getCmdtoken()[0] + "\"");
        return false;
    }
    if (dimens[0] == 0) {
        smr::semgr.seterr("eye(): Invalid argument \"" + laxb::cmdhr.getCmdtoken()[0] + "\"");
        return false;
    }
    output = core::dmtx(dimens[0]); // 制造矩阵
    for (core::dmtx::mtxSizet i = 0; i < output.getRowSize(); ++i)
        output(i, i) = 1;

    if (type == 2) {
        std::string varn(laxb::cmdhr.getCmdtoken()[1].substr(2)); // ::A

        /* 查找去掉标识符的子串 */
        auto dmtxIt = smr::semgr.finddmtx(varn);
        auto realIt = smr::semgr.findreal(varn);
        if (dmtxIt != smr::semgr.getdmtxEnd() || realIt != smr::semgr.getrealEnd()) { // 只要找到一个就返回 false
            smr::semgr.seterr("eye(): Variable \"" + varn + "\" exist");
            return false;
        }

        smr::semgr.adddmtx(varn, output);
    }

    /* 判断是否写入 smr::semgr::cptoup_ */
    if (shouldOut)
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
