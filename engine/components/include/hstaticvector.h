#ifndef HSTATICVECTOR_H
#define HSTATICVECTOR_H
#include "hgenericexception.h"

namespace hf
{
    template<typename T, size_t N>
    struct StaticVector
    {
        static_assert(!std::is_reference_v<T>, "StaticVector cannot store reference types");
        StaticVector() = default;
        StaticVector(const StaticVector& other)
        {
            m_Size = other.m_Size;
            for (size_t i = 0; i < m_Size; ++i)
                m_Elements[i] = other.m_Elements[i];
        }

        StaticVector(StaticVector&& other) noexcept
        {
            m_Size = other.m_Size;
            for (size_t i = 0; i < m_Size; ++i)
                m_Elements[i] = std::move(other.m_Elements[i]);
            other.m_Size = 0;
        }

        StaticVector& operator=(const StaticVector& other)
        {
            if (this != &other)
            {
                m_Size = other.m_Size;
                for (size_t i = 0; i < m_Size; ++i)
                    m_Elements[i] = other.m_Elements[i];
            }
            return *this;
        }

        inline void push_back(T element)
        {
            if (m_Size + 1 > N) throw GENERIC_EXCEPT("[Hyperflow]", "Array out of bounds!");
            m_Elements[m_Size] = element;
            m_Size++;
        }

        inline void push_back(const T* element, size_t size)
        {
            if (m_Size + size > N) throw GENERIC_EXCEPT("[Hyperflow]", "Array out of bounds!");

            memcpy(&m_Elements[m_Size], element, size * sizeof(T));
            m_Size += size;
        }

        inline const T& atC(size_t index) { return m_Elements[index]; }
        inline T& at(size_t index) { return m_Elements[index]; }
        inline T* atP(size_t index) { return &m_Elements[index]; }

        inline void pop_back() { m_Size--; }
        inline size_t size() const { return m_Size; }
        inline void clear() { m_Size = 0; }

        inline T* begin() { return m_Elements; }
        inline T* end() { return m_Elements + m_Size; }

        inline const T* begin() const { return m_Elements; }
        inline const T* end() const { return m_Elements + m_Size; }

        inline const T* cbegin() const { return m_Elements; }
        inline const T* cend() const { return m_Elements + m_Size; }

    private:
        T m_Elements[N]{};
        size_t m_Size = 0;
    };
}

#endif //HSTATICVECTOR_H
