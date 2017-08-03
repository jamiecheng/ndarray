//
// Created by jamie on 8/2/17.
//

#include "gtest/gtest.h"

#include "array.hpp"

using Array = nd::array<double>;

TEST(calculation, sum) {
    Array a = {
            {1, 2, 3},
            {1, 2, 3}
    };

    a[0] += 4;

    Array exp = {5, 6, 7};
    Array exp2 = {1, 2, 3};

    EXPECT_EQ(a[0], exp);
    EXPECT_EQ(a[1], exp2);

    Array b = {
            {1, 2, 3},
            {1, 2, 3}
    };

    b[1][1] += 2;

    Array exp3 = 4;
    Array exp4 = 2;

    EXPECT_EQ(b[1][1], exp3);
    EXPECT_EQ(b[0][1], exp4);
}

