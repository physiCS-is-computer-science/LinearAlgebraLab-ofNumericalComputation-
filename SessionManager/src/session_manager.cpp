#include "session_manager.hpp"
#include <fstream>

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

// bool wfile(std::string fname) {
// }

// bool rfile(std::string fname) {
//     std::ifstream fin(fname);
//     if (fin.fail()) {
//         return false;
//     }


// }

} // namespace smr