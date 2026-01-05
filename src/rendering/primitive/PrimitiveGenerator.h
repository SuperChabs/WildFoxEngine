#ifndef PRIMITIVE_GENERATOR_H
#define PRIMITIVE_GENERATOR_H

#include <cstddef>

class PrimitiveGenerator 
{
public:
    static const float* GetCubeData(size_t& outSize);
    static const float* GetQuadData(size_t& outSize);
    static const float* GetPlaneData(size_t& outSize);
};

#endif