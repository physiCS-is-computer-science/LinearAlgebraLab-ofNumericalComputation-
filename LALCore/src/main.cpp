// ============================================================================
// 此文件为 LAL 程序主循环
//
// ============================================================================

#include "core/matrix.hpp"
#include "core/vector.hpp"
#include "utils/output.hpp"
#include <iostream>

int main() {
    Matrix mtx1{{10, 20, 30}};
    Matrix mtx2{{9, 10, 11, 12, 13},
                {14, 15, 16, 17, 18}};
    Vector vec{1, 2, 3};

    std::cout << mtx1 + ~vec;

    return 0;
}