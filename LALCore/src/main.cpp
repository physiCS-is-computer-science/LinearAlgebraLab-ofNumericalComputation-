// ============================================================================
// 此文件为 LAL 程序主循环
// - 循环流程为：
//    0.注册所有函数
//    1.由 CmdParser 类的唯一对象 cmdpr 读取命令字符串并解析
//    2.cmdpr 调用 LALCore 的接口模块并由 LALCore 计算
//    3.LALCore 的接口模块使用 SessionMgr 类的唯一对象 semgr 对工作空间写入计算结果、额外信息
//    4.cmdpr 访问 semgr，判断输出内容、是否向 LaxbIO 发出命令
//    5.循环结束，重新等待新命令
// - 三大模块（LALCore 单向接收 LaxbIO 的消息（只返回执行是否成功的布尔值），二者通过 SessionMgr 间接联系）：
//    ================================================
//    LALCore ---> SessionMgr ---> LaxbIO <---> user
//      ^-----------------------------^
//    ================================================
//    - LALCore 作为计算核心，只负责计算，并附带接受命令、输出数据与状态功能
//    - SessionMgr 作为会话管理器，储存所有数据、状态于工作空间，并与其他两个模块沟通
//    - LaxbIO 处理输入、输出，与用户交互
// ============================================================================

#include "commandIO.hpp"
#include "core/matrix.hpp" // test
#include "core/vector.hpp" // test
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

    // core::dmtx mtx{{1, 2, 3}};

    /* LaxbIO module test */
    while (true) {
        smr::semgr.clear();

        /* input */
        std::cout << util::SIS(smr::semgr.getpath()); // default path identifier
        while (!laxb::cmdhr.in()) {
            std::cout << util::SIS(smr::semgr.getpath());
        }
        
        // /* check current tokens */
        // std::cout << "==== TEST ====\ntokens:\n";
        // for (const auto& i : cmdStr)
        //     std::cout << i << "|";
        // std::cout << std::endl
        //           << "==== TEST ====\n";

        /* 尝试调用 LALCore */
        laxb::cmdhr.setName(laxb::cmdhr.getCmdtoken()[0]);
        laxb::cmdhr.getCmdtoken().erase(laxb::cmdhr.getCmdtoken().begin()); // 删掉命令名
        bool state = laxb::fnmgr.call(laxb::cmdhr.getName())(); // call

        if (smr::semgr.poweroff()) {
            return 0;
        }

        if (state == true && !smr::semgr.getoup().empty()) { // 执行成功并且存在输出时（不存在时 cptoup_ 为空串）
            std::cout << util::SOS(smr::semgr.getpath()) + smr::semgr.getoup() + "\n";
        }
        else if (state == false) {
            std::cout << util::ERS(smr::semgr.getpath()) + smr::semgr.geterr() + "\n";
        }
    }

    return 0;
}