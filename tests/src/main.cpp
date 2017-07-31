#include "gtest/gtest.h"

#include "array.hpp"

using Array = nd::array<double>;

TEST(basic_op, dimension) {
    Array array1d = {1, 2, 3, 4, 5};

    EXPECT_EQ(array1d.size(), 5);
}