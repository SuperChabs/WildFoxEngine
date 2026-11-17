#include "MatN.h"

using namespace math;

MatN::MatN(const int N) {
    numDimensions = N;
    rows = new VecN[N];
    for (int i = 0; i < N; i++)
        rows[i] = VecN(N);
}

const MatN &MatN::operator=(const MatN &rhs) {
    numDimensions = rhs.numDimensions;
    rows = new VecN[numDimensions];
    for (int i = 0; i < numDimensions; i++)
        rows[i] = rhs.rows[i];
    return *this;
}

const MatN& MatN::operator=(const MatMN &rhs) {
    if (rhs.M != rhs.N)
        return *this;

    numDimensions = rhs.N;
    rows = new VecN[numDimensions];
    for (int i = 0; i < numDimensions; i++)
        rows[i] = rhs.rows[i];
    return *this;
}

void MatN::Identity() const {
    for (int i = 0; i < numDimensions; i++) {
        rows[i].Zero();
        rows[i][i] = 1.0;
    }
}

void MatN::Zero() const {
    for (int i = 0; i < numDimensions; i++)
        rows[i].Zero();
}

void MatN::Transpose() {
    const MatN tmp(numDimensions);

    for (int i = 0; i < numDimensions; i++)
        for (int j = 0; j < numDimensions; j++)
            tmp.rows[j][i] = rows[j][i];

    *this = tmp;
}

void MatN::operator*=(float rhs) const {
    for (int i = 0; i < numDimensions; i++)
        rows[i] *= rhs;
}

VecN MatN::operator*(const VecN &rhs) const {
    VecN tmp(numDimensions);

    for (int i = 0; i < numDimensions; i++)
        tmp[i] = rows[i].dot(rhs);

    return tmp;
}

MatN MatN::operator*(const MatN &rhs) const {
    MatN tmp(numDimensions);
    tmp.Zero();

    for (int i = 0; i < numDimensions; i++)
        for (int j = 0; j < numDimensions; j++)
            tmp.rows[j][i] = rows[j][i] * rhs.rows[i][j];

    return tmp;
}
