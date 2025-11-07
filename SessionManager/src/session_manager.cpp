#include "session_manager.hpp"
#include "commandIO.hpp"
#include <fstream>
#include <iostream> // test

namespace smr {

SessionMgr semgr;

/* ==== 一组重载函数，将本次计算结果转换为 std::string 格式并追加到 cptoup_ ==== */
SessionMgr& SessionMgr::operator<<(double val) { // 实数
    os_ << val;
    cptoup_ = os_.str();
    return *this;
}
SessionMgr& SessionMgr::operator<<(std::string output) { // 字符串
    os_ << output;
    cptoup_ = os_.str();
    return *this;
}
SessionMgr& SessionMgr::operator<<(core::dmtx mtx) { // 实数矩阵
    os_ << mtx;
    cptoup_ = os_.str();
    return *this;
}
SessionMgr& SessionMgr::operator<<(core::dvec vec) { // 实数向量
    os_ << vec;
    cptoup_ = os_.str();
    return *this;
}

/* ==== 操作函数 ==== */
/* 添加一个矩阵变量
 * - 存在同名 矩阵 变量则直接覆盖
 * - 存在同名 实数 变量则直接删除同名实数变量 */
void SessionMgr::adddmtx(std::string varn, core::dmtx mtx) {
    dmSpace_[varn] = mtx;
    if (realSpace_.find(varn) != realSpace_.end()) {
        realSpace_.erase(varn);
    }
}

/* 添加一个实数变量
 * - 存在同名 实数 变量则直接覆盖
 * - 存在同名 矩阵 变量则直接删除同名矩阵变量 */
void SessionMgr::addreal(std::string varn, double val) {
    realSpace_[varn] = val;
    if (dmSpace_.find(varn) != dmSpace_.end()) {
        dmSpace_.erase(varn);
    }
}

/* 写入当前会话至文件
 * - 存在同名文件时清空内容，重新写入 */
bool SessionMgr::wfile(std::string fname) {
    std::string path{this->filePath(fname)};
    std::ofstream fout(path);
    if (fout.fail()) {
        fout.close();
        return false;
    }

    /* 格式化写入所有变量进入文件 fname */
    for (const auto& i : dmSpace_) { // 矩阵
        fout << "\"" + i.first + "\":[\n";
        for (core::dmtx::mtxSizet row = 0; row < i.second.getRowSize(); ++row) {
            core::dvec temp(i.second.getRow(row)); // 临时行
            for (core::dvec::vecSizet i = 0; i < temp.getSize(); ++i) { // 逐个输出临时行的元素
                fout << std::setw(10) << temp(i); // 规定每个数字宽度为 10
            }
            fout << ";\n";
        }
        fout << "],\n";
    }
    for (const auto& i : realSpace_) {
        fout << "\"" + i.first + "\":"
             << i.second << ",\n";
    }

    return true;
}

/* 从文件读取某次会话
 * - 未找到名字为 fname 的文件则返回 false */
bool SessionMgr::rfile(std::string fname) {
    std::string path{this->filePath(fname)};
    std::ifstream fin(path);
    if (fin.fail()) {
        fin.close();
        return false;
    }

    /* 读取所有文本至字符串 */
    std::string temp{""}, originTxt{""};
    while (std::getline(fin, temp)) { // getline() 会丢弃所有换行符
        originTxt += temp;
    }
    if (originTxt.empty()) {
        fin.close();
        return false;
    }
    fin.close();

    laxb::delCh(originTxt, ','); // 去除首尾逗号

    std::vector<std::string> tokens = laxb::splitBych(originTxt, ','); // 得到: "str":[] "str":a "str":[]...

    /* 写入变量空间 */
    for (auto& i : tokens) {
        std::vector<std::string> keyVal = laxb::splitBych(i, ':');
        laxb::delCh(keyVal[0], ' '); // 去除空格
        laxb::delCh(keyVal[0], '"'); // 去除引号

        if (keyVal[1].find('[') != std::string::npos) { // 矩阵时
            core::dmtx input = laxb::cmdhr.squareToken(keyVal[1]);
            if (input.isEmpty()) {
                return false;
            }

            smr::semgr.adddmtx(keyVal[0], input);
        }
        else {
            std::vector<double> input = laxb::cmdhr.curlyToken("{" + keyVal[1] + "}");
            if (input.size() != 1) {
                return false;
            }

            smr::semgr.addreal(keyVal[0], input[0]);
        }
    }

    return true;
}

} // namespace smr