#include <iostream>

#include "array.hpp"

using namespace std;

int main() {
    nd::array<double> ar = {
            {{1, 2, 3}, {1, 2, 3}, {1, 2, 3}},
            {{4, 5, 6}, {4, 5, 6}, {4, 5, 6}}
    };

    ar.unary_expr([](double val) { return val * 10; });

    cout << "n dimensions: " << ar.ndim() << "\n";
    cout << "n elements: " << ar.size() << "\n";
    cout << ar << "\n";

    nd::array<double> ar2 = {1, 2, 3, 4, 5, 6};

    ar2[0] = 960;

    cout << "n dimensions: " << ar2.ndim() << "\n";
    cout << "n elements: " << ar2.size() << "\n";
    cout << ar2 << "\n";

    ar2.set_shape({2, 3});

    cout << "new shape:" << "\n";
    cout << ar2 << "\n";

    return 0;
}
