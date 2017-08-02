//
// Created by jamie on 8/2/17.
//

#include "gtest/gtest.h"

#include "array.hpp"

using Array = nd::array<double>;

TEST(matrix_op, transpose) {
    Array array2d = {
            {1, 2, 3},
            {4, 5, 6}
    };

    Array t = {
            {1, 4},
            {2, 5},
            {3, 6}
    };

    EXPECT_EQ(array2d.transpose(), t);
}

