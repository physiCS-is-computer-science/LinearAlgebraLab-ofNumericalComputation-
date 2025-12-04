// =====================================================================
// 所有命令都有相似的流程，调用史上最伟大的 LaxbIO、SessionManager 模块解析传递数据
// - 每个命令的流程：
//    1.去除末尾分号（记录输出标志），防止参数排序分析函数 sortToken() 返回错误消息（分号处理之后再 cmdhr.isempty("") 检查）
//    2.判断 cmdToken_ 数量，不符合则返回 false
//    3.显示指定参数顺序和种类，调用 sortToken() 函数分析排序 args
//    4.逐项分析返回的有序 args，使用
//    5.根据 ';' 判断是否写入 smr::semgr::cptoup_
// =====================================================================

#include "cmdlib.hpp"
#include "commandIO.hpp"
#include "core/matrix.hpp"
#include "decompositions/advanced_decomp.hpp"
#include "decompositions/basic_decomp.hpp"
#include "session_manager.hpp"
#include "solvers/least_squares_solver.hpp"
#include "solvers/linear_solvers.hpp"
#include "spaces/subspaces.hpp"
#include "utils/factory.hpp"
#include <Windows.h> // 为了 system("cls")
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <utility>

using laxb::Cmdt::CB;
using laxb::Cmdt::INID;
using laxb::Cmdt::NOTF;
using laxb::Cmdt::OPT;
using laxb::Cmdt::OUTID;
using laxb::Cmdt::SB;

using lc = laxb::CmdHandler;

/* ==== 基本命令 ==== */
/* 关闭程序或变量空间
 * - quit -e/f */
bool quit() {
    laxb::tvv argset{{OPT}};
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }

    if (laxb::cmdhr.getcmdtk()[0] == "-e") { // 退出程序
        smr::semgr.swich();
    }
    else if (laxb::cmdhr.getcmdtk()[0] == "-f") { // 恢复到临时会话
        smr::semgr.setpath(); // 默认为临时会话标识 "#"
        smr::semgr.clearSpace(); // 清除内存中所有变量
    }
    else {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Argument(s) error in \"" + laxb::cmdhr.getcmdtk()[0] + "\""
        );
        return false;
    }

    return true;
}

/* 显示变量，无论有无分号都输出，因而不使用 outDetermine()
 * - show -a/l
 * - show :varn */
bool show() {
    laxb::tvv argset{
        {OPT},
        {INID}
    };
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }

    std::size_t cnt{1}; // 计数器
    if (lc::type_g == 1) {
        if (smr::semgr.dmSpace_.empty() && smr::semgr.realSpace_.empty()) {
            smr::semgr << "NaN";
            return true;
        }

        if (laxb::cmdhr.getcmdtk()[0] == "-a") { // 所有变量
            smr::semgr << "\n\n";
            for (const auto& i : smr::semgr.dmSpace_) { // 矩阵/向量
                smr::semgr << i.first << ": " << i.second << "---\n";
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
        else if (laxb::cmdhr.getcmdtk()[0] == "-l") { // 所有变量名列表
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
            smr::semgr.seterr(
                laxb::cmdhr.getname() +
                ": Argument(s) error in \"" + laxb::cmdhr.getcmdtk()[0] + "\""
            );
            return false;
        }
    }
    else if (lc::type_g == 2) { // 查看特定变量
        std::string name(laxb::cmdhr.getcmdtk()[0].substr(1));
        auto itdmtx = smr::semgr.dmSpace_.find(name);
        auto itreal = smr::semgr.realSpace_.find(name);
        if (itdmtx == smr::semgr.dmSpace_.end() &&
            itreal == smr::semgr.realSpace_.end()) {
            smr::semgr << "Variable \"" + name + "\" not exist";
            return true;
        }
        else {
            if (itdmtx != smr::semgr.dmSpace_.end()) {
                smr::semgr << itdmtx->second;
            }
            if (itreal != smr::semgr.realSpace_.end()) {
                smr::semgr << itreal->second;
            }
        }
    }

    return true;
}

/* 创建变量，此函数会检查所有变量名，如果有重名则不允许创建新变量（除此之外的所有创建、更改变量操作都会覆盖原变量）
 * var ::varn {a}
 * var ::varn [a b; c b] */
bool var() {
    laxb::tvv argset{
        {OUTID, CB},
        {OUTID, SB}
    };
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }

    std::string varn(laxb::cmdhr.getcmdtk()[0].substr(2)); // 去除标识符的变量/矩阵名

    /* 查找是否存在该变量 */
    auto dmtxIt = smr::semgr.finddmtx(varn);
    auto realIt = smr::semgr.findreal(varn);
    if (dmtxIt != smr::semgr.getdmtxEnd() ||
        realIt != smr::semgr.getrealEnd()
    ) { // 找到任何一个都不行
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Variable \"" + varn + "\" exist"
        );
        return false;
    }

    if (lc::type_g == 1) { // ::varn {a}
        std::vector<double> temp = laxb::cmdhr.curlyToken(laxb::cmdhr.getcmdtk()[1]); // {a}
        if (temp.size() != 1) { // 参数太多了
            smr::semgr.seterr(
                laxb::cmdhr.getname() +
                ": Argument(s) error in \"" + laxb::cmdhr.getcmdtk()[1] + "\""
            );
            return false;
        }

        smr::semgr.addreal(varn, temp[0]); // 上方检查过，矩阵与实数都没有同名变量，因而可以直接添加
        laxb::cmdhr.outDetermine(temp[0]);
    }
    else if (lc::type_g == 2) { // ::varn [a b; c d]
        core::dmtx mtx(laxb::cmdhr.squareToken(laxb::cmdhr.getcmdtk()[1])); // 矩阵输入错误返回的是空矩阵
        if (mtx.isEmpty()) {
            smr::semgr.seterr(
                laxb::cmdhr.getname() +
                ": Error in matrix \"" + laxb::cmdhr.getcmdtk()[1] + "\""
            );
            return false;
        }

        smr::semgr.adddmtx(varn, mtx); // 上方检查过，矩阵与实数都没有同名变量，因而可以直接添加
        laxb::cmdhr.outDetermine(mtx);
    }

    // 此处本应为 shouldOut 检查，不过移到以上两个 if 选择之内了

    return true;
}

/* 删除变量（laxb::cmdhr 的友元）
 * - del :varn
 * - del -a */
bool del() {
    laxb::tvv argset{
        {INID},
        {OPT}
    };
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }

    if (lc::type_g == 1) {
        std::string varn(laxb::cmdhr.getcmdtk()[0].substr(1)); // :varn

        /* 查找变量是否存在 */
        auto dmtxIt = smr::semgr.dmSpace_.find(varn);
        auto realIt = smr::semgr.realSpace_.find(varn);
        if (dmtxIt == smr::semgr.dmSpace_.end() &&
            realIt == smr::semgr.realSpace_.end()
        ) {
            smr::semgr.seterr(
                laxb::cmdhr.getname() +
                ": Variable \"" + varn + "\" not exist"
            );
            return false;
        }

        /* 删除对应变量 */
        if (dmtxIt != smr::semgr.dmSpace_.end()) {
            laxb::cmdhr.outDetermine(dmtxIt->second);
            smr::semgr.dmSpace_.erase(dmtxIt);
        }
        else {
            laxb::cmdhr.outDetermine(realIt->second);
            smr::semgr.realSpace_.erase(realIt);
        }
    }
    else if (lc::type_g == 2) {
        if (laxb::cmdhr.getcmdtk()[0] == "-a") {
            smr::semgr.clearSpace();
        }
        else {
            smr::semgr.seterr(
                laxb::cmdhr.getname() +
                ": Argument(s) error in \"" + laxb::cmdhr.getcmdtk()[0] + "\""
            );
            return false;
        }
    }

    return true;
}

/* - save ::filename（重名文件则写入失败）
 * - save ::falename -f（强制写入） */
bool save() {
    laxb::tvv argset{
        {OUTID},
        {OUTID, OPT}
    };
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }

    std::string filename = laxb::cmdhr.getcmdtk()[0].substr(2); // ::filename
    if (lc::type_g == 1) { // ::filename
        std::string fullPath = smr::semgr.filePath(filename); // 获取文件路径
        std::ifstream tmpFin(fullPath); // 打开文件测试文件是否存在
        if (!tmpFin.fail()) { // 读取成功，即有重名文件时
            smr::semgr.seterr(
                laxb::cmdhr.getname() +
                ": File \"" + filename + "\" is exist, "
                "if you want to force write overwriting, add -f"
            );
            return false;
        }

        smr::semgr.wfile(filename); // 无重名文件时
    }
    else if (lc::type_g == 2 && laxb::cmdhr.getcmdtk()[1] == "-f") {
        smr::semgr.wfile(filename);
    }
    else {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Save file \"" + filename + "\" failed"
        );
        return false;
    }

    laxb::cmdhr.outDetermine("\"./user_session_files/" + filename + ".lal\" saved");

    return true;
}

/* 读取失败时，无论命令是绑定文件版还是不绑定版，都返回 false，并且回档变量空间（不改变变量空间）
 * - load :filename（载入变量至当前会话，不修改文件 filename，但修改当前文件会话）
 * - load :filename -c（改变会话文件绑定，每次操作都会修改文件 filename） */
bool load() {
    laxb::tvv argset{
        {INID},
        {INID, OPT}
    };
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }

    std::string filename = laxb::cmdhr.getcmdtk()[0].substr(1); // :filename

    /* 防止意外，回档用 */
    auto dmSpaceCp = smr::semgr.dmSpace_;
    auto realSpaceCp = smr::semgr.realSpace_;

    if (lc::type_g == 1) { // :filename
        if (smr::semgr.rfile(filename) != true) { // 失败时，回档
            smr::semgr.dmSpace_ = dmSpaceCp;
            smr::semgr.realSpace_ = realSpaceCp;
            smr::semgr.seterr(
                laxb::cmdhr.getname() +
                ": Read file \"" + filename + "\" failed"
            );
            return false;
        }
    }
    else if (lc::type_g == 2 &&
             laxb::cmdhr.getcmdtk()[1] == "-c"
    ) { // :filename -m
        /* 清除当前会话 a 在内存的变量空间（没回到主函数所以不会同步更改），否则同步时会把当前会话 a 的变量同步到新文件会话 b */
        smr::semgr.clearSpace();

        if (smr::semgr.rfile(filename) == true) { // 读取文件成功时
            smr::semgr.setpath(filename); // 绑定到文件会话 b
        }
        else { // 失败时，不改变文件会话 a 的绑定，恢复变量空间（回档）
            smr::semgr.dmSpace_ = dmSpaceCp;
            smr::semgr.realSpace_ = realSpaceCp;
            smr::semgr.seterr(
                laxb::cmdhr.getname() +
                ": Read file \"" + filename + "\" failed"
            );
            return false;
        }
    }
    else {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Read file \"" + filename + "\" failed"
        );
        return false;
    }

    laxb::cmdhr.outDetermine("\"./user_session_files/" + filename + ".lal\" loaded");

    return true;
}

/* 清屏 */
bool cls() {
    if (!laxb::cmdhr.getcmdtk().empty()) {
        smr::semgr.seterr(laxb::cmdhr.getname() + ": This command has no arguments");
        return false;
    }
    system("cls");
    return true;
}

/* 是否有分号都输出，因而不使用 outDetermine()
 * - help :cmdname
 * - help -l/a */
bool help() {
    laxb::tvv argset{
        {INID},
        {OPT}
    };
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }

    if (lc::type_g == 1) { // :cmdname
        auto it = laxb::manual.find(laxb::cmdhr.getcmdtk()[0].substr(1));
        if (it == laxb::manual.end()) {
            smr::semgr.seterr(
                laxb::cmdhr.getname() +
                ": Command \"" + laxb::cmdhr.getcmdtk()[0].substr(1) +
                "\" not exist"
            );
            return false;
        }
        smr::semgr << "\n\n"
                   << it->first << '\t' << it->second.first << "\n---\n"
                   << it->second.second << '\n';
    }
    else if (lc::type_g == 2 && laxb::cmdhr.getcmdtk()[0] == "-a") { // -a
        smr::semgr << "\n\n";
        for (const auto& i : laxb::manual) {
            smr::semgr << i.first << '\t' << i.second.first << '\n';
        }
    }
    else if (lc::type_g == 2 && laxb::cmdhr.getcmdtk()[0] == "-l") { // -l
        smr::semgr << "\n\n";
        std::size_t cnt = 0;
        for (const auto& i : laxb::manual) { // 五个一行
            ++cnt;
            smr::semgr << i.first << '\t';
            if (cnt % 5 == 0) {
                smr::semgr << '\n';
            }
        }
        if (cnt % 5 != 0) { // 满五时上方已经输出 '\n'
            smr::semgr << '\n';
        }
    }
    else {
        smr::semgr.seterr(laxb::cmdhr.getname() + ": Argument(s) error");
        return false;
    }

    return true;
}

/* ==== 矩阵运算 ==== */
/* 矩阵转置
 * - 1.transpose :A
 * - 2.transpose :A ::B */
bool transpose() {
    laxb::tvv argset{{INID}, {INID, OUTID}};
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }

    std::string in{laxb::cmdhr.getcmdtk()[0].substr(1)}; // :A
    auto inIt{smr::semgr.finddmtx(in)};
    if (inIt == smr::semgr.getdmtxEnd()) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Matrix \"" + in + "\"not found"
        );
        return false;
    }

    core::dmtx oup = ~(inIt->second);

    if (lc::type_g == 2) { // :A ::B
        smr::semgr.adddmtx(laxb::cmdhr.getcmdtk()[1].substr(2), oup);
    }

    laxb::cmdhr.outDetermine(oup);
    return true;
}

/* - plus :varn :varn ::varn
 * - plus :varn :varn */
bool plus() {
    laxb::tvv argset{
        {INID, INID, OUTID},
        {INID, INID}
    };
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }

    std::string varnIn1(laxb::cmdhr.getcmdtk()[0].substr(1)); // :varn
    std::string varnIn2(laxb::cmdhr.getcmdtk()[1].substr(1)); // :varn
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
    if (dmtxIts[0] == smr::semgr.getdmtxEnd() &&
        realIts[0] == smr::semgr.getrealEnd()
    ) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Variable \"" + varnIn1 + "\" is not exist"
        );
        return false;
    }
    if (dmtxIts[1] == smr::semgr.getdmtxEnd() &&
        realIts[1] == smr::semgr.getrealEnd()
    ) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Variable \"" + varnIn2 + "\" is not exist"
        );
        return false;
    }

    /* 类型一致性检查 */
    if (dmtxIts[0] != smr::semgr.getdmtxEnd() &&
        dmtxIts[1] != smr::semgr.getdmtxEnd()
    ) {
        try { // 防止矩阵维度不匹配
            dmtxIts[0]->second + dmtxIts[1]->second;
        }
        catch (std::invalid_argument& e) {
            smr::semgr.seterr(laxb::cmdhr.getname() + ": " + e.what());
            return false;
        }

        core::dmtx output = dmtxIts[0]->second + dmtxIts[1]->second; // 再执行一次

        if (lc::type_g == 1) { // plus :varn :varn ::varn
            smr::semgr.adddmtx(laxb::cmdhr.getcmdtk()[2].substr(2), output); // ::varn
        }
        laxb::cmdhr.outDetermine(output);
    }
    else if (realIts[0] != smr::semgr.getrealEnd() &&
             realIts[1] != smr::semgr.getrealEnd()
    ) {
        double output = realIts[0]->second + realIts[1]->second;

        if (lc::type_g == 1) { // plus :varn :varn ::varn
            smr::semgr.addreal(laxb::cmdhr.getcmdtk()[2].substr(2), output); // ::varn
        }
        laxb::cmdhr.outDetermine(output);
    }
    else {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Variables \"" + varnIn1 + "\" "
            "and \"" + varnIn2 + "\" are of different types"
        );
        return false;
    }

    return true;
}

/* 被减数取决于输入（:）变量的左右，和输出（::）变量位置无关
 * - minus :varn :varn ::varn
 * - minus :varn :varn */
bool minus() {
    laxb::tvv argset{
        {INID, INID, OUTID},
        {INID, INID}
    };
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }

    std::string varnIn1(laxb::cmdhr.getcmdtk()[0].substr(1));
    std::string varnIn2(laxb::cmdhr.getcmdtk()[1].substr(1)); // :varn :varn
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
    if (dmtxIts[0] == smr::semgr.getdmtxEnd() &&
        realIts[0] == smr::semgr.getrealEnd()
    ) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Variable \"" + varnIn1 + "\" is not exist"
        );
        return false;
    }
    if (dmtxIts[1] == smr::semgr.getdmtxEnd() &&
        realIts[1] == smr::semgr.getrealEnd()
    ) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Variable \"" + varnIn2 + "\" is not exist"
        );
        return false;
    }

    /* 类型一致性检查 */
    if (dmtxIts[0] != smr::semgr.getdmtxEnd() &&
        dmtxIts[1] != smr::semgr.getdmtxEnd()
    ) {
        try { // 防止矩阵维度不匹配
            dmtxIts[0]->second - dmtxIts[1]->second;
        }
        catch (std::invalid_argument& e) {
            smr::semgr.seterr(laxb::cmdhr.getname() + ": " + e.what());
            return false;
        }

        core::dmtx output = dmtxIts[0]->second - dmtxIts[1]->second; // 再执行一次

        if (lc::type_g == 1) { // plus :varn :varn ::varn
            smr::semgr.adddmtx(laxb::cmdhr.getcmdtk()[2].substr(2), output); // ::varn
        }
        laxb::cmdhr.outDetermine(output);
    }
    else if (realIts[0] != smr::semgr.getrealEnd() &&
             realIts[1] != smr::semgr.getrealEnd()
    ) {
        double output = realIts[0]->second - realIts[1]->second;

        if (lc::type_g == 1) { // plus :varn :varn ::varn
            smr::semgr.addreal(laxb::cmdhr.getcmdtk()[2].substr(2), output); // ::varn
        }
        laxb::cmdhr.outDetermine(output);
    }
    else {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Variables \"" + varnIn1 + "\" "
            "and \"" + varnIn2 + "\" are of different types"
        );
        return false;
    }

    return true;
}

/* 矩阵乘（times 为逐元素）
 * mtimes :varn :varn ::varn
 * mtimes :varn :varn */
bool mtimes() {
    laxb::tvv argset{
        {INID, INID, OUTID},
        {INID, INID}
    };
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }

    std::string varnIn1(laxb::cmdhr.getcmdtk()[0].substr(1));
    std::string varnIn2(laxb::cmdhr.getcmdtk()[1].substr(1)); // :varn :varn
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
    if (dmtxIts[0] == smr::semgr.getdmtxEnd() &&
        realIts[0] == smr::semgr.getrealEnd()
    ) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Variable \"" + varnIn1 + "\" is not exist"
        );
        return false;
    }
    if (dmtxIts[1] == smr::semgr.getdmtxEnd() &&
        realIts[1] == smr::semgr.getrealEnd()
    ) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Variable \"" + varnIn2 + "\" is not exist"
        );
        return false;
    }

    core::dmtx output{};

    /* 计算 */
    if (dmtxIts[0] != smr::semgr.getdmtxEnd() &&
        dmtxIts[1] != smr::semgr.getdmtxEnd()
    ) { // matrix * matrix
        if (dmtxIts[0]->second.getColSize() != dmtxIts[1]->second.getRowSize()) {
            smr::semgr.seterr(
                laxb::cmdhr.getname() +
                ": Dimension dismatch between matrix \"" + varnIn1 + "\" "
                "and \"" + varnIn2 + "\""
            );
            return false;
        }
        output = dmtxIts[0]->second * dmtxIts[1]->second;
    }
    else if (dmtxIts[0] != smr::semgr.getdmtxEnd() &&
             realIts[1] != smr::semgr.getrealEnd()
    ) {
        output = dmtxIts[0]->second * realIts[1]->second;
    }
    else if (realIts[0] != smr::semgr.getrealEnd() &&
             dmtxIts[1] != smr::semgr.getdmtxEnd()
    ) {
        output = realIts[0]->second * dmtxIts[1]->second;
    }

    if (lc::type_g == 1) { // mtimes :varn :varn ::varn
        smr::semgr.adddmtx(
            laxb::cmdhr.getcmdtk()[2].substr(2),
            output
        ); // ::varn
    }
    laxb::cmdhr.outDetermine(output);

    return true;
}

/* 逐元素乘
 * - times :varn :varn ::varn
 * - times :varn :varn */
bool times() {
    laxb::tvv argset{
        {INID, INID, OUTID},
        {INID, INID}
    };
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }

    std::string varnIn1(laxb::cmdhr.getcmdtk()[0].substr(1));
    std::string varnIn2(laxb::cmdhr.getcmdtk()[1].substr(1)); // :varn :varn
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
    if (dmtxIts[0] == smr::semgr.getdmtxEnd() &&
        realIts[0] == smr::semgr.getrealEnd()
    ) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Variable \"" + varnIn1 + "\" is not exist"
        );
        return false;
    }
    if (dmtxIts[1] == smr::semgr.getdmtxEnd() &&
        realIts[1] == smr::semgr.getrealEnd()
    ) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Variable \"" + varnIn2 + "\" is not exist"
        );
        return false;
    }

    if (dmtxIts[0] == smr::semgr.getdmtxEnd() ||
        dmtxIts[1] == smr::semgr.getdmtxEnd()
    ) { // 必须为两个矩阵
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Need 2 matrix variable, but not"
        );
        return false;
    }

    /* 维度检查 */
    if (core::dimeq(dmtxIts[0]->second, dmtxIts[1]->second) == false) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Dimension mismatch between \"" + varnIn1 + "\" "
            "and \"" + varnIn2 + "\""
        );
        return false;
    }

    /* 计算 */
    core::dmtx output = core::times(dmtxIts[0]->second, dmtxIts[1]->second);
    if (lc::type_g == 1) {
        smr::semgr.adddmtx(
            laxb::cmdhr.getcmdtk()[2].substr(2),
            output
        ); // :varn :varn ::varn
    }
    laxb::cmdhr.outDetermine(output);

    return true;
}

/* 逐元素除
 * - divede :varn :varn ::varn
 * - divede :varn :varn */
bool divide() {
    laxb::tvv argset{
        {INID, INID, OUTID},
        {INID, INID}
    };
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }

    std::string varnIn1(laxb::cmdhr.getcmdtk()[0].substr(1));
    std::string varnIn2(laxb::cmdhr.getcmdtk()[1].substr(1)); // :varn :varn
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
    if (dmtxIts[0] == smr::semgr.getdmtxEnd() &&
        realIts[0] == smr::semgr.getrealEnd()
    ) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Variable \"" + varnIn1 + "\" is not exist"
        );
        return false;
    }
    if (dmtxIts[1] == smr::semgr.getdmtxEnd() &&
        realIts[1] == smr::semgr.getrealEnd()
    ) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Variable \"" + varnIn2 + "\" is not exist"
        );
        return false;
    }

    if (dmtxIts[0] == smr::semgr.getdmtxEnd() ||
        dmtxIts[1] == smr::semgr.getdmtxEnd()
    ) { // 必须为两个矩阵
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Need 2 matrix variable, but not"
        );
        return false;
    }

    /* 维度检查 */
    if (core::dimeq(dmtxIts[0]->second, dmtxIts[1]->second) == false) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Dimension mismatch between \"" + varnIn1 + "\" "
            "and \"" + varnIn2 + "\""
        );
        return false;
    }

    /* 计算 */
    try {
        core::divide(dmtxIts[0]->second, dmtxIts[1]->second);
    }
    catch (std::invalid_argument& e) {
        smr::semgr.seterr(laxb::cmdhr.getname() + ": " + e.what());
        return false;
    }

    core::dmtx output = core::divide(dmtxIts[0]->second, dmtxIts[1]->second);

    if (lc::type_g == 1) {
        smr::semgr.adddmtx( // :varn :varn ::varn
            laxb::cmdhr.getcmdtk()[2].substr(2),
            output
        );
    }
    laxb::cmdhr.outDetermine(output);

    return true;
}

/* 逐元素幂
 * - power :varn :varn ::varn
 * - power :varn :varn */
bool power() {
    laxb::tvv argset{
        {INID, INID, OUTID},
        {INID, INID}
    };
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }

    std::string varnIn1(laxb::cmdhr.getcmdtk()[0].substr(1));
    std::string varnIn2(laxb::cmdhr.getcmdtk()[1].substr(1)); // :varn :varn
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
    if (dmtxIts[0] == smr::semgr.getdmtxEnd() &&
        realIts[0] == smr::semgr.getrealEnd()
    ) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Variable \"" + varnIn1 + "\" is not exist"
        );
        return false;
    }
    if (dmtxIts[1] == smr::semgr.getdmtxEnd() &&
        realIts[1] == smr::semgr.getrealEnd()
    ) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Variable \"" + varnIn2 + "\" is not exist"
        );
        return false;
    }

    if (dmtxIts[0] == smr::semgr.getdmtxEnd() ||
        dmtxIts[1] == smr::semgr.getdmtxEnd()
    ) { // 必须为两个矩阵
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Need 2 matrix variable, but not"
        );
        return false;
    }

    /* 维度检查 */
    if (core::dimeq(dmtxIts[0]->second, dmtxIts[1]->second) == false) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Dimension mismatch between \"" + varnIn1 + "\" "
            "and \"" + varnIn2 + "\""
        );
        return false;
    }

    /* 计算 */
    core::dmtx output = core::power(dmtxIts[0]->second, dmtxIts[1]->second);
    if (lc::type_g == 1) {
        smr::semgr.adddmtx( // :varn :varn ::varn
            laxb::cmdhr.getcmdtk()[2].substr(2),
            output
        );
    }

    laxb::cmdhr.outDetermine(output);

    return true;
}

/* 矩阵所有元素求和
 * 1.sum :A
 * 2.sum :A ::a */
bool sum() {
    laxb::tvv argset{{INID}, {INID, OUTID}};
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }

    std::string in{laxb::cmdhr.getcmdtk()[0].substr(1)}; // :A
    auto inIt{smr::semgr.finddmtx(in)};
    if (inIt == smr::semgr.getdmtxEnd()) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Matrix \"" + in + "\"not found"
        );
        return false;
    }

    double oup{inIt->second.getElementSum()};

    if (lc::type_g == 2) { // :A ::a
        smr::semgr.addreal(laxb::cmdhr.getcmdtk()[1].substr(2), oup);
    }

    laxb::cmdhr.outDetermine(oup);
    return true;
}

/* 计算矩阵所有元素平均值
 * - 1.mean :A
 * - 2.mean :A ::a */
bool mean() {
    laxb::tvv argset{{INID}, {INID, OUTID}};
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }

    std::string in{laxb::cmdhr.getcmdtk()[0].substr(1)}; // :A
    auto inIt{smr::semgr.finddmtx(in)};
    if (inIt == smr::semgr.getdmtxEnd()) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Matrix \"" + in + "\"not found"
        );
        return false;
    }

    double oup{inIt->second.getMean()};

    if (lc::type_g == 2) { // :A ::a
        smr::semgr.addreal(laxb::cmdhr.getcmdtk()[1].substr(2), oup);
    }

    laxb::cmdhr.outDetermine(oup);
    return true;
}

/* 返回矩阵/向量所有元素最大值
 * - 1.max :A
 * - 2.max :A ::a */
bool max() {
    laxb::tvv argset{{INID}, {INID, OUTID}};
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }

    std::string in{laxb::cmdhr.getcmdtk()[0].substr(1)}; // :A
    auto inIt{smr::semgr.finddmtx(in)};
    if (inIt == smr::semgr.getdmtxEnd()) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Matrix \"" + in + "\"not found"
        );
        return false;
    }

    double oup{inIt->second.getMax()};

    if (lc::type_g == 2) { // :A ::a
        smr::semgr.addreal(laxb::cmdhr.getcmdtk()[1].substr(2), oup);
    }

    laxb::cmdhr.outDetermine(oup);
    return true;
}

/* 返回矩阵/向量所有元素最小值
 * - 1.min :A
 * - 2.min :A ::a */
bool min() {
    laxb::tvv argset{{INID}, {INID, OUTID}};
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }

    std::string in{laxb::cmdhr.getcmdtk()[0].substr(1)}; // :A
    auto inIt{smr::semgr.finddmtx(in)};
    if (inIt == smr::semgr.getdmtxEnd()) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Matrix \"" + in + "\"not found"
        );
        return false;
    }

    double oup{inIt->second.getMin()};

    if (lc::type_g == 2) { // :A ::a
        smr::semgr.addreal(laxb::cmdhr.getcmdtk()[1].substr(2), oup);
    }

    laxb::cmdhr.outDetermine(oup);
    return true;
}

/* ==== 矩阵创建与操作 ==== */
/* - eye {num}
 * - eye ::A {num} */
bool eye() {
    laxb::tvv argset{
        {CB},
        {CB, OUTID}
    };
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }

    core::dmtx output{};
    std::vector<double> dimens = laxb::cmdhr.curlyToken(laxb::cmdhr.getcmdtk()[0]);
    if (dimens.size() != 1) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Argument(s) error in \"" + laxb::cmdhr.getcmdtk()[0] + "\""
        );
        return false;
    }
    if (static_cast<int>(dimens[0]) == 0) { // 传递 (-1, 1) 的分数，导致此处判断为非 0，进而构造出空矩阵，在 << 处 .fail() 被置位，报错
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": 0 matrix is illegal: \"" + laxb::cmdhr.getcmdtk()[0] + "\""
        );
        return false;
    }

    output = core::dmtx(dimens[0]); // 制造矩阵
    for (core::dmtx::mtxSizet i = 0; i < output.getRowSize(); ++i) {
        output(i, i) = 1;
    }

    if (lc::type_g == 2) {
        smr::semgr.adddmtx( // ::A
            laxb::cmdhr.getcmdtk()[1].substr(2),
            output
        );
    }

    laxb::cmdhr.outDetermine(output);
    return true;
}

/* 全零矩阵
 * - 1.zeros {m, n}
 * - 2.zeros {m, n} ::A */
bool zeros() {
    laxb::tvv argset{{CB}, {CB, OUTID}};
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }

    std::vector<double> dim = laxb::cmdhr.curlyToken(laxb::cmdhr.getcmdtk()[0]); // {m, n}
    if (dim.size() != 2) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Argument(s) error in \"" + laxb::cmdhr.getcmdtk()[0] + "\""
        );
        return false;
    }
    if (dim[0] <= 0 || dim[1] <= 0) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Argument(s) error in \"" + laxb::cmdhr.getcmdtk()[0] + "\" are too small"
        );
        return false;
    }

    core::dmtx oup(dim[0], dim[1]);
    
    if (lc::type_g == 2) {
        smr::semgr.adddmtx( // ::A
            laxb::cmdhr.getcmdtk()[1].substr(2),
            oup
        );
    }

    laxb::cmdhr.outDetermine(oup);
    return true;
}

/* 全一矩阵
 * - 1.ones {m, n}
 * - 2.ones {m, n} ::A */
bool ones() {
    laxb::tvv argset{{CB}, {CB, OUTID}};
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }

    std::vector<double> dim = laxb::cmdhr.curlyToken(laxb::cmdhr.getcmdtk()[0]); // {d}
    if (dim.size() != 2) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Argument(s) error in \"" + laxb::cmdhr.getcmdtk()[0] + "\""
        );
        return false;
    }
    if (dim[0] <= 0 || dim[1] <= 0) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Argument(s) error in \"" + laxb::cmdhr.getcmdtk()[0] + "\" are too small"
        );
        return false;
    }

    core::dmtx oup(dim[0], dim[1]);
    for (std::size_t r{0}; r < oup.getRowSize(); ++r) {
        for (std::size_t c{0}; c < oup.getColSize(); ++c) {
            oup(r, c) = 1;
        }
    }
    
    if (lc::type_g == 2) { // {d} ::A
        smr::semgr.adddmtx( // ::A
            laxb::cmdhr.getcmdtk()[1].substr(2),
            oup
        );
    }

    laxb::cmdhr.outDetermine(oup);
    return true;
}

/* 获取方阵对角线元素
 * - 1.diag -m/a :A
 * - 2.diag -m/a :A ::B */
bool diag() {
    laxb::tvv argset{{OPT, INID}, {OPT, INID, OUTID}};
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }

    std::string in = laxb::cmdhr.getcmdtk()[1].substr(1); // -m/s :A
    auto inIt = smr::semgr.finddmtx(in);
    if (inIt == smr::semgr.getdmtxEnd()) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Matrix \"" + in + "\"not found"
        );
        return false;
    }
    if (!(inIt->second.isSquare())) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Matrix \"" + in + "\" is not a square matrix"
        );
        return false;
    }

    core::dmtx oup{};
    if (laxb::cmdhr.getcmdtk()[0] == "-m") {
        oup = core::tomtx((inIt->second).getMainDiag());
    }
    else if (laxb::cmdhr.getcmdtk()[0] == "-a") {
        oup = core::tomtx((inIt->second).getAntiDiag());
    }
    else {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            "Option \"" + laxb::cmdhr.getcmdtk()[0].substr(1) + "\" error"
        );
        return false;
    }

    if (lc::type_g == 2) { // -m/a :A ::B
        smr::semgr.adddmtx(laxb::cmdhr.getcmdtk()[2].substr(2), oup);
    }

    laxb::cmdhr.outDetermine(oup);
    return true;
}

/* 水平连接两个矩阵
 * 1.horzcat :A :B
 * 2.horzcat :A :B ::C */
bool horzcat() {
    laxb::tvv argset{{INID, INID}, {INID, INID, OUTID}};
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }

    std::string in1{laxb::cmdhr.getcmdtk()[0].substr(1)};
    std::string in2{laxb::cmdhr.getcmdtk()[1].substr(1)};
    auto inIt1{smr::semgr.finddmtx(in1)};
    auto inIt2{smr::semgr.finddmtx(in2)};
    if (inIt1 == smr::semgr.getdmtxEnd() ||
        inIt2 == smr::semgr.getdmtxEnd()
    ) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Matrix \"" + in1 + "\" or \"" + in2 + "\" not found"
        );
        return false;
    }

    if (inIt1->second.getRowSize() != inIt2->second.getRowSize()) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            "Demension not match in \"" + in1 + "\" and \"" + in2 + "\""
        );
        return false;
    }

    core::dmtx oup = util::Factory::catCol(inIt1->second, inIt2->second);

    if (lc::type_g == 2) {
        smr::semgr.adddmtx(laxb::cmdhr.getcmdtk()[2], oup);
    }

    laxb::cmdhr.outDetermine(oup);
    return true;
}

/* 垂直连接两个矩阵
 * 1.vertcat :A :B
 * 2.vertcat :A :B ::C */
bool vertcat() {
    laxb::tvv argset{{INID, INID}, {INID, INID, OUTID}};
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }

    std::string in1{laxb::cmdhr.getcmdtk()[0].substr(1)};
    std::string in2{laxb::cmdhr.getcmdtk()[1].substr(1)};
    auto inIt1{smr::semgr.finddmtx(in1)};
    auto inIt2{smr::semgr.finddmtx(in2)};
    if (inIt1 == smr::semgr.getdmtxEnd() ||
        inIt2 == smr::semgr.getdmtxEnd()
    ) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Matrix \"" + in1 + "\" or \"" + in2 + "\" not found"
        );
        return false;
    }

    if (inIt1->second.getColSize() != inIt2->second.getColSize()) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            "Demansion not match in \"" + in1 + "\" and \"" + in2 + "\""
        );
        return false;
    }

    core::dmtx oup = util::Factory::catRow(inIt1->second, inIt2->second);

    if (lc::type_g == 2) {
        smr::semgr.adddmtx(laxb::cmdhr.getcmdtk()[2], oup);
    }

    laxb::cmdhr.outDetermine(oup);
    return true;
}

/* ==== 矩阵基本信息 ==== */
/* 输出矩阵/向量维度
 * - size :A */
bool size() {
    laxb::tvv argset{{INID}};
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }

    std::string in{laxb::cmdhr.getcmdtk()[0].substr(1)}; // :A
    auto inIt{smr::semgr.finddmtx(in)};
    if (inIt == smr::semgr.getdmtxEnd()) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Matrix \"" + in + "\"not found"
        );
        return false;
    }

    std::string oup{
        std::to_string(inIt->second.getRowSize()) + " " +
        std::to_string(inIt->second.getColSize())
    };

    laxb::cmdhr.outDetermine(oup);
    return true;
}

/* 判断两个矩阵是否相等
 * - isequal :A :B */
bool isequal() {
    laxb::tvv argset{{INID, INID}};
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }

    std::string in1{laxb::cmdhr.getcmdtk()[0].substr(1)};
    std::string in2{laxb::cmdhr.getcmdtk()[1].substr(1)};
    auto inIt1{smr::semgr.finddmtx(in1)};
    auto inIt2{smr::semgr.finddmtx(in2)};
    if (inIt1 == smr::semgr.getdmtxEnd() ||
        inIt2 == smr::semgr.getdmtxEnd()
    ) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Matrix \"" + in1 + "\" or \"" + in2 + "\" not found"
        );
        return false;
    }

    std::string oup = (inIt1->second) == (inIt2->second)
                          ? "true"
                          : "false";

    laxb::cmdhr.outDetermine(oup);
    return true;
}

/* ==== 矩阵分解 ==== */
/* - lu :A（默认输出 L、U）
 * - lu :A ::L ::U
 * - lu :A -a（输出 L、U、P）
 * - lu :A ::L ::U ::P */
bool lu() {
    laxb::tvv argset{
        {INID},
        {INID, OUTID, OUTID},
        {INID, OPT},
        {INID, OUTID, OUTID, OUTID}
    };
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }

    std::string varnIn(laxb::cmdhr.getcmdtk()[0].substr(1)); // :A
    auto it = smr::semgr.finddmtx(varnIn);
    if (it == smr::semgr.getdmtxEnd()) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Matrix \"" + varnIn + "\"not found"
        );
        return false;
    }
    if (!(it->second.isSquare())) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Matrix \"" + varnIn + "\" is not a square matrix"
        );
        return false;
    }

    decomp::BaseDecomposer decomper(it->second); // 初始化分解器
    std::vector<core::dmtx> LUP = decomper.lu(); // 分解

    if (lc::type_g == 3) {
        if (laxb::cmdhr.getcmdtk()[1] == "-a") { // "-a"
            true; // 待拓展
        }
        else {
            smr::semgr.seterr(
                laxb::cmdhr.getname() +
                ": There is no option "
                "\"" + laxb::cmdhr.getcmdtk()[1].substr(1) + "\""
            );
            return false;
        }
    }

    if (lc::type_g == 2 || lc::type_g == 4) {
        smr::semgr.adddmtx(laxb::cmdhr.getcmdtk()[1].substr(2), LUP[0]); // L
        smr::semgr.adddmtx(laxb::cmdhr.getcmdtk()[2].substr(2), LUP[1]); // U
        if (lc::type_g == 4) {
            smr::semgr.adddmtx( // P
                laxb::cmdhr.getcmdtk()[3].substr(2),
                LUP[2]
            );
        }
    }

    if (laxb::cmdhr.getoutbit() == true) {
        smr::semgr << LUP[0] << LUP[1]; // L U
        if (lc::type_g == 3 || lc::type_g == 4) {
            smr::semgr << LUP[2]; // P
        }
    }

    return true;
}

/* QR分解
 * - 1.qr :A
 * - 2.qr :A ::Q ::R */
bool qr() {
    laxb::tvv argset{{INID}, {INID, OUTID, OUTID}};
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }

    std::string in{laxb::cmdhr.getcmdtk()[0].substr(1)}; // :A
    auto inIt{smr::semgr.finddmtx(in)};
    if (inIt == smr::semgr.getdmtxEnd()) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Matrix \"" + in + "\"not found"
        );
        return false;
    }

    decomp::AdvancedDecomposer decomper(inIt->second);
    std::pair<core::dmtx, core::dmtx> oup = decomper.householderQR();

    if (laxb::cmdhr.getoutbit() == true) {
        smr::semgr << oup.first << oup.second; // Q R
    }

    if (lc::type_g == 2) {
        smr::semgr.adddmtx(laxb::cmdhr.getcmdtk()[1].substr(2), oup.first);
        smr::semgr.adddmtx(laxb::cmdhr.getcmdtk()[2].substr(2), oup.second);
    }

    return true;
}

/* ==== 线性方程组求解 ==== */
/* 矩阵求逆
 * - inv :A
 * - inv :A ::~A */
bool inv() {
    laxb::tvv argset{
        {INID},
        {INID, OUTID}
    };
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }
    
    std::string varnIn(laxb::cmdhr.getcmdtk()[0].substr(1)); // :A
    auto it{smr::semgr.finddmtx(varnIn)};
    if (it == smr::semgr.getdmtxEnd()) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Matrix \"" + varnIn + "\"not found"
        );
        return false;
    }
    if (!(it->second.isSquare())) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Matrix \"" + varnIn + "\" is not a square matrix"
        );
        return false;
    }
    
    decomp::BaseDecomposer decomper{it->second};
    core::dmtx output{decomper.inv()};
    
    if (output.isEmpty()) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Matrix \"" + varnIn + "\" is singular matrix"
        );
        return false;
    }
    
    if (lc::type_g == 2) { // :A ::_A
        smr::semgr.adddmtx(
            laxb::cmdhr.getcmdtk()[1].substr(2),
            output
        );
    }
    
    laxb::cmdhr.outDetermine(output);
    
    return true;
}

/* 求解线性方程组 Ax = b，b 可以为 0 向量
* - 第一参数必须为系数矩阵，第二参数必须为向量 b
* - 1.linsolve :A :b ::x 默认输出一个特解
* - 2.linsolve :A :b
* - 3.linsolve :A :b ::X -a 输出全解，X 为 [xp xn] 形式的矩阵
* - 4.linsolve :A :b -a */
bool linsolve() {
    laxb::tvv argset{
        {INID, INID,OUTID},
        {INID, INID},
        {INID, INID, OUTID, OPT},
        {INID, INID, OPT}
    };
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }
    
    std::string inA(laxb::cmdhr.getcmdtk()[0].substr(1)); // :A
    std::string inb(laxb::cmdhr.getcmdtk()[1].substr(1)); // :b
    
    auto itA{smr::semgr.finddmtx(inA)}, itb{smr::semgr.finddmtx(inb)};
    if (itA == smr::semgr.getdmtxEnd() ||
        itb == smr::semgr.getdmtxEnd()
    ) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Matrix \"" + inA + "\" or \"" + inb + "\" not found"
        );
        return false;
    }
    
    /* 维度判断 */
    if (itb->second.getColSize() != 1 ||
        itb->second.getRowSize() != itA->second.getColSize()
    ) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Vector \"" + inb + "\" "
            "does not match the coefficient dimension of matrix "
            "\"" + inA + "\""
        );
        return false;
    }
        
    solve::Solver solver((*itA).second); // 初始化求解器
    core::dmtx oup(solver.solveAxb(core::tovec(itb->second))); // 全解 [xp xn]

    if (oup.isEmpty()) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Linear equations have no solution"
        );
        return false;
    }
        
    /* 判断输出特解还是全解 */
    std::string opt{};
    if (lc::type_g == 3) {
        opt = laxb::cmdhr.getcmdtk()[3];
        if (opt == "-a") { // :A :b ::X -a
            // 待定
        }
        else {
            smr::semgr.seterr(
                laxb::cmdhr.getname() +
                ": Unknow option \"" + opt + "\""
            );
            return false;
        }
    }
    else if (lc::type_g == 4) {
        opt = laxb::cmdhr.getcmdtk()[2];
        if (opt == "-a") { // :A :b -a
            // 待定
        }
        else {
            smr::semgr.seterr(
                laxb::cmdhr.getname() +
                ": Unknow option \"" + opt + "\""
            );
            return false;
        }
    }
    else {
        oup = util::Factory::splitCol(oup, 1).first; // 分裂矩阵，只保留特解 xp 一列
    }
    
    if (lc::type_g == 1 || lc::type_g == 3) { // :A :b ::x, :A :b ::X -a
        std::string oupName = laxb::cmdhr.getcmdtk()[2].substr(2); // ::x
        smr::semgr.adddmtx(oupName, oup); // 添加解至变量空间
    }
    
    laxb::cmdhr.outDetermine(oup);
    
    return true;
}

/* 化为行最简型
 * - rref :A
 * - rref :A ::R */
bool rref() {
    laxb::tvv argset{
        {INID},
        {INID, OUTID}
    };
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }

    std::string varnIn(laxb::cmdhr.getcmdtk()[0].substr(1)); // :A
    auto it{smr::semgr.finddmtx(varnIn)};
    if (it == smr::semgr.getdmtxEnd()) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Matrix \"" + varnIn + "\"not found"
        );
        return false;
    }

    decomp::BaseDecomposer decomper(it->second); // 初始化分解器
    core::dmtx output{decomper.rref()};

    if (output.isEmpty()) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Simplification row simplest failure"
        );
        return false;
    }

    if (lc::type_g == 2) { // 写入矩阵变量空间
        smr::semgr.adddmtx( // :A ::R
            laxb::cmdhr.getcmdtk()[1].substr(2),
            output
        );
    }

    laxb::cmdhr.outDetermine(output);

    return true;
}

/* 输出矩阵零空间的一组正交基
 * - 1.null :A
 * - 2.null :A ::N */
bool null() {
    laxb::tvv argset{{INID}, {INID, OUTID}};
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }

    std::string in{laxb::cmdhr.getcmdtk()[0].substr(1)}; // :A
    auto inIt{smr::semgr.finddmtx(in)};
    if (inIt == smr::semgr.getdmtxEnd()) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Matrix \"" + in + "\"not found"
        );
        return false;
    }

    subsp::SubSpace spcer(inIt->second);
    core::dmtx oup{spcer.nullBase()};

    if (lc::type_g == 2) {
        smr::semgr.adddmtx(laxb::cmdhr.getcmdtk()[1].substr(2), oup);
    }

    laxb::cmdhr.outDetermine(oup);
    return true;
}

/* 输出矩阵列空间的一组正交基
 * - 1.orth :A
 * - 2.orth :A ::C */
bool orth() {
    laxb::tvv argset{{INID}, {INID, OUTID}};
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }

    std::string in{laxb::cmdhr.getcmdtk()[0].substr(1)}; // :A
    auto inIt{smr::semgr.finddmtx(in)};
    if (inIt == smr::semgr.getdmtxEnd()) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Matrix \"" + in + "\"not found"
        );
        return false;
    }

    subsp::SubSpace spcer(inIt->second);
    core::dmtx oup{spcer.colBase()};

    if (lc::type_g == 2) {
        smr::semgr.adddmtx(laxb::cmdhr.getcmdtk()[1].substr(2), oup);
    }

    laxb::cmdhr.outDetermine(oup);
    return true;
}

/* 输出最小二乘全解
 * - 1.lsq :A :b
 * - 2.lsq :A :b ::Np */
bool lsq() {
    laxb::tvv argset{
        {INID, INID},
        {INID, INID, OUTID}
    };
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }

    std::string in1{laxb::cmdhr.getcmdtk()[0].substr(1)};
    std::string in2{laxb::cmdhr.getcmdtk()[1].substr(1)};
    auto inIt1{smr::semgr.finddmtx(in1)};
    auto inIt2{smr::semgr.finddmtx(in2)};

    if (inIt1 == smr::semgr.getdmtxEnd() ||
        inIt2 == smr::semgr.getdmtxEnd()
    ) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Matrix \"" + in1 + "\" or \"" + in2 + "\" not found"
        );
        return false;
    }
    if (inIt1->second.getRowSize() != inIt2->second.getRowSize()) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            "Demension not match in \"" + in1 + "\" and \"" + in2 + "\""
        );
        return false;
    }

    solve::LeastSquaresSolver solver(inIt1->second);
    core::dmtx oup = solver.solveAxhb(inIt2->second);

    if (lc::type_g == 2) {
        smr::semgr.adddmtx(laxb::cmdhr.getcmdtk()[2].substr(2), oup);
    }

    laxb::cmdhr.outDetermine(oup);
    return true;
}

/* ==== 矩阵分析与条件数 ==== */
/* - 1.det :A
 * - 2.det :A ::val */
bool det() {
    laxb::tvv argset{
        {INID},
        {INID, OUTID}
    };
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }

    std::string varnIn(laxb::cmdhr.getcmdtk()[0].substr(1)); // :A
    auto it{smr::semgr.finddmtx(varnIn)};
    if (it == smr::semgr.getdmtxEnd()) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Matrix \"" + varnIn + "\"not found"
        );
        return false;
    }
    if (!(it->second.isSquare())) { // 非方阵时
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Matrix \"" + varnIn + "\" is not a square matrix"
        );
        return false;
    }

    decomp::BaseDecomposer decomper{it->second};
    double output{decomper.det()};

    if (lc::type_g == 2) { // det :A ::val
        smr::semgr.addreal(
            laxb::cmdhr.getcmdtk()[1].substr(2),
            output
        );
    }

    laxb::cmdhr.outDetermine(output);
    return true;
}

/* 矩阵的秩
 * - 1.rank :A
 * - 2.rank :A ::rank */
bool rank() {
    laxb::tvv argset{{INID}, {INID, OUTID}};
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }

    std::string in{laxb::cmdhr.getcmdtk()[0].substr(1)};
    auto it{smr::semgr.finddmtx(in)};
    if (it == smr::semgr.getdmtxEnd()) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Matrix \"" + in + "\"not found"
        );
        return false;
    }

    decomp::BaseDecomposer decomper(it->second);
    double oup{decomper.getrank()};

    if (lc::type_g == 2) {
        smr::semgr.addreal(laxb::cmdhr.getcmdtk()[1].substr(2), oup);
    }

    laxb::cmdhr.outDetermine(oup);
    return true;
}

/* 矩阵的迹
 * 1.trace :A
 * 2.trace :A ::tr */
bool trace() {
    laxb::tvv argset{{INID}, {INID, OUTID}};
    lc::type_g = laxb::cmdhr.argHandler(argset);
    if (!lc::type_g) {
        return false;
    }

    std::string in{laxb::cmdhr.getcmdtk()[0].substr(1)};
    auto it{smr::semgr.finddmtx(in)};
    if (it == smr::semgr.getdmtxEnd()) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Matrix \"" + in + "\"not found"
        );
        return false;
    }
    if (!(it->second.isSquare())) {
        smr::semgr.seterr(
            laxb::cmdhr.getname() +
            ": Matrix \"" + in + "\" is not a square matrix"
        );
        return false;
    }

    double oup{it->second.getTrace()};
    
    if (lc::type_g == 2) {
        smr::semgr.addreal(laxb::cmdhr.getcmdtk()[1].substr(2), oup);
    }

    laxb::cmdhr.outDetermine(oup);
    return true;
}
