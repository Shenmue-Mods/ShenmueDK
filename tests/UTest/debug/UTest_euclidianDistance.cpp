#include "gtest/gtest.h"

#include <iostream>

#include <eigen3/Eigen/Dense>

namespace {

TEST(memstream, read_write)
{
    Eigen::Matrix<float, 4, 4> mat1;
    Eigen::Matrix<float, 4, 4> mat2;
    mat1.setConstant(2.0f);
    mat2.setConstant(4.0f);
    mat2(8) = 7.0f;

    Eigen::Matrix<float, 4, 4> mat3 = mat1 - mat2;
    float sum = mat3.sum();

    std::cout << std::endl;
    std::cout << mat1 << std::endl;
    std::cout << std::endl;
    std::cout << mat2 << std::endl;
    std::cout << std::endl;
    std::cout << mat3 << std::endl;
    std::cout << std::endl;
    std::cout << sum << std::endl;

    std::cout << std::log2(256) + 1 << std::endl;

    SUCCEED();
}

}

