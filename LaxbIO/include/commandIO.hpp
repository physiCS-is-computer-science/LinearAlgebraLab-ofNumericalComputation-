// ======================================================
// 处理输入与输出
// - 字符串解析，解析成功后调用 LALCore 接口计算
// - 需要输出时调用 Workspace，查找输出的东西放在哪
// ======================================================

#include "core/matrix.hpp"
#include <string>
#include <vector>

namespace laxb {

/* 五种命令标识符 */
enum class Cmdt {
    OPT, // -
    IN, // :
    OUT, // ::
    CB, // []
    SB, // {}
    NOTF // 未找到
};

/* 命令解析器 */
class CmdHandler {
public:
    CmdHandler() = default;
    ~CmdHandler() = default;
    CmdHandler(const CmdHandler&) = delete;
    CmdHandler(CmdHandler&&) = delete;
    CmdHandler& operator=(const CmdHandler&) = delete;
    CmdHandler& operator=(CmdHandler&&) = delete;

    bool in(); // 处理输入，错误则返回空 vector，正确则返回语法正确的 tokens

    // void clear() {
    //     name_ = {};
    //     cmdStr_ = {};
    // }

    /* 一组设置读取成员的函数 */
    std::vector<std::string>& getCmdtoken() { return cmdToken_; } // 获取命令字符串
    std::string& getName() { return name_; } // 命令名
    void setCmdtoken(std::vector<std::string> cmdToken) { cmdToken_ = cmdToken; } // 设置命令字符串的值
    void setName(std::string name) { name_ = name; } // 命令名

    bool sortToken(std::vector<Cmdt> tplate); // 根据 tplate 的模板排序出相同顺序、数量的 cmdToken_
    std::vector<double> curlyToken(std::string token); // 分离出花括号字符串 arg 中的数字
    core::dmtx squareToken(std::string token); // 分离出方括号字符串 arg 中存储的矩阵

    bool isempty(std::string outStr); // 检查 args 是否为空
    bool semicolonDel(); // 处理末尾分号，返回是否输出标识

private:
    std::string name_; // 命令名
    std::vector<std::string> cmdToken_; // 命令字符串，过后会删掉命令名，仅剩参数
};
extern CmdHandler cmdhr; // 不可复制，仅作为命名解析器使用

bool tksCheck(std::vector<std::string> tks); // in() 调用，检查输入参数的语法正确与否
Cmdt argtype(std::string str); // 返回 str 属于哪种参数
void delCh(std::string& str, char tobedel); // 去除首尾空格

} // namespace laxb