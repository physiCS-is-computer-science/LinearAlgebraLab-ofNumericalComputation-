// ============================================================================
// 此文件为 LAL 程序主循环
// - 循环流程为：
//    0.注册所有函数
//    1.由 CmdParser 类的唯一对象 cmdpr 读取命令字符串并解析
//    2.cmdpr 调用 LALCore 的接口模块并由 LALCore 计算
//    3.LALCore 的接口模块使用 SessionMgr 类的唯一对象 semgr 对工作空间写入计算结果、额外信息
//    4.cmdpr 访问 semgr，判断输出内容、是否向 LaxbIO 发出命令
//    5.循环结束，重新等待新命令
// - 三大模块（LALCore 单向接收 LaxbIO 的消息，二者通过 SessionMgr 间接联系）：
//    ================================================
//    LALCore <---> SessionMgr <---> LaxbIO <---> user
//      ^-------------------------------|
//    ================================================
//    - LALCore 作为计算核心，只负责计算，并附带接受命令、输出数据与状态功能
//    - SessionMgr 作为会话管理器，储存所有数据于工作空间，并于其他两个模块沟通
//    - LaxbIO 处理输入、输出，与用户交互
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
    core::dmtx
        mtx1{{1, 2, 3},
             {1, 2, 3}},
        mtx2{{1, 2},
             {1, 2},
             {1, 2}},
        mtx3{{1, 2, 3, 4}};

    std::cout << core::tovec(mtx3) + ~vec1;

    /* LaxbIO 模块测试 */
    // std::vector<std::string> ret;
    // std::cout << oup::SIS();
    // while ((ret = cmdpr.in()).empty())
    //     std::cout << oup::SIS();
    // for (const auto& i : ret)
    //     std::cout << i << "|";

    return 0;
}