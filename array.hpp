//
// Created by jamie on 7/16/17.
//

#ifndef ARRAY_ARRAY_HPP
#define ARRAY_ARRAY_HPP

#include <type_traits>
#include <vector>
#include <numeric>
#include <memory>
#include <sstream>
#include <deque>
#include <algorithm>

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
        using strides_t = std::vector<unsigned long>;
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

            array_value(const vector_t &vec) { values = create_vector(vec); }
        };

        //////////////////
        // constructors //
        //////////////////

        array(const this_type &other)
                : m_type(other.m_type),
                  m_shape(other.m_shape),
                  m_strides(other.m_strides),
                  m_base(nullptr) {
            if (m_type == value_t::array) m_value = *other.m_value.values;
            else m_value = other.m_value.scalar;
        }

        array(T &&val, const this_type *base_from = nullptr)
                : m_base(base_from) {
            static_assert(std::is_arithmetic<T>::value, "incompatible element type");

            m_type = value_t::scalar;
            m_value.scalar = std::forward<T>(val);
            m_shape = {1};
        }

        array(std::initializer_list<array> list) :
                m_base(nullptr) {
            // type becomes array, so make vector active in the union
            m_type = value_t::array;
            m_value.values = create_vector(vector_t());

            m_shape.emplace_front(list.size());

            for (auto &val : list) {
                if (val.m_type == value_t::scalar) { // construct individual elements form list
                    m_value.values->push_back(val.scalar());
                    __set_strides(m_shape);
                } else { // add values to the actual object
                    m_value.values->insert(std::end(*m_value.values), std::begin(val.data()), std::end(val.data()));
                    m_shape = val.shape();
                    m_shape.emplace_front(list.size());
                    __set_strides(m_shape);
                }
            }
        }

        array(vector_t data, const shape_t &shape, const this_type *base_from = nullptr)
                : m_shape(shape),
                  m_base(base_from) {
            m_type = value_t::array;
            m_value.values = create_vector(data.begin(), data.end());
            __set_strides(shape);
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
                result << scalar();
            } else if (m_shape.size() == 1) {
                result << "  [ ";
                for (const auto &val : *m_value.values) {
                    result << "  " << val << " ";
                }
                result << "  ]";
            } else {
                result << "[\n";
                for (int j = 0; j < rows(); j++) {
                    result << at(j).dump() << "\n";
                }
                result << "]";
            }

            return result.str();
        }

        const vector_t &data() const {
            if (m_type != value_t::array)
                throw std::runtime_error("current value is not an array, use .scalar() instead");

            return *m_value.values;
        }

        T scalar() const {
            if (m_type != value_t::scalar) throw std::runtime_error("current value is not a scalar");

            return m_value.scalar;
        }

        bool is_scalar() const { return m_type == value_t::scalar; }

        bool is_array() const { return m_type == value_t::array; }

        unsigned long rows() const { return m_shape.at(0); }

        unsigned long columns() const {
            if (m_shape.size() == 2) return m_shape.at(1);

            /// TBD
            return 0;
        }

        const shape_t &shape() const {
            return m_shape;
        }

        unsigned long size() {
            return m_value.values->size();
        }

        unsigned long ndim() const {
            return m_shape.size();
        }

        value_t type() const {
            return m_type;
        }

        const this_type *base() const { return m_base; }

        this_type at(unsigned long index) const {
            if (index > m_shape.at(0) - 1 || m_type == value_t::scalar)
                throw std::range_error("no value at index " + std::to_string(index));

            // is a 1 dim array, get scalar
            if (m_shape.size() == 1) { return this_type(std::move(m_value.values->at(index)), this); }

            auto first = m_value.values->begin() + (index * m_strides.at(0));
            auto second = m_value.values->begin() + ((index + 1) * m_strides.at(0));
            vector_t new_data(first, second);

            return this_type(new_data, std::deque<unsigned long>(m_shape.begin() + 1, m_shape.end()), this);
        }

        this_type operator[](unsigned long index) const {
            return at(index);
        }

        ////////////////
        // modifiers //
        ////////////////

        void swap(reference other) {
            std::swap(m_type, other.m_type);
            std::swap(m_value, other.m_value);
            std::swap(m_shape, other.m_shape);
            std::swap(m_strides, other.m_strides);
        }

        void set_shape(const shape_t &shape) {
            auto pd = std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<unsigned long>());

            if (pd != m_value.values->size())
                throw std::invalid_argument("total size of new array must be unchanged when setting a new shape");
            else
                m_shape = shape;

            __set_strides(m_shape);
        }

        void unary_expr(T(*callback)(T)) {
            if (m_type == value_t::array) {
                for (auto &val : *m_value.values) {
                    val = callback(val);
                }
            } else {
                m_value.scalar = callback(m_value.scalar);
            }
        }

    private:
        value_t m_type = value_t::null;
        array_value m_value = {};
        shape_t m_shape;
        strides_t m_strides;
        const this_type *m_base;

        // private setters
        void __set_strides(const shape_t &shape) {
            m_strides = std::vector<unsigned long>(m_shape.size());

            for (int i = (int) (m_shape.size() - 2); i >= 0; i--) {
                m_strides.at(i) = (unsigned long) std::accumulate(shape.begin() + i + 1, shape.end(), 1,
                                                                  std::multiplies<unsigned long>());
            }
        }

        static vector_t *create_vector(const vector_t &vec) {
            return new vector_t(vec.begin(), vec.end());
        }

        static vector_t *
        create_vector(const typename vector_t::iterator &first, const typename vector_t::iterator &second) {
            return new vector_t(first, second);
        }
    };
}

#endif //ARRAY_ARRAY_HPP
