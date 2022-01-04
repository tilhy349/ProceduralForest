#include "InstancedGeometry.h"

InstancedGeometry::InstancedGeometry(std::vector<float>* verts, int instances) : numberOfInstances{instances}, Geometry(verts, nullptr)
{
	
	//instancedVBs{};
}

InstancedGeometry::~InstancedGeometry()
{
}

void InstancedGeometry::Render()
{
}
