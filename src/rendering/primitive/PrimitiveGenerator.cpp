#include "PrimitiveGenerator.h"

const float *PrimitiveGenerator::GetCubeData(size_t &outSize) {
    outSize = sizeof(CUBE_VERTICES);
    return CUBE_VERTICES;
}

const float *PrimitiveGenerator::GetQuadData(size_t &outSize) {
    outSize = sizeof(QUAD_VERTICES);
    return QUAD_VERTICES;
}

const float *PrimitiveGenerator::GetPlaneData(size_t &outSize) {
    outSize = 0;
    return nullptr;
}