// ============================================
// 会话管理器，存储模块之间沟通的消息、计算数据，沟通
// ============================================

#pragma once

#include "core/matrix.hpp"
#include "utils/interface.hpp"
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace smr {

/* ==== 会话管理器 ==== */
class SessionMgr {
private:
    bool quit_{0};

    std::string path_;
    std::string err_;
    std::string cptoup_; // 所有计算结果输出转化为字符串存储
    std::ostringstream os_; // 定向输出流到 cptoup_

    std::unordered_map<std::string, core::dmtx> dmSpace_; // 实矩阵变量，向量也用矩阵存储
    // std::unordered_map<std::string, core::cmtx> cmSpace_; // 复矩阵变量，向量也用矩阵存储
    std::unordered_map<std::string, double> realSpace_; // 普通变量
public:
    /* friends */
    friend bool ::show();
    friend bool ::del();

    SessionMgr() : path_("#") {};
    ~SessionMgr() = default;
    SessionMgr(const SessionMgr&) = delete;
    SessionMgr(SessionMgr&&) = delete;
    SessionMgr& operator=(const SessionMgr&) = delete;
    SessionMgr& operator=(SessionMgr&&) = delete;

    std::string getpath() { return path_; }
    std::string geterr() { return err_; }
    std::string getoup() { return cptoup_; }
    void setpath(std::string path) { path_ = path; }
    void seterr(std::string errStr) { err_ = errStr; }

    /* ==== 一组重载函数，将本次计算结果转换为 std::string 格式并追加到 cptoup_ ==== */
    SessionMgr& operator<<(double val); // 实数
    SessionMgr& operator<<(std::string output); // 字符串
    SessionMgr& operator<<(core::dmtx mtx); // 实数矩阵
    SessionMgr& operator<<(core::dvec vec); // 实数向量

    /* ==== 操作函数 ==== */
    typename std::unordered_map<std::string, core::dmtx>::iterator finddmtx(const std::string& varn) { return dmSpace_.find(varn); }; // 如果没有该函数返回空 map
    // typename std::unordered_map<std::string, core::dmtx>::iterator findcmtx(std::string varn) { return cmSpace_.find(varn); }; // 复数版本，同上
    std::unordered_map<std::string, double>::iterator findreal(const std::string& varn) { return realSpace_.find(varn); };
    void adddmtx(std::string varn, core::dmtx mtx);
    // void addcmtx(std::string varn, core::cmtx mtx); // 复数版本
    void addreal(std::string varn, double val);
    std::unordered_map<std::string, core::dmtx>::const_iterator getdmtxEnd() { return dmSpace_.end(); } // 变量空间尾后迭代器
    // std::unordered_map<std::string, core::cmtx>::const_iterator getcmtxEnd() { return cmSpace_.end(); } // 复数版本
    std::unordered_map<std::string, double>::const_iterator getrealEnd() { return realSpace_.end(); }
    void deldmtx(decltype(dmSpace_.end()) dmtxIt) { dmSpace_.erase(dmtxIt); } // 删除该变量
    // void delcmtx(decltype())
    void delreal(decltype(realSpace_.end()) realIt) { realSpace_.erase(realIt); }

    // bool wfile(std::string fname);
    // bool rfile(std::string fname);

    void clear() { // 只清除错误信息、计算结果、字符流对象的内容
        err_ = "";
        cptoup_ = "";
        os_.str("");
    }

    void swich() { quit_ = true; } // 关闭程序（swich 是错误拼写，因为 switch 是关键字用不了...）
    bool poweroff() { return quit_; } // 返回状态判断是否结束
};
extern SessionMgr semgr; // 不可复制，仅仅作为接口

} // namespace smr