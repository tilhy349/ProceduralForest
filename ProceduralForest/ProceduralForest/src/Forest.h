#pragma once
#include "vendor/glugg/glugg.h"
#include <glm.hpp>
#include <vector>

class Forest
{
private:
	unsigned int m_RendererID;
	int verticeCount;

public:
	Forest(unsigned int program);
	~Forest();

	void Render();

	std::vector<vec4>* leafMatrixRow1; 
	std::vector<vec4>* leafMatrixRow2;
	std::vector<vec4>* leafMatrixRow3;
	std::vector<vec4>* leafMatrixRow4;

private:
	void AddTree(glm::vec3 pos, float height, float maxDepth, float maxBranches);
	void MakeBranches(const int maxDepth, int currentDepth, float currentHeight, float currentBottonWidth, float currentTopWidth, int branches);
	void CreateCylinder(int aSlices, float height, float topwidth, float bottomwidth);
};

