#include "MatMN.h"

using namespace math;

MatMN::MatMN(const int _M, const int _N) {
    M = _M;
    N = _N;
    rows = new VecN[M];
    for (int i = 0; i < M; i++)
        rows[i] = VecN(N);
}

const MatMN & MatMN::operator=(const MatMN &rhs) {
    if (this == &rhs)
        return *this;

    delete[] rows;
    M = rhs.M;
    N = rhs.N;
    rows = new VecN[M];
    for (int i = 0; i < M; i++)
        rows[i] = rhs.rows[i];
    return *this;
}

const MatMN & MatMN::operator*=(float rhs) const {
    for (int i = 0; i < M; i++)
        rows[i] *= rhs;
    return *this;
}

VecN MatMN::operator*(const VecN &rhs) const {
    // check that the incoming vector is of the correct dimension
    if (rhs.m_num != N)
        return rhs;

    VecN tmp(M);
    for (int i = 0; i < M; i++)
        tmp[i] = rhs.dot(rows[i]);
    return tmp;
}

MatMN MatMN::operator*(const MatMN &rhs) const {
    // check that the incoming matrix is of the correct dimesion
    if (rhs.M != N && rhs.N != M)
        return rhs;

    const MatMN transposedRHS = rhs.Transpose();

    MatMN tmp(M, rhs.N);
    for (int m = 0; m < M; m++)
        for (int n = 0; n < rhs.N; n++)
            tmp.rows[m][n] = rows[m].dot(transposedRHS.rows[n]);
    return tmp;
}

MatMN MatMN::operator*(const float rhs) const {
    MatMN tmp = *this;
    for (int m = 0; m < M; m++)
        for (int n = 0; n < N; n++)
            tmp.rows[m][n] *= rhs;
    return tmp;
}

void MatMN::Zero() const {
    for (int i = 0; i < M; i++)
        rows[i].Zero();
}

MatMN MatMN::Transpose() const {
    MatMN tmp(N, M);
    for (int m = 0; m < M; m++)
        for (int n = 0; n < N; n++)
            tmp.rows[n][m] = rows[m][n];
    return tmp;
}
