#pragma once
#include "VecN.h"

namespace math {
    class MatMN {
    public:
        int M; // rows
        int N; // columns
        VecN *rows;

        MatMN() : M(0), N(0) {}
        MatMN(int M, int N);
        MatMN(const MatMN &rhs) { *this = rhs; }
        ~MatMN() { delete[] rows; }

        const MatMN &operator=(const MatMN &rhs);
        const MatMN &operator*=(float rhs) const;
        VecN operator*(const VecN &rhs) const;
        MatMN operator*(const MatMN &rhs) const;
        MatMN operator*(float rhs) const;

        void Zero() const;
        [[nodiscard]] MatMN Transpose() const;
    };
}
