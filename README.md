# ndarray
Multidimensional array in modern C++, inspired by Numpy array. This
library is currently incomplete. More functions will be added.

# Example
```c++
nd::array<double> array1d = {1, 2, 3, 4};

std::cout << array1d[0] << "\n"
std::cout << array1d * 3 << "\n";

nd::array<double> array3d = {
            {{1, 2, 3}, {1, 2, 3}, {1, 2, 3}},
            {{4, 5, 6}, {4, 5, 6}, {4, 5, 6}}
};

std::cout << array3d[0] << "\n";
std::cout << array3d[0][1][2] << "\n";
std::cout << array3d.transpose() << "\n"
```

# Build

Enter the following commands in a terminal:
- `git clone https://github.com/jamiecheng/ndarray; cd ndarray`
- `git submodule update --init --recursive`
- `mkdir build; cd build`
- `cmake ..`
- `make`
