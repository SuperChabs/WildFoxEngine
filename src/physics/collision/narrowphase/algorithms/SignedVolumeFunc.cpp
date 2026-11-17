#include "SignedVolumeFunc.h"

#include <glm/matrix.hpp>

int SignedVolumeFunc::CompareSigns(const float a, const float b) {
    if (a > 0.0f && b > 0.0f) return 1;
    if (a < 0.0f && b < 0.0f) return 1;
    return 0;
}

bool SignedVolumeFunc::HasPoint(Point simplexPoints[4], const Point &newPt) {
    constexpr float precision = 1e-6f;
    for (int i = 0; i < 4; i++) {
        const glm::vec3 delta = simplexPoints[i].xyz - newPt.xyz;
        if (glm::dot(delta, delta) < precision * precision)
            return true;
    }
    return false;
}

glm::vec2 SignedVolumeFunc::SignedVolume1D(const glm::vec3 &s1, const glm::vec3 &s2) {
    const glm::vec3 ab = s2 - s1; // ray from a to b
    const glm::vec3 ap = glm::vec3(0.0f) - s1; // ray from a to origin
    const glm::vec3 p0 = s1 + ab * glm::dot(ap, ab) / glm::dot(ab, ab); // projection of the origin onto line

    // choose the axis with the greatest difference/length
    int idx = 0;
    float mu_max = 0;
    for (int i = 0; i < 3; i++) {
        const float mu = s2[i] - s1[i];
        if (mu * mu > mu_max * mu_max) {
            mu_max = mu;
            idx = i;
        }
    }

    // project the simplex points and projected origin onto the axis with the greatest length
    const float a = s1[idx];
    const float b = s2[idx];
    const float p = p0[idx];

    // get the signed distance from a to p and from p to b
    const float C1 = p - a;
    const float C2 = b - p;

    // if p is between [a,b]
    if ((p > a && p < b) || (p > b && p < a)) {
        glm::vec2 lambdas;
        lambdas.x = C2 / mu_max;
        lambdas.y = C1 / mu_max;
        return lambdas;
    }

    // if p is on the far side of a
    if ((a <= b && p <= a) || (a >= b && p >= a)) {
        return {1.0f, 0.0f};
    }

    // p must be on the far side of b
    return {0.0f, 1.0f};
}

glm::vec3 SignedVolumeFunc::SignedVolume2D(const glm::vec3 &s1, const glm::vec3 &s2, const glm::vec3 &s3) {
    glm::vec3 normal = glm::cross(s3 - s1, s2 - s1);
    glm::vec3 p0 = normal * (glm::dot(normal, s1) / glm::dot(normal, normal));

    // find the axis with the greatest projected area
    int idx = 0;
    float area_max = 0;
    for (int i = 0; i < 3; i++) {
        int j = (i + 1) % 3;
        int k = (i + 2) % 3;

        glm::vec2 a{s1[j], s1[k]};
        glm::vec2 b{s2[j], s2[k]};
        glm::vec2 c{s3[j], s3[k]};
        glm::vec2 ab = b - a;
        glm::vec2 ac = c - a;

        float area = ab.x * ac.y - ab.y * ac.x;
        if (area * area > area_max * area_max) {
            idx = i;
            area_max = area;
        }
    }

    // project onto appropriate axis
    int x = (idx + 1) % 3;
    int y = (idx + 2) % 3;
    glm::vec2 s[3];
    s[0] = {s1[x], s1[y]};
    s[1] = {s2[x], s2[y]};
    s[2] = {s3[x], s3[y]};
    glm::vec2 p = {p0[x], p0[y]};

    // get the sub-areas of the triangles formed from the projected origin and the edges
    glm::vec3 areas;
    for (int i = 0; i < 3; i++) {
        int j = (i + 1) % 3;
        int k = (i + 2) % 3;

        glm::vec2 a = p;
        glm::vec2 b = s[j];
        glm::vec2 c = s[k];
        glm::vec2 ab = b - a;
        glm::vec2 ac = c - a;

        areas[i] = ab.x * ac.y - ab.y * ac.x;
    }

    // if the projected origin is inside the triangle, then return the barycentric points
    if (CompareSigns(area_max, areas[0]) > 0 && CompareSigns(area_max, areas[1]) > 0 &&
        CompareSigns(area_max, areas[2]) > 0) {
        glm::vec3 lambdas = areas / area_max;
        return lambdas;
    }

    // if we make it here, then we need to project onto edges and determine the closest point
    float dist = 1e10f;
    glm::vec3 lambdas = glm::vec3(1.0f, 0.0f, 0.0f);
    for (int i = 0; i < 3; i++) {
        int k = (i + 1) % 3;
        int l = (i + 2) % 3;

        glm::vec3 edgesPts[3];
        edgesPts[0] = s1;
        edgesPts[1] = s2;
        edgesPts[2] = s3;

        glm::vec2 lambdaEdge = SignedVolume1D(edgesPts[k], edgesPts[l]);
        glm::vec3 pt = edgesPts[k] * lambdaEdge[0] + edgesPts[l] * lambdaEdge[1];
        if (dot(pt, pt) < dist) {
            dist = dot(pt, pt);
            lambdas[i] = 0;
            lambdas[k] = lambdaEdge[0];
            lambdas[l] = lambdaEdge[1];
        }
    }

    return lambdas;
}

glm::vec4 SignedVolumeFunc::SignedVolume4D(const glm::vec3 &s1, const glm::vec3 &s2, const glm::vec3 &s3,
    const glm::vec3 &s4) {
    const auto Minor = [](glm::mat4 other, int i, int j) {
        glm::mat3 minor;
        int yy = 0;
        for(int y = 0; y < 4; y++) {
            if ( y == j ) {
                continue;
            }
            int xx = 0 ;
            for (int x = 0; x < 4; x++) {
                if ( x == i ) continue;
                minor[xx][yy] = other[x][y];
                xx++;
            }
            yy++;
        }
        return minor ;
    };

    const auto cofactor = [Minor](glm::mat4 other, int i, int j) {
        glm::mat3 minor = Minor(other, i, j);
        return static_cast<float>(glm::pow(-1, i + 1 + j + 1)) * glm::determinant(minor);
    };

    glm::mat4 M;
    M[0] = {s1.x, s2.x, s3.x, s4.x};
    M[1] = {s1.y, s2.y, s3.y, s4.y};
    M[2] = {s1.z, s2.z, s3.z, s4.z};
    M[3] = {1.0f, 1.0f, 1.0f, 1.0f};

    glm::vec4 C4;
    C4[0] = cofactor(M, 3, 0);
    C4[1] = cofactor(M, 3, 1);
    C4[2] = cofactor(M, 3, 2);
    C4[3] = cofactor(M, 3, 3);

    const float detM = C4[0] + C4[1] + C4[2] + C4[3];

    // if the barycentric coordinates put the origin inside the simplex, then return them
    if (CompareSigns(detM, C4[0]) > 0 && CompareSigns(detM, C4[1]) > 0 &&
        CompareSigns(detM, C4[2]) > 0 && CompareSigns(detM, C4[3]) > 0) {
        return C4 * (1.0f / detM);
    }

    // if we get here, then we need to project the origin onto the faces and determinate the closest one
    glm::vec4 lambdas;
    float dist = 1e10;
    for (int i = 0; i < 4; i++) {
        const int j = (i + 1) % 4;
        const int k = (i + 2) % 4;

        glm::vec3 facePts[4];
        facePts[0] = s1;
        facePts[1] = s2;
        facePts[2] = s3;
        facePts[3] = s4;

        glm::vec3 lambdasFace = SignedVolume2D(facePts[i], facePts[j], facePts[k]);
        glm::vec3 pt = facePts[i] * lambdasFace[0] + facePts[j] * lambdasFace[1] + facePts[k] * lambdasFace[2];
        if (glm::length(pt) < dist) {
            dist = glm::length(pt);
            lambdas = glm::vec4(0.0f);
            lambdas[i] = lambdasFace[0];
            lambdas[j] = lambdasFace[1];
            lambdas[k] = lambdasFace[2];
        }
    }

    return lambdas;
}

bool SignedVolumeFunc::SimplexSignedVolume(Point pts[4], const int num, glm::vec3 &newDir,
    glm::vec4 &lambdasOut) {
    lambdasOut = glm::vec4(0.0f);
    constexpr float epsilonf = 0.0001f * 0.0001f;

    bool doesIntersect = false;
    switch (num) {
        default:
        case 2: {
            glm::vec2 lambdas = SignedVolume1D(pts[0].xyz, pts[1].xyz);
            auto v = glm::vec3(0.0f);
            for (int i = 0; i < 2; i++)
                v += pts[i].xyz * lambdas[i];
            newDir = v * -1.0f;
            doesIntersect = glm::dot(newDir, newDir) < epsilonf;
            lambdasOut[0] = lambdas[0];
            lambdasOut[1] = lambdas[1];
        } break;
        case 3: {
            glm::vec3 lambdas = SignedVolume2D(pts[0].xyz, pts[1].xyz, pts[2].xyz);
            auto v = glm::vec3(0.0f);
            for (int i = 0; i < 3; i++)
                v += pts[i].xyz * lambdas[i];
            newDir = v * -1.0f;
            doesIntersect = glm::dot(newDir, newDir) < epsilonf;
            lambdasOut[0] = lambdas[0];
            lambdasOut[1] = lambdas[1];
            lambdasOut[2] = lambdas[2];
        } break;
        case 4: {
            glm::vec4 lambdas = SignedVolume4D(pts[0].xyz, pts[1].xyz, pts[2].xyz, pts[3].xyz);
            auto v = glm::vec3(0.0f);
            for (int i = 0; i < 4; i++)
                v += pts[i].xyz * lambdas[i];
            newDir = v * -1.0f;
            doesIntersect = glm::dot(newDir, newDir) < epsilonf;
            lambdasOut[0] = lambdas[0];
            lambdasOut[1] = lambdas[1];
            lambdasOut[2] = lambdas[2];
            lambdasOut[3] = lambdas[3];
        } break;
    }

    return doesIntersect;
}

void SignedVolumeFunc::SortValids(Point simplexPoints[4], glm::vec4 &lambdas) {
    bool valids[4];
    for (int i = 0; i < 4; i++) {
        valids[i] = true;
        if (lambdas[i] == 0.0f)
            valids[i] = false;
    }

    auto validLambdas = glm::vec4(0.0f);
    int validCount = 0;
    Point validPts[4];
    for (int i = 0; i < 4; i++) {
        if (valids[i]) {
            validPts[validCount] = simplexPoints[i];
            validLambdas[validCount] = lambdas[i];
            validCount++;
        }
    }

    // copy the valids back into simplexPoints
    for (int i = 0; i < 4; i++) {
        simplexPoints[i] = validPts[i];
        lambdas[i] = validLambdas[i];
    }
}

int SignedVolumeFunc::NumValids(const glm::vec4 &lambdas) {
    int num = 0;
    for (int i = 0; i < 4; i++)
        if (lambdas[i] != 0.0f)
            num++;
    return num;
}
