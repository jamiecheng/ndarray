//
// Created by jamie on 8/2/17.
//

#include "gtest/gtest.h"

#include "array.hpp"

using array = nd::array<double>;

TEST(matrix_op, transpose) {
    array array2d = {
            {1, 2, 3},
            {4, 5, 6}
    };

    array t = {
            {1, 4},
            {2, 5},
            {3, 6}
    };

    EXPECT_EQ(array2d.transpose(), t);
}

TEST(matrix_op, dot) {
    {
        array a = {1, 2, 3, 4, 5};
        array b = {1, 2, 3, 4, 5};
        array c = 55;

        auto res = a.dot(b);

        EXPECT_EQ(res, c);
    }

    {
        array a = {
                {1, 2, 3},
                {4, 5, 6}
        };

        array b = {
                {7, 8},
                {9, 10},
                {11, 12}
        };

        array c = {
                {58, 64},
                {139, 154}
        };

        EXPECT_EQ(a.dot(b), c);
    }
}

