#include "GJK.h"

#include "EPA.h"
#include "ShapeFuncStruckt.h"
#include "ShapeFunctions.h"
#include "SignedVolumeFunc.h"
#include "glm/vec3.hpp"
#include "math/Math.h"

bool GJK::DoesIntersect(ECSWorld &ecs, entt::entity entityA, entt::entity entityB, const float bias, glm::vec3 & ptOnA, glm::vec3 & ptOnB) {
    const glm::vec3 origin = glm::vec3(0.0f);

    int numPts = 1;
    Point simplexPoints[4];
    simplexPoints[0] = Shape::Support(ecs, entityA, entityB, glm::vec3{1, 1, 1}, 0.0f);

    float closestDist = 1e10f;
    bool doesContainOrigin = false;
    glm::vec3 newDir = simplexPoints[0].xyz * -1.0f;
    do {
        // get the new point to check
        Point newPt = Shape::Support(ecs, entityA, entityB, newDir, 0.0f);

        // if the new point is the same as a previous point, then we can’t expand any further
        if (SignedVolumeFunc::HasPoint(simplexPoints, newPt)) break;

        simplexPoints[numPts] = newPt;
        numPts++;

        // if this new point hasn’t moved passed the origin, then the origin cannot be in the set. And therefore there is no collision
        float dotdot = glm::dot(newPt.xyz - origin, newDir);
        if (dotdot < 0.0f) break;

        glm::vec4 lambdas(0.0f);
        doesContainOrigin = SignedVolumeFunc::SimplexSignedVolume(simplexPoints, numPts, newDir, lambdas);
        if (doesContainOrigin) break;

        // check that the new projection of the origin onto simplex is closer than the previous
        float dist = glm::length(newDir);
        if (dist >= closestDist) break;
        closestDist = dist;

        // use the lambdas that support the new search direction, and invalidate any points that don’t support it
        SignedVolumeFunc::SortValids(simplexPoints, lambdas);
        numPts = SignedVolumeFunc::NumValids(lambdas);
        doesContainOrigin = (4 == numPts);
    } while (!doesContainOrigin);

    if (!doesContainOrigin) return false;

    //
    // Check that we have a 3-simplex (EPA expects a tetrahedron)
    //
    if (numPts == 1) {
        glm::vec3 searchDir = simplexPoints[0].xyz * -1.0f;
        Point newPt = Shape::Support(ecs, entityA, entityB, searchDir, 0.0f);
        simplexPoints[numPts] = newPt;
        numPts++;
    }
    if (numPts == 2) {
        glm::vec3 ab = simplexPoints[1].xyz - simplexPoints[0].xyz;
        glm::vec3 u, v;
        math::GetOrtho(ab, u, v);

        newDir = u;
        Point newPt = Shape::Support(ecs, entityA, entityB, newDir, 0.0f);
        simplexPoints[numPts] = newPt;
        numPts++;
    }
    if (numPts == 3) {
        glm::vec3 ab = simplexPoints[1].xyz - simplexPoints[0].xyz;
        glm::vec3 ac = simplexPoints[2].xyz - simplexPoints[0].xyz;
        glm::vec3 norm = glm::cross(ac, ab);

        newDir = norm;
        Point newPt = Shape::Support(ecs, entityA, entityB, newDir, 0.0f);
        simplexPoints[numPts] = newPt;
        numPts++;
    }

    //
    // Expand the simplex by the bias amount
    //

    // get the center point of the simplex
    auto avg = glm::vec3(0.0f);
    for (auto & simplexPoint : simplexPoints)
        avg += simplexPoint.xyz;
    avg *= 0.25f;

    // now expand the simplex by the bias amount
    for (int i = 0; i < numPts; i++) {
        Point& pt = simplexPoints[i];

        glm::vec3 dir = pt.xyz - avg; // ray from "center" to witness point
        dir = glm::normalize(dir);
        pt.ptA += dir * bias;
        pt.ptB -= dir * bias;
        pt.xyz = pt.ptA - pt.ptB;
    }

    //
    // Perform EPA expansion of the simplex to find the closest face on the CSO
    //
    EPA::Expand(ecs, entityA, entityB, bias, simplexPoints, ptOnA, ptOnB);
    return true;
}

void GJK::CLosestPoints(ECSWorld &ecs, entt::entity entityA, entt::entity entityB, glm::vec3 &ptOnA, glm::vec3 &ptOnB) {
    const auto origin = glm::vec3(0.0f);

    float closestDist = 1e10f;
    const float bias = 0.0f;

    int numPts = 1;
    Point simplexPoints[4];
    simplexPoints[0] = Shape::Support(ecs, entityA, entityB, glm::vec3(1), bias);

    glm::vec4 lambdas = glm::vec4(1, 0, 0, 0);
    glm::vec3 newDir = simplexPoints[0].xyz * -1.0f;
    do {
        // get the point to check on
        Point newPt = Shape::Support(ecs, entityA, entityB, newDir, bias);

        // if the new point is the same as a previous point, then we can't expand any further
        if (SignedVolumeFunc::HasPoint(simplexPoints, newPt)) break;

        // add point and get new search direction
        simplexPoints[numPts] = newPt;
        numPts++;

        SignedVolumeFunc::SimplexSignedVolume(simplexPoints, numPts, newDir, lambdas);
        SignedVolumeFunc::SortValids(simplexPoints, lambdas);
        numPts = SignedVolumeFunc::NumValids(lambdas);

        // check that the new projection of the origin onto simplex is closer than the previous
        float dist = glm::length(newDir);
        if (dist >= closestDist) break;

        closestDist = dist;
    } while (numPts < 4);

    ptOnA = glm::vec3(0.0f);
    ptOnB = glm::vec3(0.0f);
    for (int i = 0; i < 4; i++) {
        ptOnA += simplexPoints[i].ptA * lambdas[i];
        ptOnB += simplexPoints[i].ptB * lambdas[i];
    }
}
