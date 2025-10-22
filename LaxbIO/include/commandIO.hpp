// ======================================================
// 处理输入与输出
// - 字符串解析，解析成功后调用 LALCore 接口计算
// - 需要输出时调用 Workspace，查找输出的东西放在哪
// ======================================================

#include <string>
#include <vector>

class CmdParser {
public:
    CmdParser() = default;
    ~CmdParser() = default;
    CmdParser(const CmdParser&) = delete;
    CmdParser(CmdParser&&) = delete;
    CmdParser& operator=(const CmdParser&) = delete;
    CmdParser& operator=(CmdParser&&) = delete;

    std::vector<std::string> in(); // 处理输入，错误则返回空 vector，正确则返回语法正确的 tokens
};
extern CmdParser cmdpr; // 不可复制，仅作为命名解析器使用

bool tksCheck(std::vector<std::string> tks); // in() 调用，检查输入参数的语法正确与否