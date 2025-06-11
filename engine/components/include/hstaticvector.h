#ifndef HSTATICVECTOR_H
#define HSTATICVECTOR_H

namespace hf
{
    template<typename T, size_t N>
    struct StaticVector
    {
        inline void push_back(T element)
        {
            m_Elements[m_Size] = element;
            m_Size++;
        }

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
