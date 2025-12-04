// =================================================
// 存放所有命令语法的 "cmdname": pair<str, str> 对
// =================================================

#pragma once

#include <map>
#include <string>
#include <utility>

namespace laxb {

using constr = const std::string;

/* ==== 矩阵创建与操作 ==== */
constr eyeMan{
    "# Create unit matrix\n"
    "::varn:    Output to this variable\n"
    "{d}:       Number of dimension of matrix"
};
constr zerosMan{
    "# Create a zero matrix\n"
    "::varn:    Output to this variable"
    "{m, n}:    Number of rows and columns of matrix"
};
constr onesMan{
    "# Create an all-one matrix\n"
    "::varn:    Output to this variable\n"
    "{m, n}:    Number of rows and columns of matrix"
};
constr diagMan{
    "# Get matrix diagonal elements\n"
    "-m:        Gets the main diagonal elements\n"
    "-a:        Gets the anti-diagonal element\n"
    ":varn:     Accept a variable\n"
    "::varn:    Output to this variable"
};
constr horzcatMan{
    "# Connect two matrices horizontally\n"
    ":varn:     Accept a variable\n"
    "           :varn :varn\n"
    "::varn:    Output to this variable"
};
constr vertcatMan{
    "# Connect two matrices vertically\n"
    ":varn:     Accept a variable\n"
    "           :varn :varn\n"
    "::varn:    Output to this variable"
};

/* ==== 矩阵基本信息 ==== */
constr sizeMan{
    "# Output matrix dimension\n"
    ":varn:    Accept a variable"
};
constr isequalMan{
    "# Determine whether the two matrices are equal\n"
    ":varn:    Accept a variable\n"
    "          :varn :varn"
};

/* ==== 矩阵运算 ==== */
constr transposeMan{
    "# Matrix transposition\n"
    ":varn:     Accept a variable\n"
    "::varn:    Output to this variable"
};
constr plusMan{
    "# Add two matrices together\n"
    ":varn:     Accept a variable\n"
    "           :varn :varn\n"
    "::varn:    Output to this variable"
};
constr minusMan{
    "# Subtraction of two matrices\n"
    ":varn:     Accept a variable\n"
    "           :varn :varn\n"
    "::varn:    Output to this variable"
};
constr mtimesMan{
    "# Multiplication of two matrices\n"
    ":varn:     Accept a variable\n"
    "           :varn :varn\n"
    "::varn:    Output to this variable"
};
constr timesMan{
    "# The two matrices are multiplied by elements one by one.\n"
    ":varn:     Accept a variable\n"
    "           :varn :varn\n"
    "::varn:    Output to this variable"
};
constr divideMan{
    "# Divided of two matrices\n"
    ":varn:     Accept a variable\n"
    "           :varn :varn\n"
    "::varn:    Output to this variable"
};
constr powerMan{
    "# The power of two matrices by element\n"
    ":varn:     Accept a variable\n"
    "           :varn :varn\n"
    "::varn:    Output to this variable"
};
constr sumMan{
    "# Summing matrix / vector elements\n"
    ":varn:     Accept a variable\n"
    "::varn:    Output to this variable"
};
constr meanMan{
    "# Calculate the average value of all elements of the matrix / vector\n"
    ":varn:     Accept a variable\n"
    "::varn:    Output to this variable"
};
constr maxMan{
    "# Find the maximum value of matrix / vector elements\n"
    ":varn:     Accept a variable\n"
    "::varn:    Output to this variable"
};
constr minMan{
    "# Find the minimum value of matrix / vector elements\n"
    ":varn:     Accept a variable\n"
    "::varn:    Output to this variable"
};

/* ==== 矩阵分解 ==== */
constr luMan{
    "# LU decomposition of the matrix\n"
    "-a:        The output includes all the results of L, U, P\n"
    ":varn:     Accept a variable\n"
    "::varn:    Output to this variable\n"
    "           ::L ::U\n"
    "           ::L ::U ::P"
};
constr qrMan{
    "# QR decomposition of the matrix\n"
    ":varn:     Accept a variable\n"
    "::varn:    Output to this variable\n"
    "           ::Q ::E"
};

/* ==== 线性方程组求解 ==== */
constr invMan{
    "# Matrix inversion\n"
    ":varn:     Accept a variable\n"
    "::varn:    Output to this variable"
};
constr linsolveMan{
    "# Solving linear equations Ax = b\n"
    "-a:       The total solution of the output [xp xn] form of the matrix\n"
    ":varn:    Accept a variable\n"
    "          :A :b"
    "::varn:   Output to this variable\n"
    "          -a ::X\n"
    "          ::x"
};
constr rrefMan{
    "# Transform the matrix into the simplest row type\n"
    ":varn:     Accept a variable\n"
    "           :A\n"
    "::varn:    Output to this variable"
    "           ::R"
};
constr nullMan{
    "# A set of orthogonal bases of null space of output matrix\n"
    ":varn:     Accept a variable\n"
    "           :A\n"
    "::varn:    Output to this variable"
    "           ::N"
};
constr orthMan{
    "# A set of orthogonal bases of column space of output matrix\n"
    ":varn:     Accept a variable\n"
    "           :A\n"
    "::varn:    Output to this variable\n"
    "           ::C"
};
constr lsqMan{
    "# Output least squares total solution\n"
    ":varn:     Accept a variable\n"
    "           :A :b\n"
    "::varn:    Output to this variable\n"
    "           ::Np"
};

/* ==== 特征值与奇异值 ==== */

/* ==== 矩阵分析与条件数 ==== */
constr detMan{
    "# Calculate the determinant of matrix\n"
    ":varn:     Accept a variable\n"
    "           :A\n"
    "::varn:    Output to this variable\n"
    "           ::det"
};
constr rankMan{
    "# Calculate the rank of matrix\n"
    ":varn:     Accept a variable\n"
    "           :A\n"
    "::varn:    Output to this variable\n"
    "           ::rank"
};
constr traceMan{
    "# Calculate the trace of the matrix\n"
    ":varn:     Accept a variable\n"
    "           :A\n"
    "::varn:    Output to this variable\n"
    "           ::tr"
};

/* ==== 特殊矩阵 ==== */

/* ==== 基本命令 ==== */
constr quitMan{
    "-f:    Exit the currently loaded file\n"
    "-e:    Exit the LAL program"
};
constr showMan{
    "-a:       Display all variables\n"
    "-l:       Display a list of all variable names\n"
    ":varn:    Display specific variables"
};
constr varMan{
    "::varn:    Save the output to this variable\n"
    "{}:        parameter list\n"
    "[]:        Matrix elements list"
};
constr delMan{
    "-a:       Delete all variables\n"
    ":varn:    Deleting specific variables"
};
constr saveMan{
    "-f:            Forcibly write a file\n"
    "::filename:    Save the current session to the file"
};
constr loadMan{
    "-c:           Binding file\n"
    ":filename:    Variables that load this file session"
};
constr clsMan{
    "Clear screen"
};
constr helpMan{
    "-a:          Display all commands\n"
    "-l:          Display a list of all command names\n"
    ":cmdname:    Display specific command details"
};



const std::map<std::string, std::pair<std::string, std::string>> manual{
    /* ==== 矩阵创建与操作 ==== */
    {"eye",     {"::varn {d}", eyeMan}},
    {"zeros",   {"::varn {m, n}", zerosMan}},
    {"ones",    {"::varn {m, n}", onesMan}},
    {"diag",    {"-m/a :varn ::varn", diagMan}},
    {"horzcat", {":varn ::varn", horzcatMan}},
    {"vertcat", {":varn ::varn", vertcatMan}},

    /* ==== 矩阵基本信息 ==== */
    {"size",    {":varn", sizeMan}},
    {"isequal", {":varn", isequalMan}},

    /* ==== 矩阵运算 ==== */
    {"transpose", {":varn ::varn", transposeMan}},
    {"plus",      {":varn ::varn", plusMan}},
    {"minus",     {":varn ::varn", minusMan}},
    {"mtimes",    {":varn ::varn", mtimesMan}},
    {"times",     {":varn ::varn", timesMan}},
    {"divide",    {":varn ::varn", divideMan}},
    {"power",     {":varn ::varn", powerMan}},
    {"sum",       {":varn ::varn", sumMan}},
    {"mean",      {":varn ::varn", meanMan}},
    {"max",       {":varn ::varn", maxMan}},
    {"min",       {":varn ::varn", minMan}},

    /* ==== 矩阵分解 ==== */
    {"lu", {"-a :varn ::varn", luMan}},
    {"qr", {":varn ::varn", qrMan}},
    
    /* ==== 线性方程组求解 ==== */
    {"inv",      {":varn ::varn", invMan}},
    {"linsolve", {"-opt :varn ::varn", linsolveMan}},
    {"rref",     {":varn ::varn", rrefMan}},
    {"null",     {":varn ::varn", nullMan}},
    {"orth",     {":varn ::varn", orthMan}},
    {"lsq",      {":varn ::varn", lsqMan}},

    /* ==== 特征值与奇异值 ==== */

    /* ==== 矩阵分析与条件数 ==== */
    {"det",   {":varn ::varn", detMan}},
    {"rank",  {":varn ::varn", rankMan}},
    {"trace", {":varn ::varn", traceMan}},

    /* ==== 特殊矩阵 ==== */

    /* ==== 基本命令 ==== */
    {"quit", {"-opt", quitMan}},
    {"show", {"-opt ::varn", showMan}},
    {"var",  {"::varn {a} [a b; c d]", varMan}},
    {"del",  {"-opt :varn", delMan}},
    {"save", {"::filename", saveMan}},
    {"load", {":filename", loadMan}},
    {"cls",  {"", clsMan}},
    {"help", {":cmdname", helpMan}},
};

} // namespace laxb