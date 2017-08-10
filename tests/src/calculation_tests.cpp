//
// Created by jamie on 8/2/17.
//

#include "gtest/gtest.h"

#include "array.hpp"

using array = nd::array<double>;

TEST(calculation, sum) {
    {
        array a = {
                {1, 2, 3},
                {1, 2, 3}
        };

        a[0] += 4;

        array exp = {5, 6, 7};
        array exp2 = {1, 2, 3};

        EXPECT_EQ(a[0], exp);
        EXPECT_EQ(a[1], exp2);

        array b = {
                {1, 2, 3},
                {1, 2, 3}
        };

        b[1][1] += 2;

        array exp3 = 4;
        array exp4 = 2;

        EXPECT_EQ(b[1][1], exp3);
        EXPECT_EQ(b[0][1], exp4);
    }

    {
        array a = {1, 2, 3};

        array exp = {2, 3, 4};

        auto b = 1 + a;

        EXPECT_EQ(exp, b);
    }
}

TEST(calculation, minus) {
    {
        array a = {1, 2, 3};
        array exp = {0, -1, -2};

        auto b = 1 - a;

        EXPECT_EQ(exp, b);
    }
}

TEST(calculation, mean) {
    {
        array a = {10, 20, 30, 40};
        EXPECT_EQ(a.mean(), 25.0);
    }
}
