// ============================================
// 会话管理器，存储模块之间沟通的消息、计算数据，沟通
// ============================================

#pragma once

#include "core/matrix.hpp"
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace smr {

/* ==== 会话管理器 ==== */
class SessionMgr {
public:
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
    SessionMgr& operator<<(double val) { // 实数
        os_ << val;
        cptoup_ = os_.str();
        return *this;
    }
    SessionMgr& operator<<(std::string output) { // 字符串
        os_ << output;
        cptoup_ = os_.str();
        return *this;
    }
    SessionMgr& operator<<(core::dmtx mtx) { // 实数矩阵
        os_ << mtx;
        cptoup_ = os_.str();
        return *this;
    }
    SessionMgr& operator<<(core::dvec vec) { // 实数向量
        os_ << vec;
        cptoup_ = os_.str();
        return *this;
    }

    /* ==== 操控 VarStorage<> 的函数 ==== */
    /* get 系列函数返回迭代器，add 系列函数直接存储，不检查 */
    typename std::unordered_map<std::string, core::dmtx>::const_iterator getdmtx(std::string varn) const { return dmSpace_.find(varn); }; // 如果没有该函数返回空 map
    // typename std::unordered_map<std::string, core::dmtx>::const_iterator getcmtx(std::string varn) const { return cmSpace_.find(varn); }; // 复数版本，同上
    std::unordered_map<std::string, double>::const_iterator getreal(std::string varn) const { return realSpace_.find(varn); };
    void adddmtx(std::string varn, core::dmtx& mtx) { dmSpace_[varn] = mtx; }; // 无检查
    // void addcmtx(std::string varn, core::cmtx& mtx) { cmSpace_[varn] = mtx; }; // 复数重载版本
    void addreal(std::string varn, double val) { realSpace_[varn] = val; }; // 无检查

    void clear() { // 只清除错误信息、计算结果、字符流对象的内容
        err_ = "";
        cptoup_ = "";
        os_.str("");
    }

private:
    std::string path_;
    std::string err_;
    std::string cptoup_; // 所有计算结果输出转化为字符串存储
    std::ostringstream os_; // 定向输出流到 cptoup_

    std::unordered_map<std::string, core::dmtx> dmSpace_; // 实矩阵变量，向量也用矩阵存储
    // std::unordered_map<std::string, core::cmtx> cmSpace_; // 复矩阵变量，向量也用矩阵存储
    std::unordered_map<std::string, double> realSpace_; // 普通变量
};

extern SessionMgr semgr; // 不可复制，仅仅作为接口

} // namespace smr