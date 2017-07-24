#include <iostream>

#include "array.hpp"

using namespace std;

int main() {
    cout << "----------------------- 3D -----------------------\n";
    nd::array<double> array3d = {
            {{1, 2, 3}, {1, 2, 3}, {1, 2, 3}},
            {{4, 5, 6}, {4, 5, 6}, {4, 5, 6}}
    };

    cout << "n dimensions: " << array3d.ndim() << "\n";
    cout << "n elements: " << array3d.size() << "\n";
    cout << array3d << "\n";

    // get array in an array
    cout << "sub array is:\n" << array3d[0] << "\n";

    // get array scalar object
    cout << "scalar value: " << array3d[0][1][2] << "\n";

    // get and set a scalar value with template type T.
    // this way is faster than at() and operator[]
    array3d.item({1, 1, 1}) = 999;

    // apply function to all elements
    array3d.unary_expr([](double val) { return val / 2; });

    cout << "array is now:\n" << array3d << "\n\n";

    cout << "----------------------- 1D -----------------------\n";

    nd::array<double> array1d = {1, 2, 3, 4, 5, 6.5};

    cout << array1d << "\n";

    // make array1d a 2x3 array
    array1d.set_shape({2, 3});

    cout << "reshape array:" << "\n";
    cout << array1d << "\n";

    cout << "--------------- random initialise ----------------\n";

    nd::array<double> someRandomArray({3, 3}, 3.5, true);

    cout << someRandomArray << "\n";

    return 0;
}
