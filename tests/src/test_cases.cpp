#include "gtest/gtest.h"

#include "array.hpp"

using Array = nd::array<double>;

TEST(basic_op, data) {
    Array array1d = {1, 2, 3, 4, 5};
    std::vector<double> vec = {1, 2, 3, 4, 5};

    EXPECT_EQ(array1d.data(), vec);

    Array array2d = {
            {1, 2, 3},
            {4, 5, 6}
    };
    std::vector<double> vec2 = {1, 2, 3, 4, 5, 6};

    EXPECT_EQ(array2d.data(), vec2);
}

TEST(basic_op, ndim) {
    Array array2d = {
            {1, 2, 3},
            {4, 5, 6}
    };
    EXPECT_EQ(array2d.ndim(), 2);
}

TEST(basic_op, strides) {
    Array array2d = {
            {1, 2, 3},
            {4, 5, 6}
    };
    EXPECT_EQ(array2d.strides(), std::deque<unsigned long>({3, 1}));
}

TEST(basic_op, shape) {
    Array array3d = {
            {
                    {1, 2, 3},
                    {1, 2, 3},
                    {1, 2, 3}
            },

            {
                    {4, 5, 6},
                    {4, 5, 6},
                    {4, 5, 6}
            }
    };

    EXPECT_EQ(array3d.shape(), std::deque<unsigned long>({2, 3, 3}));
}

TEST(basic_op, type) {
    Array array2d = {
            {1, 2, 3},
            {4, 5, 6}
    };
    EXPECT_EQ(array2d.type(), nd::value_t::array);

    Array array = 1;
    EXPECT_EQ(array.type(), nd::value_t::scalar);
}

TEST(basic_op, item) {
    Array array2d = {
            {1, 2, 3},
            {4, 5, 6}
    };
    EXPECT_EQ(array2d.item({0, 0}), 1);
    EXPECT_EQ(array2d.item({1, 2}), 6);
}

TEST(basic_op, at) {
    nd::array<int> array2d = {
            {1, 2, 3},
            {4, 5, 6}
    };

    EXPECT_EQ(array2d.at(1).shape(), std::deque<unsigned long>({3}));
    EXPECT_EQ(array2d.at(1).offset(), 3);
}

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

TEST(calculation, sum) {
    ///TBD
}
