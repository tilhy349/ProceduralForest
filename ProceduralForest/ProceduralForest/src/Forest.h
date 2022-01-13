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
	const float widthOfTerrain, depthOfTerrain;

	//Terrain specifications
	const float verticalScale = 0.7f; //Vertical scale of height values
	const int octaves = 5; //Number of octaves for perlin noice
	const float step = 0.01f; //Tile size in world coords

	float lowBound = 0; //Low-bound for noise values
	float highBound = 0; //High-bound for noise values

	const int verticesWidth; //Number of vertices along width
	const int verticesDepth; //Number of vertices along height

	//Leaves data
	std::unique_ptr<VertexBuffer> instanceVBrow1;
	std::unique_ptr<VertexBuffer> instanceVBrow2;
	std::unique_ptr<VertexBuffer> instanceVBrow3;
	std::unique_ptr<VertexBuffer> instanceVBrow4;

	std::unique_ptr<VertexArray> leafVAO;
	std::unique_ptr<VertexBuffer> leafVB;
	int numberOfInstances;

public:
	Forest(unsigned int program, float width, float depth);
	~Forest();

	/// <summary>
	/// Render the trees.
	/// </summary>
	void Render();

	/// <summary>
	/// Render the leaves on the trees.
	/// </summary>
	void RenderLeaves();

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
	/// <summary>
	/// Function for adding a tree at a specific position, 
	/// with a specified height, depth and number of branches.
	/// </summary>
	/// <param name="pos">The position</param>
	/// <param name="height">The height of the each branch. All branches except the tree trunk will be scaled.</param>
	/// <param name="maxDepth">Max depth of the tree, i.e number of recursion calls.</param>
	/// <param name="maxBranches">Max number of branches for each branching point.</param>
	void AddTree(const glm::vec3 pos, const float height, const int maxDepth, const int maxBranches);

	/// <summary>
	/// Recursive function which creates the branches for a tree.
	/// </summary>
	/// <param name="maxDepth">The depth of the tree.</param>
	/// <param name="currentDepth">The current depth of the current recursion call.</param>
	/// <param name="height">The height of a tree branch.</param>
	/// <param name="branches">Number of branches for each branching point.</param>
	/// <param name="totalScale">Total scale of the current matrix in the top of the stack.</param>
	void MakeBranches(const int maxDepth, int currentDepth, const float height, const int branches, float totalScale);

	/// <summary>
	/// Creates the vertices with vertex positions, normals, texture coordinates and also the indices for a cylinder object.
	/// </summary>
	/// <param name="aSlices">Number of slices for the bottom and top tringle strips.</param>
	/// <param name="height">Height of the cylinder</param>
	/// <param name="topwidth">Width of the top of the cylinder.</param>
	/// <param name="bottomwidth">Width of the bottom of the cylinder.</param>
	void CreateCylinder(int aSlices, float height, float topwidth, float bottomwidth);

	/// <summary>
	/// Function which genereates the data for the terrain.
	/// </summary>
	void GenerateTerrain();

	/// <summary>
	/// Function which generate the data for the leaves.
	/// </summary>
	void GenerateLeaves();
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

