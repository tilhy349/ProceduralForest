#pragma once
#include "vendor/glugg/glugg.h"
#include "vendor/noise/noise1234.h"
#include <random>
#include <glm.hpp>
#include <vector>
#include <memory>

#include "Geometry.h"

class Forest
{
private:
	unsigned int m_RendererID;
	int verticeCount;
	float widthOfTerrain, depthOfTerrain;

	//Terrain specifications
	const float verticalScale = 1.0f; //Vertical scale of height values
	const int octaves = 5; //Number of octaves for perlin noice
	const float step = 0.01f; //Tile size in world coords

	float lowBound = 0; //Low-bound for noise values
	float highBound = 0; //High-bound for noise values

	const int verticesWidth; //Number of vertices along width
	const int verticesDepth; //Number of vertices along height

public:
	Forest(unsigned int program, float width, float depth);
	~Forest();

	void Render();
	void FreeMatrixData() {
		delete leafMatrixCol1;
		delete leafMatrixCol2;
		delete leafMatrixCol3;
		delete leafMatrixCol4;
	}

	std::unique_ptr<Geometry> terrain;

	std::vector<vec3>* leafMatrixCol1; 
	std::vector<vec3>* leafMatrixCol2;
	std::vector<vec3>* leafMatrixCol3;
	std::vector<vec3>* leafMatrixCol4;

private:
	void AddTree(glm::vec3 pos, float height, int maxDepth, int maxBranches);
	void MakeBranches(const int maxDepth, int currentDepth, float currentHeight, int branches, float totalScale);
	void CreateCylinder(int aSlices, float height, float topwidth, float bottomwidth);
	void GenerateTerrain();
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

