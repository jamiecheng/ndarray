#include "gtest/gtest.h"

#include "array.hpp"

using array = nd::array<double>;

TEST(basic_op, data) {
    array array1d = {1, 2, 3, 4, 5};
    std::vector<double> vec = {1, 2, 3, 4, 5};

    EXPECT_EQ(array1d.data(), vec);

    array array2d = {
            {1, 2, 3},
            {4, 5, 6}
    };
    std::vector<double> vec2 = {1, 2, 3, 4, 5, 6};

    EXPECT_EQ(array2d.data(), vec2);
}

TEST(basic_op, ndim) {
    array array2d = {
            {1, 2, 3},
            {4, 5, 6}
    };
    EXPECT_EQ(array2d.ndim(), 2);
}

TEST(basic_op, strides) {
    array array2d = {
            {1, 2, 3},
            {4, 5, 6}
    };
    EXPECT_EQ(array2d.strides(), std::deque<unsigned long>({3, 1}));
}

TEST(basic_op, shape) {
    array array3d = {
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
    array array2d = {
            {1, 2, 3},
            {4, 5, 6}
    };
    EXPECT_EQ(array2d.type(), nd::value_t::array);

    array array = 1;
    EXPECT_EQ(array.type(), nd::value_t::scalar);
}

TEST(basic_op, item) {
    array array2d = {
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

    nd::array<int> expected = {1, 2, 3};

    EXPECT_EQ(array2d.at(0), expected);

    expected = {4, 5, 6};
    EXPECT_EQ(array2d.at(1), expected);
}

TEST(basic_op, compare) {
    {
        nd::array<int> a = {
                {1, 2, 3},
                {4, 5, 7}
        };

        nd::array<int> a1 = {
                {1, 2, 3},
                {4, 5, 7}
        };

        EXPECT_EQ(a, a1);
    }

    {
        nd::array<int> a = {
                {1, 2, 3},
                {4, 5, 7}
        };

        nd::array<int> a1 = {
                {1, 2, 3},
                {4, 5, 7}
        };

        EXPECT_EQ(a1.hash(), a.hash());
    }

    {
        nd::array<int> a = {
                {144, 244, 344},
                {444, 544, 447}
        };

        nd::array<int> a1 = {
                {1, 2, 3},
                {4, 5, 7}
        };

        EXPECT_NE(a, a1);
    }
}
