// ============================================================================
// 此文件为 LAL 程序主循环
//
// ============================================================================

#include "core/matrix.hpp"
#include "core/vector.hpp"
#include "utilities/matrix_vector_output.hpp"
#include <iostream>
#include <stdexcept>

int main() {
    Matrix mtx1{{1, 2},
                {3, 4},
                {5, 6},
                {7, 8}};
    Matrix mtx2{{9, 10, 11, 12, 13},
                {14, 15, 16, 17, 18}};
    Vector vec{1, 2, 3};

    mvout << dot(~vec, vec) << '\n'
          << cross(vec, ~vec);

    return 0;
}