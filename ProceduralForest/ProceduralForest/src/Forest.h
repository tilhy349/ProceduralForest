#pragma once
#include "vendor/glugg/glugg.h"
#include <random>
#include <glm.hpp>
#include <vector>

class Forest
{
private:
	unsigned int m_RendererID;
	int verticeCount;
	float widthOfTerrain, depthOfTerrain;

public:
	Forest(unsigned int program, float width, float depth);
	~Forest();

	void Render();

	std::vector<vec4>* leafMatrixRow1; 
	std::vector<vec4>* leafMatrixRow2;
	std::vector<vec4>* leafMatrixRow3;
	std::vector<vec4>* leafMatrixRow4;

private:
	void AddTree(glm::vec3 pos, float height, float maxDepth, float maxBranches);
	void MakeBranches(const int maxDepth, int currentDepth, float currentHeight, int branches, float totalScale);
	void CreateCylinder(int aSlices, float height, float topwidth, float bottomwidth);
};

//Random generator of doubles, ints, floats
template<typename Numeric, typename Generator = std::mt19937>
Numeric random(Numeric from, Numeric to)
{
	thread_local static Generator gen(std::random_device{}());

	using dist_type = typename std::conditional
		<
		std::is_integral<Numeric>::value
		, std::uniform_int_distribution<Numeric>
		, std::uniform_real_distribution<Numeric>
		>::type;

	thread_local static dist_type dist;

	return dist(gen, typename dist_type::param_type{ from, to });
}

