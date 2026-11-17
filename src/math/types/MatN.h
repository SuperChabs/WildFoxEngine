#pragma once
#include "MatMN.h"
#include "VecN.h"

namespace math {
    class MatN {
    public:
        int numDimensions;
        VecN *rows;

        MatN() : numDimensions(0) {}
        explicit MatN(int N);
        MatN(const MatN &rhs) { *this = rhs; }
        MatN(const MatMN &rhs) { *this = rhs; }
        ~MatN() { delete[] rows; }

        const MatN &operator=(const MatN &rhs);
        const MatN &operator=(const MatMN &rhs);

        void Identity() const;
        void Zero() const;
        void Transpose();

        void operator*=(float rhs) const;
        VecN operator*(const VecN &rhs) const;
        MatN operator*(const MatN &rhs) const;
    };
}
