#include "Forest.h"
#include <iostream>

Forest::Forest(unsigned int program, float width, float depth) : widthOfTerrain{width}, depthOfTerrain{ depth }
{

    GenerateTerrain();

    leafMatrixRow1 = new std::vector<vec4>();
    leafMatrixRow2 = new std::vector<vec4>();
    leafMatrixRow3 = new std::vector<vec4>();
    leafMatrixRow4 = new std::vector<vec4>();

    gluggSetPositionName("inPosition");
    gluggSetNormalName("inNormal");
    gluggSetTexCoordName("inTexCoord");

    gluggBegin(GLUGG_TRIANGLES);

    //TODO: Create function which places the randomly generated trees over the terrain

    AddTree(glm::vec3(widthOfTerrain/2, 0, depthOfTerrain / 4), 1.5, 5, 4);
    AddTree(glm::vec3(widthOfTerrain / 2 - 2, 0, depthOfTerrain / 4), 2.0, 4, 5);
    AddTree(glm::vec3(widthOfTerrain / 2 + 2, 0, depthOfTerrain / 4), 2.2, 6, 4);
    
    //AddTree(glm::vec3(-2, 0, 0), 3.0, 3, 3);
    //AddTree(glm::vec3(1, 0, 2), 2.0, 1, 3);

    m_RendererID = gluggEnd(&verticeCount, program, 0);
}

Forest::~Forest()
{

}

void Forest::Render()
{
    glBindVertexArray(m_RendererID);	// Select VAO
    glDrawArrays(GL_TRIANGLES, 0, verticeCount);
}

void Forest::AddTree(glm::vec3 pos, float height, float maxDepth, float maxBranches)
{
    gluggPushMatrix();
    gluggTranslate(pos.x, pos.y, pos.z);

    CreateCylinder(20, height, 0.07, 0.14);

    MakeBranches(maxDepth, 0, height, maxBranches, 1.0);
    gluggPopMatrix();
}

void Forest::MakeBranches(const int maxDepth, int currentDepth, float currentHeight, int branches, float totalScale)
{
    //branches += rand() % (3 + 1 - 0) + 0;
    //currentHeight = currentHeight / 2;
    //currentBottonWidth = currentBottonWidth / 2;
    //currentTopWidth = currentTopWidth / 2;

    //Might want this, maybe not
    //if (currentDepth < maxDepth) {
    //    //Branch 1 has no angle
    //    gluggPushMatrix();
    //    gluggTranslate(0, currentHeight, 0);

    //    gluggScale(0.5, 0.5, 0.5);
    //    //totalScale *= 0.5;

    //    CreateCylinder(20, currentHeight, 0.07, 0.14);

    //    MakeBranches(maxDepth, currentDepth + 1, currentHeight, branches, totalScale * 0.5);
    //}

    //branches = random<int>(branches - 2, branches);
    
    for (int i = 0; i < branches; ++i) {
        if (currentDepth < maxDepth) {
            gluggPushMatrix();
            gluggTranslate(0, currentHeight, 0);
            
            //Might want scaling that is not uniform, --> requires vec3 scaling to be sent
            //Higher currentDepth -> lower scaling (higher scaling value)
            double randomScale = random<double>(0.4, 0.6);
            
            gluggScale(randomScale, randomScale, randomScale);
            //gluggScale(0.5, 0.5, 0.5);
            //gluggRotate((double)i * 2 * M_PI / branches, 0.0, 1.0, 0.0);
            gluggRotate(random<double>(0.0, 2 * M_PI), 0.0, 1.0, 0.0);

            float random_ = rand() % (7 + 1 - 3) + 3;
            gluggRotate(3.14 / random_, 0.0, 0.0, 1.0);

            CreateCylinder(20 / (currentDepth + 1), currentHeight, 0.07, 0.14);

            MakeBranches(maxDepth, currentDepth + 1, currentHeight, branches, totalScale * randomScale);
        }
        else {
            //Create a leaf position
            gluggPushMatrix();
            gluggTranslate(0, currentHeight + 0.05, 0);
            //gluggScale(pow(1.5, maxDepth), pow(1.5, maxDepth), pow(1.5, maxDepth));
            gluggScale(1 / totalScale, 1 / totalScale, 1 / totalScale);
            mat4 currentMatrix = gluggCurrentMatrix();

            //printMat4(currentMatrix);

            //The currentMatrix is stored columnwise in 4 vectors
            leafMatrixRow1->push_back(vec4{ currentMatrix.m[0], currentMatrix.m[4], currentMatrix.m[8], currentMatrix.m[12] });
            leafMatrixRow2->push_back(vec4{ currentMatrix.m[1], currentMatrix.m[5], currentMatrix.m[9], currentMatrix.m[13] });
            leafMatrixRow3->push_back(vec4{ currentMatrix.m[2], currentMatrix.m[6], currentMatrix.m[10], currentMatrix.m[14] });
            leafMatrixRow4->push_back(vec4{ currentMatrix.m[3], currentMatrix.m[7], currentMatrix.m[11], currentMatrix.m[15] });
            gluggPopMatrix();
            break;
        }
    }

    gluggPopMatrix();
}

void Forest::CreateCylinder(int aSlices, float height, float topwidth, float bottomwidth)
{
    gluggMode(GLUGG_TRIANGLE_STRIP);
    vec3 top = SetVector(0, height, 0);
    vec3 center = SetVector(0, 0, 0);
    vec3 bn = SetVector(0, -1, 0); // Bottom normal
    vec3 tn = SetVector(0, 1, 0); // Top normal

    for (float a = 0.0; a < 2.0 * M_PI + 0.0001; a += 2.0 * M_PI / aSlices)
    {
        float a1 = a;

        vec3 p1 = SetVector(topwidth * cos(a1), height, topwidth * sin(a1));
        vec3 p2 = SetVector(bottomwidth * cos(a1), 0, bottomwidth * sin(a1));
        vec3 pn = SetVector(cos(a1), 0, sin(a1));

        // Done making points and normals. Now create polygons!
        gluggNormalv(pn);
        gluggTexCoord(height, a1 / M_PI);
        gluggVertexv(p2);
        gluggTexCoord(0, a1 / M_PI);
        gluggVertexv(p1);
    }

    // Then walk around the top and bottom with fans
    gluggMode(GLUGG_TRIANGLE_FAN);
    gluggNormalv(bn);
    gluggVertexv(center);
    // Walk around edge
    for (float a = 0.0; a <= 2.0 * M_PI + 0.001; a += 2.0 * M_PI / aSlices)
    {
        vec3 p = SetVector(bottomwidth * cos(a), 0, bottomwidth * sin(a));
        gluggVertexv(p);
    }
    // Walk around edge
    gluggMode(GLUGG_TRIANGLE_FAN); // Reset to new fan
    gluggNormalv(tn);
    gluggVertexv(top);
    for (float a = 2.0 * M_PI; a >= -0.001; a -= 2.0 * M_PI / aSlices)
    {
        vec3 p = SetVector(topwidth * cos(a), height, topwidth * sin(a));
        gluggVertexv(p);
    }
}

void Forest::GenerateTerrain()
{
    /*std::vector<float>* terrainVertices = new std::vector<float>{
    -widthOfTerrain / 2, 0.0f, -depthOfTerrain / 2, 0.0f, 1.0f, 0.0f, 0.0f, 10.0f,
     widthOfTerrain / 2, 0.0f, -depthOfTerrain / 2, 0.0f, 1.0f, 0.0f, 10.0f, 10.0f,
     widthOfTerrain / 2, 0.0f,  depthOfTerrain / 2, 0.0f, 1.0f, 0.0f, 10.0f, 0.0f,
    -widthOfTerrain / 2, 0.0f,  depthOfTerrain / 2, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f
    };

    std::vector<unsigned int>* terrainIndices = new std::vector<unsigned int>{
       0, 3, 2,
       0, 2, 1
    };*/

    //Voronoi noise
    //Divide terrain into cells
    //Generate a random position (center) in each cell
    //Calculate distance from center to cell edge

    std::vector<float>* terrainVertices = new std::vector<float>();
    std::vector<unsigned int>* terrainIndices = new std::vector<unsigned int>();

    for (float x = 0; x < widthOfTerrain; ++x) {
        for (float z = 0; z < depthOfTerrain; ++z) {

            //Positions
            terrainVertices->push_back(x);
            terrainVertices->push_back(sin(x * z)); //This value will change later
            terrainVertices->push_back(z);

            //Normals (not really correct but works for now)
            terrainVertices->push_back(0.0f);
            terrainVertices->push_back(1.0f);
            terrainVertices->push_back(0.0f);

            //Texture coordinates
            terrainVertices->push_back(x);
            terrainVertices->push_back(z);
        }
    }

    for (float x = 0; x < widthOfTerrain; ++x) {
        for (float z = 0; z < depthOfTerrain; ++z) {
            // Indices
            terrainIndices->push_back(x + z * widthOfTerrain);
            terrainIndices->push_back(x + 1 + z * widthOfTerrain);
            terrainIndices->push_back(x + (z + 1) * widthOfTerrain);
            terrainIndices->push_back(x + 1 + z * widthOfTerrain);
            terrainIndices->push_back(x + 1 + (z + 1) * widthOfTerrain);
            terrainIndices->push_back(x + (z + 1) * widthOfTerrain);
        }
    }

    terrain = std::make_unique<Geometry>(terrainVertices, terrainIndices);
}
