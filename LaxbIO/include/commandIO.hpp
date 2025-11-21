// ======================================================
// 处理输入与输出
// - 字符串解析，解析成功后调用 LALCore 接口计算
// - 需要输出时调用 Workspace，查找输出的东西放在哪
// ======================================================

#include "core/matrix.hpp"
#include "session_manager.hpp"
#include <string>
#include <vector>

namespace laxb {

/* 五种命令标识符 */
enum class Cmdt {
    OPT, // -
    INID, // :
    OUTID, // ::
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

    /* 一组设置读取成员的函数 */
    std::vector<std::string>& getcmdtk() { return cmdToken_; } // 获取命令字符串
    std::string& getname() { return name_; } // 命令名
    bool getoutbit() { return outbit_; }
    void setcmdtk(std::vector<std::string> cmdToken) { // 设置命令字符串的值
        cmdToken_ = cmdToken;
    }
    void setname(std::string name) { name_ = name; } // 命令名

    bool sortToken(const std::vector<Cmdt>& tplate); // 根据 tplate 的模板排序出相同顺序、数量的 cmdToken_
    std::size_t argHandler(const std::vector<std::vector<laxb::Cmdt>>& arglsts); // 接受变量形式列表，按照传递顺序返回类型数
    std::vector<double> curlyToken(std::string token); // 分离出花括号字符串 arg 中的数字
    core::dmtx squareToken(std::string token); // 分离出方括号字符串 arg 中存储的矩阵

    bool isempty(std::string outStr); // 检查 args 是否为空
    void semicolonDel(); // 处理末尾分号，置位 outbit_

    /* 根据 outbit_ 判断是否向 semgr 写入输出并执行 */
    template <typename T>
    void outDetermine(const T& output) {
        if (outbit_ == true) {
            smr::semgr << output;
        }
    }

    static std::size_t type_g;

private:
    std::string name_; // 命令名
    std::vector<std::string> cmdToken_; // 命令字符串，过后会删掉命令名，仅剩参数
    bool outbit_;
};
extern CmdHandler cmdhr; // 不可复制，仅作为命名解析器使用

using tvv = std::vector<std::vector<laxb::Cmdt>>; // 原写法真是太长了

bool tksCheck(std::vector<std::string> tks); // in() 调用，检查输入参数的语法正确与否
Cmdt argtype(std::string str); // 返回 str 属于哪种参数
void delCh(std::string& str, char tobedel); // 去除首尾空格
std::vector<std::string> splitBych(std::string& str, char sep = ' '); // 以字符 sep 为分隔符，分割出 tokens，结果不包含 sep

} // namespace laxb