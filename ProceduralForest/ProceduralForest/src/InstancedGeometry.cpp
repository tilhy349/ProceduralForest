#include "InstancedGeometry.h"

InstancedGeometry::InstancedGeometry(std::vector<float>* verts, std::vector<std::vector<vec3>>* VBs, int instances) : numberOfInstances{instances}, Geometry(verts, nullptr)
{
    //instancedVBs = *VBs;
    // also set instance data
    for(int i = 0; i < instancedVBs.size(); ++i){
        glEnableVertexAttribArray(2);
        instancedVBs[i].Bind(); // this attribute comes from a different vertex buffer
        glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        instancedVBs[i].UnBind();
        glVertexAttribDivisor(i, 1); // tell OpenGL this is an instanced vertex attribute.
    }
    
}

InstancedGeometry::~InstancedGeometry()
{
}

void InstancedGeometry::Render()
{
}
