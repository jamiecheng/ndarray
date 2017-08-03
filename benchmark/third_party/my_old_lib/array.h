//
// Created by jamie on 6/30/17.
//

#ifndef NET_MATRIX_HPP
#define NET_MATRIX_HPP

#include <utility>
#include <valarray>
#include <string>
#include <vector>
#include <vector>
#include <functional>
#include <random>

namespace nnet {
    class Array {
        friend std::ostream &operator<<(std::ostream &os, const Array &m) {
            return os << Array::toString(m);
        }

        // ---------------------------- operator functions ----------------------------
        friend inline Array operator+(const Array &lhs, double val) {
            Array tmp = lhs;

            for (int i = 0; i < tmp.rows(); i++) {
                for (int j = 0; j < tmp.columns(); j++) {
                    tmp.set(i, j, tmp.get(i, j) + val);
                }
            }

            return tmp;
        }

        friend inline Array operator+(const Array &lhs, const Array &ar) {
            Array tmp = lhs;

            if (tmp.columns() != ar.columns() || tmp.rows() != ar.rows())
                throw std::runtime_error("arrays do not have equal sizes");

            for (int i = 0; i < tmp.rows(); i++) {
                for (int j = 0; j < tmp.columns(); j++) {
                    tmp.set(i, j, tmp.get(i, j) + ar.get(i, j));
                }
            }

            return tmp;
        }

        friend inline Array operator-(const Array &lhs, double val) {
            Array tmp = lhs;

            for (int i = 0; i < tmp.rows(); i++) {
                for (int j = 0; j < tmp.columns(); j++) {
                    tmp.set(i, j, tmp.get(i, j) - val);
                }
            }

            return tmp;
        }

        friend inline Array operator-(const Array &lhs, const Array &ar) {
            Array tmp = lhs;

            if (tmp.columns() != ar.columns() || tmp.rows() != ar.rows())
                throw std::runtime_error("arrays do not have equal sizes");

            for (int i = 0; i < tmp.rows(); i++) {
                for (int j = 0; j < tmp.columns(); j++) {
                    tmp.set(i, j, tmp.get(i, j) - ar.get(i, j));
                }
            }

            return tmp;
        }

        friend inline Array operator*(const Array &lhs, double val) {
            Array tmp = lhs;

            for (int i = 0; i < tmp.rows(); i++) {
                for (int j = 0; j < tmp.columns(); j++) {
                    tmp.set(i, j, tmp.get(i, j) * val);
                }
            }

            return tmp;
        }

        friend inline Array operator*(const Array &lhs, const Array &ar) {
            Array tmp = lhs;

            if (tmp.columns() != ar.columns() || tmp.rows() != ar.rows())
                throw std::runtime_error("arrays do not have equal sizes");

            for (int i = 0; i < tmp.rows(); i++) {
                for (int j = 0; j < tmp.columns(); j++) {
                    tmp.set(i, j, tmp.get(i, j) * ar.get(i, j));
                }
            }

            return tmp;
        }

        friend inline Array operator/(const Array &lhs, double val) {
            Array tmp = lhs;

            for (int i = 0; i < tmp.rows(); i++) {
                for (int j = 0; j < tmp.columns(); j++) {
                    tmp.set(i, j, tmp.get(i, j) / val);
                }
            }

            return tmp;
        }

        friend inline Array operator/(const Array &lhs, const Array &ar) {
            Array tmp = lhs;

            if (tmp.columns() != ar.columns() || tmp.rows() != ar.rows())
                throw std::runtime_error("arrays do not have equal sizes");

            for (int i = 0; i < tmp.rows(); i++) {
                for (int j = 0; j < tmp.columns(); j++) {
                    tmp.set(i, j, tmp.get(i, j) / ar.get(i, j));
                }
            }

            return tmp;
        }

        friend Array operator-(double lhs, const Array &rhs) {
            Array tmp = rhs;

            for (int i = 0; i < tmp.rows(); i++) {
                for (int j = 0; j < tmp.columns(); j++) {
                    tmp.set(i, j, lhs - rhs.get(i, j));
                }
            }

            return tmp;
        }

        friend Array operator*(double lhs, const Array &rhs) {
            Array tmp = rhs;

            for (int i = 0; i < tmp.rows(); i++) {
                for (int j = 0; j < tmp.columns(); j++) {
                    tmp.set(i, j, lhs * rhs.get(i, j));
                }
            }

            return tmp;
        }

        friend Array operator/(double lhs, Array rhs) {
            Array tmp = rhs;

            for (int i = 0; i < tmp.rows(); i++) {
                for (int j = 0; j < tmp.columns(); j++) {
                    tmp[i][j] = lhs / rhs[i][j];
                }
            }

            return tmp;
        }

    public:
        using array_type = std::vector<std::vector<double>>;

        // ---------------------------- constructors ----------------------------
        Array(unsigned long rows, unsigned long colums) :
                rows_(rows),
                columns_(colums),
                array_(rows, std::vector<double>(colums)) {}

        explicit Array(const std::vector<std::vector<double>> &matrix) :
                rows_(matrix.size()),
                columns_(matrix.at(0).size()),
                array_(matrix) {}

        Array(std::initializer_list<std::initializer_list<double>> list) : rows_(list.size()),
                                                                           columns_(list.begin()->size()) {
            for (const auto &row : list) {
                array_.push_back(std::vector<double>(row.begin(), row.end()));
            }
        }

        // ---------------------------- modifiers ----------------------------
        inline Array unaryExpr(double(*callback)(double)) {
            Array tmp(array_);

            for (int i = 0; i < tmp.rows(); i++) {
                for (int j = 0; j < tmp.columns(); j++) {
                    tmp.set(i, j, callback(get(i, j)));
                }
            }

            return tmp;
        }

        inline static Array exp(const Array &ar) {
            Array tmp = ar;

            for (int i = 0; i < tmp.rows(); i++) {
                for (int j = 0; j < tmp.columns(); j++) {
                    tmp.set(i, j, std::exp(tmp.get(i, j)));
                }
            }

            return tmp;
        }

        inline Array dot(const Array &ar) {
            Array tmp(rows(), ar.columns());

            for (int i = 0; i < rows(); i++) {
                for (int j = 0; j < ar.columns(); j++) {
                    double sum = 0.0;

                    for (int k = 0; k < columns(); k++) {
                        sum = sum + array_.at(i).at(k) * ar.get(k, j);
                    }

                    tmp.set(i, j, sum);
                }
            }

            return tmp;
        }

        inline static double mean(const Array &m) {
            double mean{};

            for (int i = 0; i < m.rows(); i++) {
                for (int j = 0; j < m.columns(); j++) {
                    mean += std::abs(m.get(i, j));
                }
            }

            return mean / (m.rows() * m.columns());
        }

        inline static Array randomize(const Array &m, int min, int max) {
            std::random_device rd;
            std::mt19937 rng(rd());
            std::uniform_real_distribution<double> uni(min, max);

            Array tmp = m;

            for (int i = 0; i < tmp.rows(); i++) {
                for (int j = 0; j < tmp.columns(); j++) {
                    tmp.set(i, j, uni(rng));
                }
            }

            return tmp;
        }

        inline static std::string toString(const Array &matrix) {
            std::string out;

            for (auto row : matrix.matrix()) {
                out += "[ ";
                for (auto value : row) {
                    out += std::to_string(value) + " ";
                }
                out += "]\n";
            }

            return out;
        }

        inline Array transponse() {
            Array tmp(columns(), rows());

            for (int j = 0; j < columns(); j++) {
                tmp[j] = column(j);
            }

            return tmp;
        }

        // ---------------------------- acces functions ----------------------------
        inline void set(int r, int c, double val) {
            array_[r][c] = val;
        }

        inline double get(int r, int c) const {
            return array_.at(r).at(c);
        }

        inline std::vector<double> column(int index) {
            if (index >= columns()) throw std::range_error("column index out of range");

            std::vector<double> tmp(rows());

            for (int i = 0; i < rows(); i++) {
                tmp[i] = array_[i][index];
            }

            return tmp;
        }

        inline unsigned long rows() const { return rows_; }

        inline unsigned long columns() const { return columns_; }

        inline const std::vector<std::vector<double>> &matrix() const { return array_; }

        inline std::vector<double> &operator[](int index) { return array_[index]; }

    private:
        unsigned long rows_;
        unsigned long columns_;
        array_type array_;
    };
}

#endif //NET_MATRIX_HPP
