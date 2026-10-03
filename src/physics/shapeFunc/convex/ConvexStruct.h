#pragma once

struct Tri {
    int a, b, c;
};

struct Edge {
    int a, b;

    bool operator==(const Edge &rhs) const {
        return ((a == rhs.a && b == rhs.b) || (a == rhs.b && b == rhs.a));
    }
};