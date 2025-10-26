// ======================================================
// 处理输入与输出
// - 字符串解析，解析成功后调用 LALCore 接口计算
// - 需要输出时调用 Workspace，查找输出的东西放在哪
// ======================================================

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
class CmdParser {
public:
    CmdParser() = default;
    ~CmdParser() = default;
    CmdParser(const CmdParser&) = delete;
    CmdParser(CmdParser&&) = delete;
    CmdParser& operator=(const CmdParser&) = delete;
    CmdParser& operator=(CmdParser&&) = delete;

    std::vector<std::string> in(); // 处理输入，错误则返回空 vector，正确则返回语法正确的 tokens

    bool sortArgs(std::vector<Cmdt> tplate, std::vector<std::string>& args); // 根据 tplate 的模板排序出相同顺序、数量的 args
    std::vector<double> curlyArgs(std::string arg); // 分离出花括号字符串 arg 中的数字
    std::vector<std::vector<double>> squareArgs(std::string arg); // 分离出方括号字符串 arg 中存储的矩阵
};
extern CmdParser cmdpr; // 不可复制，仅作为命名解析器使用

bool tksCheck(std::vector<std::string> tks); // in() 调用，检查输入参数的语法正确与否
Cmdt argtype(std::string str); // 返回 str 属于哪种参数

} // namespace laxb