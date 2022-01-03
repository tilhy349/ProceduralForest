#include "Forest.h"
#include <iostream>

Forest::Forest(unsigned int program, float width, float depth) : widthOfTerrain{width}, depthOfTerrain{ depth }
{

    GenerateTerrain();

    leafMatrixCol1 = new std::vector<vec3>();
    leafMatrixCol2 = new std::vector<vec3>();
    leafMatrixCol3 = new std::vector<vec3>();
    leafMatrixCol4 = new std::vector<vec3>();

    gluggSetPositionName("inPosition");
    gluggSetNormalName("inNormal");
    gluggSetTexCoordName("inTexCoord");

    gluggBegin(GLUGG_TRIANGLES);

    //TODO: Create function which places the randomly generated trees over the terrain

    const float widthOfPatch = widthOfTerrain / 5;
    const float depthOfPatch = depthOfTerrain / 5;

    //Divide terrain into rectangular patches, generate a random position in that patch. Spawn random tree
    for (float i = 0; i < widthOfTerrain; i += widthOfPatch) {
        for (float j = 0; j < depthOfTerrain; j += depthOfPatch) {
            float xPos = random<float>(i, i + widthOfPatch);
            float zPos = random<float>(j, j + depthOfPatch);

            AddTree(glm::vec3(xPos, 0.0f, zPos), 1.5, random<int>(3, 6), 3);
            //std::cout << "Spawning a tree at pos: (" << xPos << ", " << "0.0f, " << zPos << ")\n";
        }
    }

    //glm::vec3 pos(widthOfTerrain / 2, 0, depthOfTerrain / 4);
    //pos = glm::vec3(widthOfTerrain / 2 - 2, 0, depthOfTerrain / 4);
    float y = noise2(widthOfTerrain / 2 + 0.23, depthOfTerrain / 4 + 0.22);

    //AddTree(glm::vec3(widthOfTerrain/2, y * 2, depthOfTerrain / 4), 1.5, 5, 4);
    //AddTree(glm::vec3(widthOfTerrain / 2 - 2, 0, depthOfTerrain / 4), 2.0, 4, 5);
    //AddTree(glm::vec3(widthOfTerrain / 2 + 2, 0, depthOfTerrain / 4), 2.2, 6, 4);

    //int numberOfTrees = 10;
    //for(int i = 0; i < )
    
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
            leafMatrixCol1->push_back(vec3{ currentMatrix.m[0], currentMatrix.m[4], currentMatrix.m[8]});
            leafMatrixCol2->push_back(vec3{ currentMatrix.m[1], currentMatrix.m[5], currentMatrix.m[9]});
            leafMatrixCol3->push_back(vec3{ currentMatrix.m[2], currentMatrix.m[6], currentMatrix.m[10]});
            leafMatrixCol4->push_back(vec3{ currentMatrix.m[3], currentMatrix.m[7], currentMatrix.m[11]});
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

    //Voronoi noise
    //Divide terrain into cells
    //Generate a random position (center) in each cell
    //Calculate distance from center to cell edge

    std::vector<float>* terrainVertices = new std::vector<float>();
    std::vector<unsigned int>* terrainIndices = new std::vector<unsigned int>();

    std::vector<float> heightMapValues = std::vector<float>();
    float lowBound = 0;
    float highBound = 0;

    for (float x = 0; x < widthOfTerrain; x++) {
        for (float z = 0; z < depthOfTerrain; z++) {
            float amp = 1;
            float freq = 0.9;
            float y = 0;

            for (int i = 0; i < 5; i++) { //% octaves of noise
                //Generate height value
                float perlinValue = noise2(x * freq, z * freq);
                y += perlinValue * amp;
                //std::cout << "y = " << y << "\n";
                amp *= 0.5;
                freq *= 2;
            }

            //std::cout << "y = " << y << "\n";
            heightMapValues.push_back(y);

            if (y < lowBound)
                lowBound = y;
            else if (y > highBound)
                highBound = y;
        }
    }

    for (float x = 0; x < widthOfTerrain; x++) {
        for (float z = 0; z < depthOfTerrain; z++) {

            //Positions
            terrainVertices->push_back(x);           
            terrainVertices->push_back(heightMapValues[x * widthOfTerrain + z]); //Normalize this value
            terrainVertices->push_back(z);

            //Normals (not really correct but works for now)
            terrainVertices->push_back(0.0f);
            terrainVertices->push_back(1.0f);
            terrainVertices->push_back(0.0f);

            //Correct normals
            /*vec3 p1 = SetVec3(x + 1, noise2((x + 1) * 0.4, (z + 1) * 0.4), z + 1);
            vec3 p2 = SetVec3(x + 1, noise2((x + 1) * 0.4, (z - 1) * 0.4), z - 1);
            vec3 p3 = SetVec3(x - 1, noise2((x - 1) * 0.4, (z - 1) * 0.4), z - 1);
            vec3 p4 = SetVec3(x - 1, noise2((x - 1) * 0.4, (z + 1) * 0.4), z + 1);

            vec3 v1 = VectorSub(p1, p3);
            vec3 v2 = VectorSub(p2, p4);

            vec3 normal = CrossProduct(v1, v2);
            terrainVertices->push_back(normal.x);
            terrainVertices->push_back(normal.y);
            terrainVertices->push_back(normal.z);*/

            //Texture coordinates
            terrainVertices->push_back(x);
            terrainVertices->push_back(z);

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
