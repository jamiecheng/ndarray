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

namespace nd {
    enum class value_t : uint8_t {
        null,
        scalar,
        array,
    };

    template<class T>
    class array {
        using this_type = array<T>;
        using reference = array &;
        using const_reference = const array &;

        using shape_t = std::deque<unsigned long>;
        using strides_t = std::deque<unsigned long>;
        using vector_t = std::vector<T>;

        //////////////////////
        // friend operators //
        //////////////////////

        friend std::ostream &operator<<(std::ostream &os, const array<T> &ar) {
            return os << ar.dump();
        }

    public:
        union array_value {
            T scalar;
            vector_t *values;

            array_value() = default;

            array_value(T sc) : scalar(sc) {}

            array_value(const vector_t &vec) { values = __create_vector(vec); }
        };

        //////////////////
        // constructors //
        //////////////////

        array(const this_type &other, unsigned long offset = 0)
                : m_type(other.m_type),
                  m_shape(other.m_shape),
                  m_strides(other.m_strides),
                  m_base(nullptr),
                  m_offset(offset) {
            if (m_type == value_t::array) m_value = *other.m_value.values;
            else m_value = other.m_value.scalar;
        }

        array(T &&val, this_type *base_from = nullptr, unsigned long offset = 0)
                : m_base(base_from),
                  m_type(value_t::scalar),
                  m_offset(offset) {
            static_assert(std::is_arithmetic<T>::value, "incompatible element type");

            m_value.scalar = std::forward<T>(val);
            m_shape = {1};
        }

        array(std::initializer_list<array> list)
                : m_type(value_t::array),
                  m_base(nullptr) {
            // type becomes array, so make vector active in the union
            m_value.values = __create_vector(vector_t());

            m_shape.emplace_front(list.size());

            for (auto &val : list) {
                if (val.m_type == value_t::scalar) { // construct individual elements form list
                    m_value.values->push_back(val.m_value.scalar);
                    __set_strides(m_shape);
                } else { // add values to the actual object
                    m_value.values->insert(std::end(*m_value.values), std::begin(val.data()), std::end(val.data()));
                    m_shape = val.shape();
                    m_shape.emplace_front(list.size());
                    __set_strides(m_shape);
                }
            }
        }

        array(vector_t data, const shape_t &shape, this_type *base_from = nullptr, unsigned long offset = 0)
                : m_shape(shape),
                  m_base(base_from),
                  m_offset(offset) {
            m_type = value_t::array;
            m_value.values = __create_vector(data.begin(), data.end());
            __set_strides(shape);
        }

        array(const shape_t &shape, T init)
                : m_shape(shape),
                  m_type(value_t::array) {
            auto size = std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<unsigned long>());

            m_value.values = __create_vector(vector_t(size, init));

            __set_strides(m_shape);
        }

        ~array() {
            if (m_type == value_t::array) {
                delete m_value.values;
            }
        }

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
                result << m_value.scalar;
            } else if (m_shape.size() == 1) {
                if (m_base == nullptr) result << "[ ";
                else result << "  [ ";

                for (unsigned long i = 0; i < m_shape.at(0); ++i) {
                    result << " " << std::to_string(item({i})) << " ";
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
            if (m_type != value_t::array)
                throw std::runtime_error("current value is not an array");

            return *m_value.values;
        }

        T &item(const shape_t &indexes) const {
            if (indexes.size() != m_shape.size())
                throw std::invalid_argument("requested shape size is not equal with the current shape size");

            if (m_type == value_t::scalar)
                throw std::invalid_argument("type is already is a scalar");

            // calculate offset
            unsigned long offset = {m_offset};

            for (int i = 0; i < m_shape.size(); ++i) {
                offset += indexes[i] * m_strides[i];
            }

//            if (offset > m_value.values->size())
//                throw std::range_error("requested index is out of range(" +
//                                       std::to_string(m_value.values->size()) + ")");

            return m_value.values->at(offset);
        }

        unsigned long rows() const { return m_shape.at(0); }

        unsigned long columns() const {
            if (m_shape.size() == 2) return m_shape.at(1);

            /// TBD
            return 0;
        }

        const shape_t &shape() const {
            return m_shape;
        }

        unsigned long offset() const { return m_offset; }

        unsigned long size() const {
            return m_value.values->size();
        }

        unsigned long ndim() const {
            return m_shape.size();
        }

        value_t type() const {
            return m_type;
        }

        const this_type *base() const { return m_base; }

        bool is_scalar() const { return m_type == value_t::scalar; }

        bool is_array() const { return m_type == value_t::array; }

        this_type at(unsigned long index) const {
            if (index > m_shape.at(0) || m_type == value_t::scalar)
                throw std::range_error("requested index is out of range");

            strides_t indexes(ndim(), 0);
            unsigned long offset = {m_offset};

            indexes.at(0) = index;

            for (unsigned long i = 0; i < indexes.size(); ++i) {
                offset += indexes.at(i) * m_strides.at(i);
            }

            this_type ret = *this;

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
                ret.m_value.scalar = m_value.values->at(offset);
            }

            return ret;
        }

        this_type operator[](unsigned long index) const {
            return at(index);
        }

        void arrange(unsigned long start, unsigned long end, T step = 1) {
            for (auto &val : *m_value.values) {
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

                    revpermutation[axis] = i;
                    permutation[i] = axis;
                }
            }

            this_type ret = *this;

            for (unsigned long i = 0; i < n; ++i) {
                ret.m_shape.at(i) = m_shape.at(permutation[i]);
                ret.m_strides.at(i) = m_strides.at(permutation[i]);
            }

            return ret;
        }

        ////////////////
        //  modifiers //
        ////////////////

        void swap(reference other) {
            std::swap(m_type, other.m_type);
            std::swap(m_value, other.m_value);
            std::swap(m_shape, other.m_shape);
            std::swap(m_strides, other.m_strides);
            std::swap(m_base, other.m_base);
            std::swap(m_offset, other.m_offset);
        }

        void set_val(T val, unsigned long offset = 0) {
            if (m_type == value_t::scalar) m_value.scalar = val;
            else m_value.values->at(offset) = val;
        }

        void reshape(const shape_t &shape) {
            auto pd = std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<unsigned long>());

            if (pd != m_value.values->size())
                throw std::invalid_argument("total size of new array must be unchanged when setting a new shape");
            else
                m_shape = shape;

            __set_strides(m_shape);
        }

        template <typename callback>
        void unary_expr(callback clb) {
            if (m_type == value_t::array) {
                for (auto &val : *m_value.values) {
                    val = clb(val);
                }
            } else {
                m_value.scalar = clb(m_value.scalar);
            }
        }

        template <typename callback>
        void unary_expr(const_reference other, callback clb) {
            if(m_type != other.type() || size() != other.size())
                throw std::invalid_argument("cannot perform unary_expr() if arrays are not equal");

            if (m_type == value_t::array) {
                for (unsigned long i = 0; i < size(); ++i) {
                    m_value.values->at(i) = clb(m_value.values->at(i), other.data().at(i));
                }
            } else {
                m_value.scalar = clb(m_value.scalar, other.m_value.scalar);
            }
        }

        void random(int min, int max) {
            std::random_device rd;
            std::mt19937 rng(rd());
            std::uniform_real_distribution<T> uni(-1, 1);

            for (auto &val : *m_value.values) {
                val = uni(rng);
            }
        }

        const this_type &operator=(T val) {
            if (m_type != value_t::scalar) throw std::invalid_argument("can't assign number, current type is array");

            if (m_base == nullptr) {
                m_value.scalar = val;
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
            unary_expr([&val](double v){ return v + val; });
            return *this;
        }

        const_reference operator+=(const_reference other) {
            unary_expr(other, std::plus<T>());
            return *this;
        }

        const_reference operator-=(T val) {
            unary_expr([&val](double v){ return v - val; });
            return *this;
        }

        const_reference operator-=(const_reference other) {
            unary_expr(other, std::minus<T>());
            return *this;
        }

        const_reference operator*=(T val) {
            unary_expr([&val](double v){ return v * val; });
            return *this;
        }

        const_reference operator*=(const_reference other) {
            unary_expr(other, std::multiplies<T>());
            return *this;
        }

        const_reference operator/=(T val) {
            unary_expr([&val](double v){ return v / val; });
            return *this;
        }

        const_reference operator/=(const_reference other) {
            unary_expr(other, std::divides<T>());
            return *this;
        }

    private:
        value_t m_type = value_t::null;
        array_value m_value = {};
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

        static vector_t *__create_vector(const vector_t &vec) {
            return new vector_t(vec.begin(), vec.end());
        }

        static vector_t *
        __create_vector(const typename vector_t::iterator &first, const typename vector_t::iterator &second) {
            return new vector_t(first, second);
        }

        std::string __to_string(const T val, const int n = 6) const {
            char out[64];
            sprintf(out, "%0.3f", val);
            return out;
        }
    };
}

#endif //ARRAY_ARRAY_HPP
