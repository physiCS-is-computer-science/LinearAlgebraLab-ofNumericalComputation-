// =============================================================================================
// 此头文件包含的函数为 LALCore 能做的所有事情（即为LALCore 对 LaxbIO 与 SessionMgr 的接口）
// - 所有函数拥有统一风格的函数签名，除名字不同外其余都完全相同
// - 函数格式为：bool funcName()
// - 每个函数都使用全局变量对象 semgr、cmdpr 与 SessionMgr、CommandParser 交流
// =============================================================================================

#pragma once

#include <string>
#include <vector>

/* 此类负责处理核心与其他模块的数据沟通
 * - 如每个函数拥有的第二参数为一个 vector，存储着数量不同含义不一致的字符串，因而通过此类解析 */
// class CoreIntf {
// public:
// };

/* ==== 交互 ==== */
bool quit();
bool show();
bool var();
// bool modify();
bool del();
// bool save();
// bool load();
bool cls();
// bool help();

/* ==== 数学 ==== */
bool plus();
// bool minus();
// bool mtimes();
// bool times();
// bool divide();
// bool mpower();
// bool power();
bool eye();
// bool zeros();
// bool ones();
// bool random();
// bool randn();
// bool diag();
