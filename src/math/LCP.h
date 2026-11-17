#pragma once
#include "types/MatN.h"
#include "types/VecN.h"

namespace math {
    inline VecN GaussSeidel(const MatN &a, const VecN &b) {
        const int N = b.m_num;
        VecN x(N);
        x.Zero();

        for (int iter = 0; iter < N; iter++)
            for (int i = 0; i < N; i++) {
                float dx = (b[i] - a.rows[i].dot(x)) / a.rows[i][i];
                if (dx * 0.0f == dx * 0.0f)
                    x[i] = x[i] + dx;
            }

        return x;
    }
}
