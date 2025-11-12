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

/* ==== 基本命令 ==== */
bool quit();
bool show();
bool var();
bool del();
bool save();
bool load();
bool cls();
bool help();

/* ==== 运算 ==== */
bool plus();
bool minus();
bool mtimes();
bool times();
bool divide();
// bool mpower(); // 设计负数次方（矩阵求逆），因此暂时不实现
bool power();

/* ==== 矩阵创建与操作 ==== */
bool eye();
// bool zeros();
// bool ones();
// bool random();
// bool randn();
// bool diag();

/* ==== 矩阵分解 ==== */
bool lu();
bool rref();

/* ==== 线性方程组求解 ==== */
bool inv();

/* ==== 矩阵分析与条件数 ==== */
bool det();
