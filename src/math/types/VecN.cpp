#include "VecN.h"

using namespace math;

VecN::VecN(const int N) {
    m_num = N;
    m_data = new float[N];
}

VecN::VecN(const VecN &rhs) {
    m_num = rhs.m_num;
    m_data = new float[m_num];
    for (int i = 0; i < m_num; i++)
        m_data[i] = rhs.m_data[i];
}

VecN & VecN::operator=(const VecN &rhs) {
    delete[] m_data;

    m_num = rhs.m_num;
    m_data = new float[m_num];
    for (int i = 0; i < m_num; i++)
        m_data[i] = rhs.m_data[i];

    return *this;
}

const VecN & VecN::operator*=(const float rhs) {
    for (int i = 0; i < m_num; i++)
        m_data[i] *= rhs;

    return *this;
}

const VecN & VecN::operator-=(const VecN &rhs) {
    for (int i = 0; i < m_num; i++)
        m_data[i] -= rhs.m_data[i];

    return *this;
}

const VecN & VecN::operator+=(const VecN &rhs) {
    for (int i = 0; i < m_num; i++)
        m_data[i] += rhs.m_data[i];

    return *this;
}

VecN VecN::operator*(const float rhs) const {
    VecN tmp = *this;
    tmp *= rhs;
    return tmp;
}

VecN VecN::operator+(const VecN &rhs) const {
    VecN tmp = *this;
    for (int i = 0; i < m_num; i++)
        tmp.m_data[i] += rhs.m_data[i];

    return tmp;
}

VecN VecN::operator-(const VecN &rhs) const {
    VecN tmp = *this;
    for (int i = 0; i < m_num; i++)
        tmp.m_data[i] -= rhs.m_data[i];

    return tmp;
}

float VecN::dot(const VecN &rhs) const {
    float sum = 0;
    for (int i = 0; i < m_num; i++)
        sum += m_data[i] * rhs.m_data[i];
    return sum;
}

void VecN::Zero() const {
    for (int i = 0; i < m_num; i++)
        m_data[i] = 0.0f;
}
