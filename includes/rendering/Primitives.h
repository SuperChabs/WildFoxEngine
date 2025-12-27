#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include "rendering/buffer.h"
#include "core/shader.h"

class PrimitivesRenderer 
{
private:
    // Cube
    VertexArray* cubeVAO;
    VertexBuffer* cubeVBO;
    
    // Quad
    VertexArray* quadVAO;
    VertexBuffer* quadVBO;
    
    // Plane
    VertexArray* planeVAO;
    VertexBuffer* planeVBO;

    bool initialized;
    
    void SetupCube();
    void SetupQuad();
    void SetupPlane();

public:
    PrimitivesRenderer();
    ~PrimitivesRenderer();
    
    void Initialize();
    void Cleanup();
    
    void RenderCube();
    void RenderQuad();
    void RenderPlane();
};

#endif // PRIMITIVES_HPP