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
#include "decompositions/basic_decomp.hpp"
#include "session_manager.hpp"
#include <cstdlib> // 为了 system("cls")
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

/* ==== 基本命令 ==== */
/* 关闭 */
bool quit() {
    laxb::cmdhr.semicolonDel(); // 有分号就删掉

    if (!laxb::cmdhr.getCmdtoken().empty()) {
        smr::semgr.seterr(laxb::cmdhr.getName() + ": This command has no arguments");
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

    if (laxb::cmdhr.isempty(laxb::cmdhr.getName())) {
        return false;
    }

    /* sortToken() */
    std::size_t type{0};
    if (laxb::cmdhr.sortToken(std::vector<laxb::Cmdt>{laxb::Cmdt::OPT}) == true) {
        type = 1;
    }
    else if (laxb::cmdhr.sortToken(std::vector<laxb::Cmdt>{laxb::Cmdt::IN}) == true) {
        type = 2;
    }
    else {
        smr::semgr.seterr(laxb::cmdhr.getName() + ": Argument(s) error");
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
            for (const auto& i : smr::semgr.dmSpace_) { // 矩阵/向量
                smr::semgr << i.first << ": " << i.second;
            }
            cnt = 0;
            smr::semgr << "\n";
            for (const auto& i : smr::semgr.realSpace_) { // 实数
                smr::semgr << i.first + ": " << i.second << "\t";
                ++cnt;
                if (cnt % 5 == 0) {
                    smr::semgr << "\n";
                }
            }
            if (cnt % 5 != 0) { // 满一行的话已经放过 '\n' 了
                smr::semgr << "\n";
            }
        }
        else if (laxb::cmdhr.getCmdtoken()[0] == "-l") { // 所有变量名列表
            cnt = 0;
            smr::semgr << "\n\nMatrix:\n";
            for (const auto& i : smr::semgr.dmSpace_) { // 向量和矩阵名列表
                smr::semgr << i.first + "\t";
                ++cnt;
                if (cnt % 5 == 0) { // 五个一行
                    smr::semgr << "\n";
                }
            }
            if (cnt % 5 != 0) { // 满一行的话已经放过 '\n' 了
                smr::semgr << "\n";
            }
            cnt = 0;
            smr::semgr << "\nReal:\n";
            for (const auto& i : smr::semgr.realSpace_) { // 实数名列表
                smr::semgr << i.first + "\t";
                ++cnt;
                if (cnt % 5 == 0) { // 五个一行
                    smr::semgr << "\n";
                }
            }
            if (cnt % 5 != 0) { // 同上
                smr::semgr << "\n";
            }
        }
        else {
            smr::semgr.seterr(laxb::cmdhr.getName() + ": Argument(s) error in \"" + laxb::cmdhr.getCmdtoken()[0] + "\"");
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
            if (itdmtx != smr::semgr.dmSpace_.end()) {
                smr::semgr << "\n"
                           << itdmtx->second;
            }
            if (itreal != smr::semgr.realSpace_.end()) {
                smr::semgr << "\n"
                           << itreal->second << "\n";
            }
        }
    }

    return true;
}

/* 创建变量，此函数会检查所有变量名，如果有重名则不允许创建新变量（除此之外的所有创建、更改变量操作都会覆盖原变量）
 * var ::varn {a}
 * var ::varn [a b; c b] */
bool var() {
    bool shouldOut = laxb::cmdhr.semicolonDel();

    if (laxb::cmdhr.isempty(laxb::cmdhr.getName())) {
        return false;
    }

    std::size_t type{0};
    if (laxb::cmdhr.sortToken(std::vector<laxb::Cmdt>{laxb::Cmdt::OUT, laxb::Cmdt::CB}) == true) { // ::varn {a}
        type = 1;
    }
    else if (laxb::cmdhr.sortToken(std::vector<laxb::Cmdt>{laxb::Cmdt::OUT, laxb::Cmdt::SB}) == true) { // ::varn [a b; c b]
        type = 2;
    }
    else {
        smr::semgr.seterr(laxb::cmdhr.getName() + ": Argument(s) error");
        return false;
    }

    std::string varn(laxb::cmdhr.getCmdtoken()[0].substr(2)); // 去除标识符的变量/矩阵名

    /* 查找是否存在该变量 */
    auto dmtxIt = smr::semgr.finddmtx(varn);
    auto realIt = smr::semgr.findreal(varn);
    if (dmtxIt != smr::semgr.getdmtxEnd() || realIt != smr::semgr.getrealEnd()) { // 找到任何一个都不行
        smr::semgr.seterr(laxb::cmdhr.getName() + ": Variable \"" + varn + "\" exist");
        return false;
    }

    if (type == 1) { // ::varn {a}
        std::vector<double> temp = laxb::cmdhr.curlyToken(laxb::cmdhr.getCmdtoken()[1]); // {a}
        if (temp.size() != 1) { // 参数太多了
            smr::semgr.seterr(laxb::cmdhr.getName() + ": Argument(s) error in \"" + laxb::cmdhr.getCmdtoken()[1] + "\"");
            return false;
        }

        smr::semgr.addreal(varn, temp[0]); // 上方检查过，矩阵与实数都没有同名变量，因而可以直接添加
        if (shouldOut) {
            smr::semgr << temp[0];
        }
    }
    else if (type == 2) { // ::varn [a b; c d]
        core::dmtx mtx(laxb::cmdhr.squareToken(laxb::cmdhr.getCmdtoken()[1])); // 矩阵输入错误返回的是空矩阵
        if (mtx.isEmpty()) {
            smr::semgr.seterr(laxb::cmdhr.getName() + ": Error in matrix \"" + laxb::cmdhr.getCmdtoken()[1] + "\"");
            return false;
        }

        smr::semgr.adddmtx(varn, mtx); // 上方检查过，矩阵与实数都没有同名变量，因而可以直接添加
        if (shouldOut) {
            smr::semgr << "\n"
                       << mtx;
        }
    }

    // 此处本应为 shouldOut 检查，不过移到以上两个 if 选择之内了

    return true;
}

/* 删除变量（laxb::cmdhr 的友元）
 * del :varn */
bool del() {
    bool shouldOut = laxb::cmdhr.semicolonDel();

    if (laxb::cmdhr.isempty(laxb::cmdhr.getName())) {
        return false;
    }

    if (laxb::cmdhr.sortToken(std::vector<laxb::Cmdt>{laxb::Cmdt::IN}) == false) { // :varn
        smr::semgr.seterr(laxb::cmdhr.getName() + ": Argument(s) error");
        return false;
    }

    std::string varn(laxb::cmdhr.getCmdtoken()[0].substr(1)); // :varn

    /* 查找变量是否存在 */
    auto dmtxIt = smr::semgr.dmSpace_.find(varn);
    auto realIt = smr::semgr.realSpace_.find(varn);
    if (dmtxIt == smr::semgr.dmSpace_.end() && realIt == smr::semgr.realSpace_.end()) {
        smr::semgr.seterr(laxb::cmdhr.getName() + ": Variable \"" + varn + "\" not exist");
        return false;
    }

    /* 删除对应变量 */
    if (dmtxIt != smr::semgr.dmSpace_.end()) {
        if (shouldOut) {
            smr::semgr << "\n"
                       << dmtxIt->second;
        }
        smr::semgr.dmSpace_.erase(dmtxIt);
    }
    else {
        if (shouldOut) {
            smr::semgr << realIt->second;
        }
        smr::semgr.realSpace_.erase(realIt);
    }

    return true;
}

/* load :filename */
// bool load() {
//     bool shouldOut = laxb::cmdhr.semicolonDel();
//     if (laxb::cmdhr.isempty(laxb::cmdhr.getName())) {
//         return false;
//     }
//     std::size_t type = 0;
//     if (laxb::cmdhr.sortToken(std::vector<laxb::Cmdt>{laxb::Cmdt::IN}) == true) {
//         type = 1;
//     }
//     else {
//         smr::semgr.seterr(laxb::cmdhr.getName() + ": Argument(s) error");
//         return false;
//     }

//     smr::semgr.rfile(smr::SEPATH + laxb::cmdhr.getCmdtoken()[0]);
// }

/* 清屏 */
bool cls() {
    laxb::cmdhr.semicolonDel();

    if (!laxb::cmdhr.getCmdtoken().empty()) {
        smr::semgr.seterr(laxb::cmdhr.getName() + ": This command has no arguments");
        return false;
    }
    system("cls");
    return true;
}

/* ==== 运算 ==== */
/* - plus :varn :varn ::varn
 * - plus :varn :varn */
bool plus() {
    bool shouldOut = laxb::cmdhr.semicolonDel();
    if (laxb::cmdhr.isempty(laxb::cmdhr.getName())) {
        return false;
    }
    std::size_t type{0};
    if (laxb::cmdhr.sortToken(std::vector<laxb::Cmdt>{laxb::Cmdt::IN, laxb::Cmdt::IN, laxb::Cmdt::OUT}) == true) { // :a :b ::c
        type = 1;
    }
    else if (laxb::cmdhr.sortToken(std::vector<laxb::Cmdt>{laxb::Cmdt::IN, laxb::Cmdt::IN}) == true) { // :a :b
        type = 2;
    }
    else {
        smr::semgr.seterr(laxb::cmdhr.getName() + ": Argument(s) error");
        return false;
    }

    std::string varnIn1(laxb::cmdhr.getCmdtoken()[0].substr(1)), varnIn2(laxb::cmdhr.getCmdtoken()[1].substr(1)); // :varn :varn
    std::vector<decltype(smr::semgr.getdmtxEnd())> dmtxIts{};
    std::vector<decltype(smr::semgr.getrealEnd())> realIts{};
    std::vector<std::string> varns{varnIn1, varnIn2}; // 这样子好算

    for (const auto& i : varns) { // 矩阵查找
        dmtxIts.push_back(smr::semgr.finddmtx(i));
    }
    for (const auto& i : varns) { // 实数查找
        realIts.push_back(smr::semgr.findreal(i));
    }

    /* 变量存在性检查 */
    if (dmtxIts[0] == smr::semgr.getdmtxEnd() && realIts[0] == smr::semgr.getrealEnd()) {
        smr::semgr.seterr(laxb::cmdhr.getName() + ": Variable \"" + varnIn1 + "\" is not exist");
        return false;
    }
    if (dmtxIts[1] == smr::semgr.getdmtxEnd() && realIts[1] == smr::semgr.getrealEnd()) {
        smr::semgr.seterr(laxb::cmdhr.getName() + ": Variable \"" + varnIn2 + "\" is not exist");
        return false;
    }

    /* 类型一致性检查 */
    if (dmtxIts[0] != smr::semgr.getdmtxEnd() && dmtxIts[1] != smr::semgr.getdmtxEnd()) {
        try { // 防止矩阵维度不匹配
            dmtxIts[0]->second + dmtxIts[1]->second;
        } catch (std::invalid_argument& e) {
            smr::semgr.seterr(laxb::cmdhr.getName() + ": " + e.what());
            return false;
        }

        core::dmtx output = dmtxIts[0]->second + dmtxIts[1]->second; // 再执行一次

        if (type == 1) { // plus :varn :varn ::varn
            smr::semgr.adddmtx(laxb::cmdhr.getCmdtoken()[2].substr(2), output); // ::varn
        }
        if (shouldOut) {
            smr::semgr << "\n"
                       << output;
        }
    }
    else if (realIts[0] != smr::semgr.getrealEnd() && realIts[1] != smr::semgr.getrealEnd()) {
        double output = realIts[0]->second + realIts[1]->second;

        if (type == 1) { // plus :varn :varn ::varn
            smr::semgr.addreal(laxb::cmdhr.getCmdtoken()[2].substr(2), output); // ::varn
        }
        if (shouldOut) {
            smr::semgr << output;
        }
    }
    else {
        smr::semgr.seterr(laxb::cmdhr.getName() + ": Variables \"" + varnIn1 + "\" and \"" + varnIn2 + "\" are of different types");
        return false;
    }

    return true;
}

/* 被减数取决于输入（:）变量的左右，和输出（::）变量位置无关
 * - minus :varn :varn ::varn
 * - minus :varn :varn */
bool minus() {
    bool shouldOut = laxb::cmdhr.semicolonDel();

    if (laxb::cmdhr.isempty(laxb::cmdhr.getName())) {
        return false;
    }

    std::size_t type{0};
    if (laxb::cmdhr.sortToken(std::vector<laxb::Cmdt>{laxb::Cmdt::IN, laxb::Cmdt::IN, laxb::Cmdt::OUT}) == true) {
        type = 1;
    }
    else if (laxb::cmdhr.sortToken(std::vector<laxb::Cmdt>{laxb::Cmdt::IN, laxb::Cmdt::IN}) == true) {
        type = 2;
    }
    else {
        smr::semgr.seterr(laxb::cmdhr.getName() + ": Argument(s) error");
        return false;
    }

    std::string varnIn1(laxb::cmdhr.getCmdtoken()[0].substr(1)), varnIn2(laxb::cmdhr.getCmdtoken()[1].substr(1)); // :varn :varn
    std::vector<decltype(smr::semgr.getdmtxEnd())> dmtxIts{};
    std::vector<decltype(smr::semgr.getrealEnd())> realIts{};
    std::vector<std::string> varns{varnIn1, varnIn2}; // 这样子好算

    for (const auto& i : varns) { // 矩阵查找
        dmtxIts.push_back(smr::semgr.finddmtx(i));
    }
    for (const auto& i : varns) { // 实数查找
        realIts.push_back(smr::semgr.findreal(i));
    }

    /* 变量存在性检查 */
    if (dmtxIts[0] == smr::semgr.getdmtxEnd() && realIts[0] == smr::semgr.getrealEnd()) {
        smr::semgr.seterr(laxb::cmdhr.getName() + ": Variable \"" + varnIn1 + "\" is not exist");
        return false;
    }
    if (dmtxIts[1] == smr::semgr.getdmtxEnd() && realIts[1] == smr::semgr.getrealEnd()) {
        smr::semgr.seterr(laxb::cmdhr.getName() + ": Variable \"" + varnIn2 + "\" is not exist");
        return false;
    }

    /* 类型一致性检查 */
    if (dmtxIts[0] != smr::semgr.getdmtxEnd() && dmtxIts[1] != smr::semgr.getdmtxEnd()) {
        try { // 防止矩阵维度不匹配
            dmtxIts[0]->second - dmtxIts[1]->second;
        } catch (std::invalid_argument& e) {
            smr::semgr.seterr(laxb::cmdhr.getName() + ": " + e.what());
            return false;
        }

        core::dmtx output = dmtxIts[0]->second - dmtxIts[1]->second; // 再执行一次

        if (type == 1) { // plus :varn :varn ::varn
            smr::semgr.adddmtx(laxb::cmdhr.getCmdtoken()[2].substr(2), output); // ::varn
        }
        if (shouldOut) {
            smr::semgr << "\n"
                       << output;
        }
    }
    else if (realIts[0] != smr::semgr.getrealEnd() && realIts[1] != smr::semgr.getrealEnd()) {
        double output = realIts[0]->second - realIts[1]->second;

        if (type == 1) { // plus :varn :varn ::varn
            smr::semgr.addreal(laxb::cmdhr.getCmdtoken()[2].substr(2), output); // ::varn
        }
        if (shouldOut) {
            smr::semgr << output;
        }
    }
    else {
        smr::semgr.seterr(laxb::cmdhr.getName() + ": Variables \"" + varnIn1 + "\" and \"" + varnIn2 + "\" are of different types");
        return false;
    }

    return true;
}

/* 矩阵乘（times 为逐元素）
 * mtimes :varn :varn ::varn
 * mtimes :varn :varn */
bool mtimes() {
    bool shouldOut = laxb::cmdhr.semicolonDel();

    if (laxb::cmdhr.isempty(laxb::cmdhr.getName())) {
        return false;
    }

    std::size_t type{0};
    if (laxb::cmdhr.sortToken(std::vector<laxb::Cmdt>{laxb::Cmdt::IN, laxb::Cmdt::IN, laxb::Cmdt::OUT}) == true) { // :A :B ::C
        type = 1;
    }
    else if (laxb::cmdhr.sortToken(std::vector<laxb::Cmdt>{laxb::Cmdt::IN, laxb::Cmdt::IN}) == true) { // :A :B
        type = 2;
    }
    else {
        smr::semgr.seterr(laxb::cmdhr.getName() + ": Argument(s) error");
        return false;
    }

    std::string varnIn1(laxb::cmdhr.getCmdtoken()[0].substr(1)), varnIn2(laxb::cmdhr.getCmdtoken()[1].substr(1)); // :varn :varn
    std::vector<decltype(smr::semgr.getdmtxEnd())> dmtxIts{};
    std::vector<decltype(smr::semgr.getrealEnd())> realIts{};
    std::vector<std::string> varns{varnIn1, varnIn2}; // 这样子好算

    for (const auto& i : varns) { // 矩阵查找
        dmtxIts.push_back(smr::semgr.finddmtx(i));
    }
    for (const auto& i : varns) { // 实数查找
        realIts.push_back(smr::semgr.findreal(i));
    }

    /* 变量存在性检查 */
    if (dmtxIts[0] == smr::semgr.getdmtxEnd() && realIts[0] == smr::semgr.getrealEnd()) {
        smr::semgr.seterr(laxb::cmdhr.getName() + ": Variable \"" + varnIn1 + "\" is not exist");
        return false;
    }
    if (dmtxIts[1] == smr::semgr.getdmtxEnd() && realIts[1] == smr::semgr.getrealEnd()) {
        smr::semgr.seterr(laxb::cmdhr.getName() + ": Variable \"" + varnIn2 + "\" is not exist");
        return false;
    }

    core::dmtx output{};

    /* 计算 */
    if (dmtxIts[0] != smr::semgr.getdmtxEnd() && dmtxIts[1] != smr::semgr.getdmtxEnd()) { // matrix * matrix
        if (dmtxIts[0]->second.getColSize() != dmtxIts[1]->second.getRowSize()) {
            smr::semgr.seterr(laxb::cmdhr.getName() + ": Dimension dismatch between matrix \"" + varnIn1 + "\" and \"" + varnIn2 + "\"");
            return false;
        }
        output = dmtxIts[0]->second * dmtxIts[1]->second;
    }
    else if (dmtxIts[0] != smr::semgr.getdmtxEnd() && realIts[1] != smr::semgr.getrealEnd()) {
        output = dmtxIts[0]->second * realIts[1]->second;
    }
    else if (realIts[0] != smr::semgr.getrealEnd() && dmtxIts[1] != smr::semgr.getdmtxEnd()) {
        output = realIts[0]->second * dmtxIts[1]->second;
    }

    if (type == 1) { // mtimes :varn :varn ::varn
        smr::semgr.adddmtx(laxb::cmdhr.getCmdtoken()[2].substr(2), output); // ::varn
    }
    if (shouldOut) {
        smr::semgr << "\n"
                   << output;
    }

    return true;
}

/* 逐元素乘
 * - times :varn :varn ::varn
 * - times :varn :varn */
bool times() {
    bool shouldOut = laxb::cmdhr.semicolonDel();
    if (laxb::cmdhr.isempty(laxb::cmdhr.getName())) {
        return false;
    }

    std::size_t type{0};
    if (laxb::cmdhr.sortToken(std::vector<laxb::Cmdt>{laxb::Cmdt::IN, laxb::Cmdt::IN, laxb::Cmdt::OUT}) == true) { // :A :B ::C
        type = 1;
    }
    else if (laxb::cmdhr.sortToken(std::vector<laxb::Cmdt>{laxb::Cmdt::IN, laxb::Cmdt::IN}) == true) { // :A :B
        type = 2;
    }
    else {
        smr::semgr.seterr(laxb::cmdhr.getName() + ": Argument(s) error");
        return false;
    }

    std::string varnIn1(laxb::cmdhr.getCmdtoken()[0].substr(1)), varnIn2(laxb::cmdhr.getCmdtoken()[1].substr(1)); // :varn :varn
    std::vector<decltype(smr::semgr.getdmtxEnd())> dmtxIts{};
    std::vector<decltype(smr::semgr.getrealEnd())> realIts{};
    std::vector<std::string> varns{varnIn1, varnIn2}; // 这样子好算

    for (const auto& i : varns) { // 矩阵查找
        dmtxIts.push_back(smr::semgr.finddmtx(i));
    }
    for (const auto& i : varns) { // 实数查找
        realIts.push_back(smr::semgr.findreal(i));
    }

    /* 变量存在性检查 */
    if (dmtxIts[0] == smr::semgr.getdmtxEnd() && realIts[0] == smr::semgr.getrealEnd()) {
        smr::semgr.seterr(laxb::cmdhr.getName() + ": Variable \"" + varnIn1 + "\" is not exist");
        return false;
    }
    if (dmtxIts[1] == smr::semgr.getdmtxEnd() && realIts[1] == smr::semgr.getrealEnd()) {
        smr::semgr.seterr(laxb::cmdhr.getName() + ": Variable \"" + varnIn2 + "\" is not exist");
        return false;
    }

    if (dmtxIts[0] == smr::semgr.getdmtxEnd() || dmtxIts[1] == smr::semgr.getdmtxEnd()) { // 必须为两个矩阵
        smr::semgr.seterr(laxb::cmdhr.getName() + ": Need 2 matrix variable, but not");
        return false;
    }

    /* 维度检查 */
    if (core::dimeq(dmtxIts[0]->second, dmtxIts[1]->second) == false) {
        smr::semgr.seterr(laxb::cmdhr.getName() + ": Dimension mismatch between \"" + varnIn1 + "\" and \"" + varnIn2 + "\"");
        return false;
    }

    /* 计算 */
    core::dmtx output = core::times(dmtxIts[0]->second, dmtxIts[1]->second);
    if (type == 1) {
        smr::semgr.adddmtx(laxb::cmdhr.getCmdtoken()[2].substr(2), output); // :varn :varn ::varn
    }
    if (shouldOut) {
        smr::semgr << "\n"
                   << output;
    }

    return true;
}

/* 逐元素除
 * - divede :varn :varn ::varn
 * - divede :varn :varn */
bool divide() {
    bool shouldOut = laxb::cmdhr.semicolonDel();
    if (laxb::cmdhr.isempty(laxb::cmdhr.getName())) {
        return false;
    }

    std::size_t type{0};
    if (laxb::cmdhr.sortToken(std::vector<laxb::Cmdt>{laxb::Cmdt::IN, laxb::Cmdt::IN, laxb::Cmdt::OUT}) == true) { // :A :B ::C
        type = 1;
    }
    else if (laxb::cmdhr.sortToken(std::vector<laxb::Cmdt>{laxb::Cmdt::IN, laxb::Cmdt::IN}) == true) { // :A :B
        type = 2;
    }
    else {
        smr::semgr.seterr(laxb::cmdhr.getName() + ": Argument(s) error");
        return false;
    }

    std::string varnIn1(laxb::cmdhr.getCmdtoken()[0].substr(1)), varnIn2(laxb::cmdhr.getCmdtoken()[1].substr(1)); // :varn :varn
    std::vector<decltype(smr::semgr.getdmtxEnd())> dmtxIts{};
    std::vector<decltype(smr::semgr.getrealEnd())> realIts{};
    std::vector<std::string> varns{varnIn1, varnIn2}; // 这样子好算

    for (const auto& i : varns) { // 矩阵查找
        dmtxIts.push_back(smr::semgr.finddmtx(i));
    }
    for (const auto& i : varns) { // 实数查找
        realIts.push_back(smr::semgr.findreal(i));
    }

    /* 变量存在性检查 */
    if (dmtxIts[0] == smr::semgr.getdmtxEnd() && realIts[0] == smr::semgr.getrealEnd()) {
        smr::semgr.seterr(laxb::cmdhr.getName() + ": Variable \"" + varnIn1 + "\" is not exist");
        return false;
    }
    if (dmtxIts[1] == smr::semgr.getdmtxEnd() && realIts[1] == smr::semgr.getrealEnd()) {
        smr::semgr.seterr(laxb::cmdhr.getName() + ": Variable \"" + varnIn2 + "\" is not exist");
        return false;
    }

    if (dmtxIts[0] == smr::semgr.getdmtxEnd() || dmtxIts[1] == smr::semgr.getdmtxEnd()) { // 必须为两个矩阵
        smr::semgr.seterr(laxb::cmdhr.getName() + ": Need 2 matrix variable, but not");
        return false;
    }

    /* 维度检查 */
    if (core::dimeq(dmtxIts[0]->second, dmtxIts[1]->second) == false) {
        smr::semgr.seterr(laxb::cmdhr.getName() + ": Dimension mismatch between \"" + varnIn1 + "\" and \"" + varnIn2 + "\"");
        return false;
    }

    /* 计算 */
    try {
        core::divide(dmtxIts[0]->second, dmtxIts[1]->second);
    } catch (std::invalid_argument& e) {
        smr::semgr.seterr(laxb::cmdhr.getName() + ": " + e.what());
        return false;
    }

    core::dmtx output = core::divide(dmtxIts[0]->second, dmtxIts[1]->second);

    if (type == 1) {
        smr::semgr.adddmtx(laxb::cmdhr.getCmdtoken()[2].substr(2), output); // :varn :varn ::varn
    }
    if (shouldOut) {
        smr::semgr << "\n"
                   << output;
    }

    return true;
}

/* 逐元素幂
 * - power :varn :varn ::varn
 * - power :varn :varn */
bool power() {
    bool shouldOut = laxb::cmdhr.semicolonDel();
    if (laxb::cmdhr.isempty(laxb::cmdhr.getName())) {
        return false;
    }

    std::size_t type{0};
    if (laxb::cmdhr.sortToken(std::vector<laxb::Cmdt>{laxb::Cmdt::IN, laxb::Cmdt::IN, laxb::Cmdt::OUT}) == true) { // :A :B ::C
        type = 1;
    }
    else if (laxb::cmdhr.sortToken(std::vector<laxb::Cmdt>{laxb::Cmdt::IN, laxb::Cmdt::IN}) == true) { // :A :B
        type = 2;
    }
    else {
        smr::semgr.seterr(laxb::cmdhr.getName() + ": Argument(s) error");
        return false;
    }

    std::string varnIn1(laxb::cmdhr.getCmdtoken()[0].substr(1)), varnIn2(laxb::cmdhr.getCmdtoken()[1].substr(1)); // :varn :varn
    std::vector<decltype(smr::semgr.getdmtxEnd())> dmtxIts{};
    std::vector<decltype(smr::semgr.getrealEnd())> realIts{};
    std::vector<std::string> varns{varnIn1, varnIn2}; // 这样子好算

    for (const auto& i : varns) { // 矩阵查找
        dmtxIts.push_back(smr::semgr.finddmtx(i));
    }
    for (const auto& i : varns) { // 实数查找
        realIts.push_back(smr::semgr.findreal(i));
    }

    /* 变量存在性检查 */
    if (dmtxIts[0] == smr::semgr.getdmtxEnd() && realIts[0] == smr::semgr.getrealEnd()) {
        smr::semgr.seterr(laxb::cmdhr.getName() + ": Variable \"" + varnIn1 + "\" is not exist");
        return false;
    }
    if (dmtxIts[1] == smr::semgr.getdmtxEnd() && realIts[1] == smr::semgr.getrealEnd()) {
        smr::semgr.seterr(laxb::cmdhr.getName() + ": Variable \"" + varnIn2 + "\" is not exist");
        return false;
    }

    if (dmtxIts[0] == smr::semgr.getdmtxEnd() || dmtxIts[1] == smr::semgr.getdmtxEnd()) { // 必须为两个矩阵
        smr::semgr.seterr(laxb::cmdhr.getName() + ": Need 2 matrix variable, but not");
        return false;
    }

    /* 维度检查 */
    if (core::dimeq(dmtxIts[0]->second, dmtxIts[1]->second) == false) {
        smr::semgr.seterr(laxb::cmdhr.getName() + ": Dimension mismatch between \"" + varnIn1 + "\" and \"" + varnIn2 + "\"");
        return false;
    }

    /* 计算 */
    core::dmtx output = core::power(dmtxIts[0]->second, dmtxIts[1]->second);
    if (type == 1) {
        smr::semgr.adddmtx(laxb::cmdhr.getCmdtoken()[2].substr(2), output); // :varn :varn ::varn
    }

    if (shouldOut) {
        smr::semgr << "\n"
                   << output;
    }

    return true;
}

/* ==== 矩阵创建与操作 ==== */
/* - eye {num}
 * - eye ::A {num} */
bool eye() {
    bool shouldOut = laxb::cmdhr.semicolonDel(); // 末尾分号
    if (laxb::cmdhr.isempty(laxb::cmdhr.getName())) {
        return false;
    }

    /* 排序 args */
    std::size_t type{0};
    if (laxb::cmdhr.sortToken(std::vector<laxb::Cmdt>{laxb::Cmdt::CB}) == true) { // eye {num}
        type = 1;
    }
    else if (laxb::cmdhr.sortToken(std::vector<laxb::Cmdt>{laxb::Cmdt::CB, laxb::Cmdt::OUT}) == true) { // eye ::A {num}
        type = 2;
    }
    else {
        smr::semgr.seterr(laxb::cmdhr.getName() + ": Argument(s) error");
        return false;
    }

    /* 分析、使用 */
    core::dmtx output{};
    std::vector<double> dimens = laxb::cmdhr.curlyToken(laxb::cmdhr.getCmdtoken()[0]);
    if (dimens.size() != 1) {
        smr::semgr.seterr(laxb::cmdhr.getName() + ": Argument(s) error in \"" + laxb::cmdhr.getCmdtoken()[0] + "\"");
        return false;
    }
    if (static_cast<int>(dimens[0]) == 0) { // 传递 (-1, 1) 的分数，导致此处判断为非 0，进而构造出空矩阵，在 << 处 .fail() 被置位，报错
        smr::semgr.seterr(laxb::cmdhr.getName() + ": 0 matrix is illegal: \"" + laxb::cmdhr.getCmdtoken()[0] + "\"");
        return false;
    }

    output = core::dmtx(dimens[0]); // 制造矩阵
    for (core::dmtx::mtxSizet i = 0; i < output.getRowSize(); ++i) {
        output(i, i) = 1;
    }

    if (type == 2) {
        smr::semgr.adddmtx(laxb::cmdhr.getCmdtoken()[1].substr(2), output); // ::A
    }

    /* 判断是否写入 smr::semgr::cptoup_ */
    if (shouldOut) {
        smr::semgr << "\n"
                   << output;
    }

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

/* ==== 矩阵分解 ==== */
/* - lu :A（默认输出 L、U）
 * - lu :A ::L ::U
 * - lu :A -a（输出 L、U、P）
 * - lu :A ::L ::U ::P */
bool lu() {
    bool shouldOut = laxb::cmdhr.semicolonDel();
    if (laxb::cmdhr.isempty(laxb::cmdhr.getName())) {
        return false;
    }

    std::size_t type{0};
    if (laxb::cmdhr.sortToken(std::vector<laxb::Cmdt>{laxb::Cmdt::IN}) == true) { // :A
        type = 1;
    }
    else if (laxb::cmdhr.sortToken(std::vector<laxb::Cmdt>{laxb::Cmdt::IN, laxb::Cmdt::OUT, laxb::Cmdt::OUT}) == true) { // :A ::L ::U
        type = 2;
    }
    else if (laxb::cmdhr.sortToken(std::vector<laxb::Cmdt>{laxb::Cmdt::IN, laxb::Cmdt::OPT}) == true) { // -a :A
        type = 3;
    }
    else if (laxb::cmdhr.sortToken(std::vector<laxb::Cmdt>{laxb::Cmdt::IN, laxb::Cmdt::OUT, laxb::Cmdt::OUT, laxb::Cmdt::OUT}) == true) { // :A ::L ::U ::P
        type = 4;
    }
    else {
        smr::semgr.seterr(laxb::cmdhr.getName() + ": Argument(s) error");
        return false;
    }

    std::string varnIn(laxb::cmdhr.getCmdtoken()[0].substr(1)); // :A
    auto it = smr::semgr.finddmtx(varnIn);
    if (it == smr::semgr.getdmtxEnd()) {
        smr::semgr.seterr(laxb::cmdhr.getName() + ": Matrix \"" + varnIn + "\"not found");
        return false;
    }
    if (!(it->second.isSquare())) {
        smr::semgr.seterr(laxb::cmdhr.getName() + ": Matrix \"" + varnIn + "\"is not a square matrix");
        return false;
    }

    decomp::BaseDecomposer decomper(it->second); // 初始化分解器
    std::vector<core::dmtx> LUP = decomper.lu(); // 分解

    if (type == 3) {
        if (laxb::cmdhr.getCmdtoken()[1] == "-a") { // "-a"
            true; // 待拓展
        }
        else {
            smr::semgr.seterr(laxb::cmdhr.getName() + ": There is no option \"" + laxb::cmdhr.getCmdtoken()[1].substr(1) + "\"");
            return false;
        }
    }

    if (type == 2 || type == 4) {
        smr::semgr.adddmtx(laxb::cmdhr.getCmdtoken()[1].substr(2), LUP[0]); // L
        smr::semgr.adddmtx(laxb::cmdhr.getCmdtoken()[2].substr(2), LUP[1]); // U
        if (type == 4) {
            smr::semgr.adddmtx(laxb::cmdhr.getCmdtoken()[3].substr(2), LUP[2]); // P
        }
    }

    if (shouldOut) {
        smr::semgr << LUP[0] << LUP[1]; // L U
        if (type == 3 || type == 4) {
            smr::semgr << LUP[2]; // P
        }
    }

    return true;
}