// condition estimator
// error analysis
// numerical  rank
// preconditioner

#pragma once

#include <limits>

namespace util {

/* double 类型数据计算有误差，无法直接 = 比较，需通过 abs() < EPSLION */
const double EPSILON = std::numeric_limits<double>::epsilon() * 10;

} // namespace util