#pragma once

namespace math {

    class VecN {
    public:
        int m_num;
        float *m_data;

        VecN() : m_num(0), m_data(nullptr) {}

        explicit VecN(int N);
        VecN(const VecN &rhs);
        VecN &operator=(const VecN &rhs);
        ~VecN() { delete[] m_data; }

        float operator[](const int idx) const { return m_data[idx]; }
        float &operator[](const int idx) { return m_data[idx]; }
        const VecN &operator*=(float rhs);
        const VecN &operator-=(const VecN &rhs);
        const VecN &operator+=(const VecN &rhs);
        VecN operator*(float rhs) const;
        VecN operator+(const VecN &rhs) const;
        VecN operator-(const VecN &rhs) const;

        float dot(const VecN &rhs) const;
        void Zero() const;
    };
}
