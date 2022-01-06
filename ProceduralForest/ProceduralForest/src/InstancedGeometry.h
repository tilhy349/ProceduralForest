#pragma once
#include "Geometry.h"

//TODO: Move content from main into this class and create instance of this class in class forest
class InstancedGeometry : Geometry {
private:
	int numberOfInstances;
	std::vector<VertexBuffer> instancedVBs;

public:
	InstancedGeometry(std::vector<float>* verts, std::vector<std::vector<vec3>>* VBs, int noInstances);
	~InstancedGeometry();

	void Render();
};