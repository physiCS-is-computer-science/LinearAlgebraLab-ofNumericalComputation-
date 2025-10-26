// matrix_vector_output.hpp

#include "utils/output.hpp"
#include "core/matrix.hpp"
#include "core/vector.hpp"
#include <iostream>
#include <string>

namespace util {

/* 三个常量，分别为错误字符串、标准输入字符串、标准输出字符串 */
OutPathID ERS = OutPathID(">! ");
OutPathID SOS = OutPathID(">: ");
OutPathID SIS = OutPathID("<< ");

void startupBanner() {
    std::string name{"LAL v0.1 (Beta)"};

    std::cout << "+---------- " << name << " ----------+\n";
    std::cout << " > Support: Matrix/Vector Calculation\n";
    std::cout << " > Real number version               \n";
    std::cout << "+-------------------------------------+\n\n";
}

} // namespace util