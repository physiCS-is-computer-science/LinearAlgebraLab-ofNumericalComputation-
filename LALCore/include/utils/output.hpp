// ========================================
// 杂七杂八的输出部分
// ========================================

#pragma once

#include <iostream>
#include <string>

namespace oup {
/* 标注当前使用文件路径，默认为$，使用方式为纯粹的类名调用重载的 ()，其中括号接受一个 std::string 作为路径 */
class OutPathID {
public:
    OutPathID() = default;
    ~OutPathID() = default;
    OutPathID(const OutPathID&) = delete;
    OutPathID(OutPathID&&) = delete;
    OutPathID& operator=(const OutPathID&) = delete;
    OutPathID& operator=(OutPathID&&) = delete;

    OutPathID(const std::string& ioid) : ioid_(ioid) {}

    std::string operator()(const std::string& path = "#") { return "[LAL " + path + "]" + ioid_; } // 传递路径，无参数时默认为 $

private:
    const std::string ioid_; // 初始化后就不可改
};
extern oup::OutPathID ERS;
extern oup::OutPathID SOS;
extern oup::OutPathID SIS;
} // namespace oup

void startupBanner(); // 启动信息栏