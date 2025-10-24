// ============================================================================
// 此文件为 LAL 程序主循环
// - 循环流程为：
//   0.注册所有函数
//   1.由 CmdParser 类的唯一对象 cmdpr 读取命令字符串并解析
//   2.cmdpr 调用 LALCore 的接口模块计算
//   3.LALCore 的接口模块使用 Workspace 类的唯一对象 ws 对工作空间写入计算结果
//   4.cmdpr 检查 ws 的标记，判断是否需要输出本次计算的结果
//   5.循环结束，重新等待新命令
// ============================================================================

#include "commandIO.hpp"
#include "core/matrix.hpp"
#include "core/vector.hpp"
#include "function_manager.hpp"
#include "utils/output.hpp"
#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>

int main() {
    system("cls");

    laxb::registerAllFunc(); // 注册所有函数
    util::startupBanner(); // 启动信息栏

    core::dvec vec1{0, 1, 2, 3}, vec2{1, 2, 5, 7};
    std::cout << core::dot(vec1, ~(vec2 + vec1 * 4));

    /* LaxbIO 模块测试 */
    // std::vector<std::string> ret;
    // std::cout << oup::SIS();
    // while ((ret = cmdpr.in()).empty())
    //     std::cout << oup::SIS();
    // for (const auto& i : ret)
    //     std::cout << i << "|";

    return 0;
}