/*
MIT License

Copyright (c) 2017 Jamie Cheng

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef ARRAY_ARRAY_HPP
#define ARRAY_ARRAY_HPP

#include <sstream>
#include <deque>
#include <random>
#include <iomanip>

namespace nd {
    enum class value_t : uint8_t {
        null,
        scalar,
        array,
    };

    template<class T>
    class array {
        using this_type = array<T>;
        using reference = this_type &;
        using const_reference = const this_type &;

        using shape_t = std::deque<unsigned long>;
        using strides_t = std::deque<unsigned long>;
        using vector_t = std::vector<T>;

        //////////////////////
        // friend operators //
        //////////////////////

        friend std::ostream &operator<<(std::ostream &os, const_reference ar) {
            return os << ar.dump();
        }

        friend this_type operator+(this_type lhs, T rhs) {
            return lhs += rhs;
        }

        friend this_type operator-(this_type lhs, T rhs) {
            return lhs -= rhs;
        }

        friend this_type operator*(this_type lhs, T rhs) {
            return lhs *= rhs;
        }

        friend this_type operator/(this_type lhs, T rhs) {
            return lhs /= rhs;
        }

    public:

        //////////////////
        // constructors //
        //////////////////

        array(const this_type &other, unsigned long offset = 0)
                : m_data(other.data()),
                  m_type(other.m_type),
                  m_shape(other.m_shape),
                  m_strides(other.m_strides),
                  m_base(nullptr),
                  m_offset(offset) {
        }

        array(T &&val, unsigned long offset = 0)
                : m_type(value_t::scalar),
                  m_offset(offset) {
            static_assert(std::is_arithmetic<T>::value, "incompatible element type");

            m_data.resize(1);
            m_data.at(0) = std::forward<T>(val);
            m_shape = {1};
        }

        array(std::initializer_list<array> list)
                : m_type(value_t::array),
                  m_base(nullptr) {

            m_shape.emplace_front(list.size());

            for (auto &val : list) {
                if (val.m_type == value_t::scalar) { // construct individual elements form list
                    m_data.push_back(val.m_data.at(0));
                    __set_strides(m_shape);
                } else { // add values to the actual object
                    m_data.insert(std::end(m_data), std::begin(val.data()), std::end(val.data()));
                    m_shape = val.shape();
                    m_shape.emplace_front(list.size());
                    __set_strides(m_shape);
                }
            }
        }

        array(const shape_t &shape, T init)
                : m_shape(shape),
                  m_type(value_t::array) {
            auto size = std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<unsigned long>());

            m_data = vector_t(size, init);

            __set_strides(m_shape);
        }

        ~array() {}

        reference &operator=(array other) {
            swap(other);

            return *this;
        }

        /////////////////////
        // object accesors //
        /////////////////////

        std::string dump() const {
            std::stringstream result;

            if (m_type == value_t::scalar) {
                result << std::to_string(m_data.at(0));
            } else if (m_shape.size() == 1) {
                if (m_base == nullptr) result << "[ ";
                else result << "  [ ";

                for (unsigned long i = 0; i < m_shape.at(0); ++i) {
                    result << " " << at(i) << " ";
                }

                result << " ]";
            } else {
                result << "[\n";
                for (int i = 0; i < rows(); ++i) {
                    result << at(i).dump() << "\n";
                }
                result << "]";
            }

            return result.str();
        }

        const vector_t &data() const {
            return m_data;
        }

        unsigned long rows() const { return m_shape.at(0); }

        unsigned long columns() const {
            if (m_shape.size() == 2) return m_shape.at(1);

            /// TBD
            return 0;
        }

        const strides_t &strides() const { return m_strides; }

        const shape_t &shape() const { return m_shape; }

        unsigned long offset() const { return m_offset; }

        unsigned long size() const { return m_data.size(); }

        unsigned long ndim() const { return m_shape.size(); }

        value_t type() const { return m_type; }

        const this_type *base() const { return m_base; }

        bool is_scalar() const { return m_type == value_t::scalar; }

        bool is_array() const { return m_type == value_t::array; }

        T &item(const shape_t &indexes)  {
            if (indexes.size() != m_shape.size())
                throw std::invalid_argument("requested shape size is not equal with the current shape size");

            if (m_type == value_t::scalar)
                throw std::invalid_argument("type is already is a scalar");

            // calculate offset
            unsigned long offset = {m_offset};

            for (int i = 0; i < m_shape.size(); ++i) {
                offset += indexes[i] * m_strides[i];
            }

            return m_data.at(offset);
        }

        this_type at(unsigned long index) const {
            if (index > m_shape.at(0) || m_type == value_t::scalar)
                throw std::range_error("requested index is out of range");

            strides_t indexes(ndim(), 0);
            auto offset {m_offset};

            indexes.at(0) = index;

            for (unsigned long i = 0; i < indexes.size(); ++i) {
                offset += indexes.at(i) * m_strides.at(i);
            }

            auto ret = *this;

            if (m_base == nullptr) {
                ret.m_base = const_cast<this_type *>(this);
            } else {
                ret.m_base = m_base;
            }

            ret.m_shape.pop_front();
            ret.m_strides.pop_front();
            ret.m_offset = offset;

            if (m_shape.size() == 1) {
                ret.m_type = value_t::scalar;
                ret.m_data.at(0) = m_data.at(offset);
            }

            return ret;
        }

        this_type operator[](unsigned long index) const {
            return at(index);
        }

        void arrange(unsigned long start, unsigned long end, T step = 1) {
            for (auto &val : m_data) {
                if (start < end) {
                    val = start;
                    start += step;
                }
            }
        }

        this_type transpose(const shape_t &permute = {}) const {
            if (ndim() < 2)
                return *this;

            auto n = permute.size();
            int permutation[64], revpermutation[64];

            // default reverse
            if (n == 0) {
                n = ndim();
                for (unsigned long i = 0; i < n; ++i) {
                    permutation[i] = n - 1 - i;
                }
            } else {
                if (n != ndim())
                    throw std::invalid_argument("axes don't match array");

                for (unsigned long i = 0; i < n; ++i) {
                    revpermutation[i] = -1;
                }

                for (unsigned long i = 0; i < n; ++i) {
                    auto axis = permute.at(i);

                    if (revpermutation[axis] != -1) {
                        throw std::runtime_error("repeated axis in transpose");
                    }

                    revpermutation[axis] = static_cast<int>(i);
                    permutation[i] = static_cast<int>(axis);
                }
            }

            this_type ret = *this;

            for (unsigned long i = 0; i < n; ++i) {
                ret.m_shape.at(i) = m_shape.at(static_cast<unsigned long>(permutation[i]));
                ret.m_strides.at(i) = m_strides.at(static_cast<unsigned long>(permutation[i]));
            }

            return ret;
        }

        ////////////////
        //  modifiers //
        ////////////////

        void swap(reference other) {
            std::swap(m_type, other.m_type);
            std::swap(m_data, other.m_data);
            std::swap(m_shape, other.m_shape);
            std::swap(m_strides, other.m_strides);
            std::swap(m_base, other.m_base);
            std::swap(m_offset, other.m_offset);
        }

        void set_val(T val, unsigned long offset = 0) {
            m_data.at(offset) = val;
        }

        void reshape(const shape_t &shape) {
            auto pd = std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<unsigned long>());

            if (pd != m_data.size())
                throw std::invalid_argument("total size of new array must be unchanged when setting a new shape");
            else
                m_shape = shape;

            __set_strides(m_shape);
        }

        template<typename callback>
        const_reference unary_expr(callback clb) {
            for (auto &val : m_data) {
                val = clb(val);
            }

            return *this;
        }

        template<typename callback>
        const_reference unary_expr(const_reference other, callback clb) {
            if (m_type != other.type() || size() != other.size())
                throw std::invalid_argument("cannot perform unary_expr() if arrays are not equal");

            for (unsigned long i = 0; i < size(); ++i) {
                m_data.at(i) = clb(m_data.at(i), other.data().at(i));
            }

            return *this;
        }

        void random(int min, int max) {
            std::random_device rd;
            std::mt19937 rng(rd());
            std::uniform_real_distribution<T> uni(-1, 1);

            for (auto &val : m_data) {
                val = uni(rng);
            }
        }

        bool operator==(const_reference other) const {
            if(!std::equal(m_shape.begin(), m_shape.end(), other.shape().begin()) ||
                    m_type != other.type()) return false;

            ///TBD: check if values are equal
            return true;
        }

        const this_type &operator=(T val) {
            if (m_type != value_t::scalar) throw std::invalid_argument("can't assign number, current type is array");

            if (m_base == nullptr) {
                m_data.at(0) = val;
                return *this;
            } else {
                m_base->set_val(val, m_offset);
                return *m_base;
            }
        }

        ///////////////////////////
        //  arithmetic operators //
        ///////////////////////////

        const_reference operator+=(T val) {
            return unary_expr([&val](double v) { return v + val; });;
        }

        const_reference operator+=(const_reference other) {
            return unary_expr(other, std::plus<T>());;
        }

        const_reference operator-=(T val) {
            return unary_expr([&val](double v) { return v - val; });;
        }

        const_reference operator-=(const_reference other) {
            return unary_expr(other, std::minus<T>());;
        }

        const_reference operator*=(T val) {
            return unary_expr([&val](double v) { return v * val; });;
        }

        const_reference operator*=(const_reference other) {
            return unary_expr(other, std::multiplies<T>());;
        }

        const_reference operator/=(T val) {
            return unary_expr([&val](double v) { return v / val; });;
        }

        const_reference operator/=(const_reference other) {
            return unary_expr(other, std::divides<T>());;
        }

    private:
        value_t m_type = value_t::null;
        vector_t m_data;
        shape_t m_shape;
        strides_t m_strides;
        this_type *m_base = nullptr;
        unsigned long m_offset = {};

        // private setters
        void __set_strides(const shape_t &shape) {
            m_strides = strides_t(m_shape.size());

            m_strides.back() = 1;

            for (int i = (int) (m_shape.size() - 2); i >= 0; --i) {
                m_strides.at(i) = (unsigned long) std::accumulate(shape.begin() + i + 1, shape.end(), 1,
                                                                  std::multiplies<unsigned long>());
            }
        }
    };
}

#endif //ARRAY_ARRAY_HPP
