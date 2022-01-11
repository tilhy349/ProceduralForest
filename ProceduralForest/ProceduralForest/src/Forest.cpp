#include "Forest.h"
#include <iostream>

float generateFBMNoiseValue(float xPos, float zPos, int octaves, float scaling = 1) {
    float amp = 1;
    float freq = 0.9f;
    float totalNoiceVal = 0;

    for (int i = 0; i < octaves; i++) { //Octaves of FBM noise
        //Generate height value
        float noise = noise2(xPos * scaling * freq, zPos * scaling * freq);
        totalNoiceVal += noise * amp;
        amp *= 0.5;
        freq *= 2;
    }

    return totalNoiceVal;
}

Forest::Forest(unsigned int program, float width, float depth) : widthOfTerrain{width}, depthOfTerrain{ depth }, 
    verticesWidth{ (int)(widthOfTerrain / step) }, verticesDepth{ (int)(depthOfTerrain / step) }
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

    const float widthOfPatch = widthOfTerrain / 6;
    const float depthOfPatch = depthOfTerrain / 6;

    //Divide terrain into rectangular patches, generate a random position in that patch. Spawn random tree
    for (float i = 0; i < widthOfTerrain; i += widthOfPatch) {
        for (float j = 0; j < depthOfTerrain; j += depthOfPatch) {
            float xPos = random<float>(i, i + widthOfPatch);
            float zPos = random<float>(j, j + depthOfPatch);
            float yPos = (generateFBMNoiseValue(xPos, zPos, octaves, 0.5) - lowBound) / (highBound - lowBound) * verticalScale;

            AddTree(glm::vec3(xPos, yPos, zPos), random<float>(0.5, 1.3), random<int>(3, 6), random<int>(3, 4));
            //std::cout << "Spawning a tree at pos: (" << xPos << ", " << yPos << ", " << zPos << ")\n";
        }
    }

    m_RendererID = gluggEnd(&verticeCount, program, 0);

    GenerateLeaves();
}

Forest::~Forest()
{
    //TODO: Check how this works
    /*if (leafMatrixCol1 != nullptr) {
        delete leafMatrixCol1;
        delete leafMatrixCol2;
        delete leafMatrixCol3;
        delete leafMatrixCol4;
    }*/
    
}

void Forest::Render()
{
    glBindVertexArray(m_RendererID);	// Select VAO
    glDrawArrays(GL_TRIANGLES, 0, verticeCount);
}

void Forest::RenderLeaves()
{
    leafVAO->Bind();
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, numberOfInstances); //Render leaves
}

void Forest::GenerateLeaves() {

    numberOfInstances = leafMatrixCol1->size();

    instanceVBrow1 = std::make_unique<VertexBuffer>(static_cast<void*>(leafMatrixCol1->data()),
        numberOfInstances * 3 * sizeof(float));
    instanceVBrow2 = std::make_unique<VertexBuffer>(static_cast<void*>(leafMatrixCol2->data()),
        numberOfInstances * 3 * sizeof(float));
    instanceVBrow3 = std::make_unique<VertexBuffer>(static_cast<void*>(leafMatrixCol3->data()),
        numberOfInstances * 3 * sizeof(float));
    instanceVBrow4 = std::make_unique<VertexBuffer>(static_cast<void*>(leafMatrixCol4->data()),
        numberOfInstances * 3 * sizeof(float));

    FreeMatrixData();

    std::vector<float> leafVertices{
        -0.025f,  0.025f,  0.0f, 0.0f, 1.0f,
        -0.025f, -0.025f,  0.0f, 0.0f, 0.0f,
         0.025f, -0.025f,  0.0f, 1.0f, 0.0f,

        -0.025f,  0.025f,  0.0f, 0.0f, 1.0f,
         0.025f, -0.025f,  0.0f, 1.0f, 0.0f,
         0.025f,  0.025f,  0.0f, 1.0f, 1.0f
    };

    leafVAO = std::make_unique<VertexArray>();
    leafVB = std::make_unique<VertexBuffer>(static_cast<void*>(leafVertices.data()), leafVertices.size() * sizeof(float));

    VertexBufferLayout layoutLeaf;
    layoutLeaf.Push<float>(3);
    layoutLeaf.Push<float>(2);

    leafVAO->AddBuffer(*leafVB, layoutLeaf); //Sets vertexAttribs
    leafVAO->Bind();

    // also set instance data
    glEnableVertexAttribArray(2);
    instanceVBrow1->Bind(); // this attribute comes from a different vertex buffer
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    instanceVBrow1->UnBind();
    glVertexAttribDivisor(2, 1); // tell OpenGL this is an instanced vertex attribute.

    glEnableVertexAttribArray(3);
    instanceVBrow2->Bind(); // this attribute comes from a different vertex buffer
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    instanceVBrow2->UnBind();
    glVertexAttribDivisor(3, 1); // tell OpenGL this is an instanced vertex attribute.

    glEnableVertexAttribArray(4);
    instanceVBrow3->Bind(); // this attribute comes from a different vertex buffer
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    instanceVBrow3->UnBind();
    glVertexAttribDivisor(4, 1); // tell OpenGL this is an instanced vertex attribute.

    glEnableVertexAttribArray(5);
    instanceVBrow4->Bind(); // this attribute comes from a different vertex buffer
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    instanceVBrow4->UnBind();
    glVertexAttribDivisor(5, 1); // tell OpenGL this is an instanced vertex attribute.
}

void Forest::AddTree(const glm::vec3 pos, const float height, const int maxDepth, const int maxBranches)
{
    gluggPushMatrix();
    gluggTranslate(pos.x, pos.y, pos.z);

    CreateCylinder(16, height, height*0.035f, height*0.07f);

    MakeBranches(maxDepth, 0, height, maxBranches, 1.0);
    gluggPopMatrix();
}

void Forest::MakeBranches(const int maxDepth, int currentDepth, const float height, const int branches, float totalScale)
{
    
    for (int i = 0; i < branches; ++i) {
        if (currentDepth < maxDepth) {
            gluggPushMatrix();
            gluggTranslate(0, height, 0);
            
            //Might want scaling that is not uniform, --> requires vec3 scaling to be sent
            //Higher currentDepth -> lower scaling (higher scaling value)
            float randomScale = random<float>(0.4f, 0.6f);
            
            gluggScale(randomScale, randomScale, randomScale);
            gluggRotate(random<float>(0.0f, (float)(2.0f * M_PI)), 0.0, 1.0, 0.0);

            int random_ = rand() % (7 + 1 - 3) + 3;
            gluggRotate(3.14f / random_, 0.0, 0.0, 1.0);

            CreateCylinder(16 / (currentDepth + 1), height, height * 0.035f, height * 0.07f);

            MakeBranches(maxDepth, currentDepth + 1, height, branches, totalScale * randomScale);
        }
        else {
            //Create a leaf position
            gluggPushMatrix();
            gluggTranslate(0, height + height * 0.025f, 0);
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

    for (float a = 0.0; a < 2.0 * M_PI + 0.0001; a += (float)(2.0f * M_PI / aSlices))
    {
        float a1 = a;

        vec3 p1 = SetVector(topwidth * cos(a1), height, topwidth * sin(a1));
        vec3 p2 = SetVector(bottomwidth * cos(a1), 0, bottomwidth * sin(a1));
        vec3 pn = SetVector(cos(a1), 0, sin(a1));

        // Done making points and normals. Now create polygons!
        gluggNormalv(pn);
        gluggTexCoord(height, (float)(a1 / M_PI));
        gluggVertexv(p2);
        gluggTexCoord(0, (float)(a1 / M_PI));
        gluggVertexv(p1);
    }

    // Then walk around the top and bottom with fans
    gluggMode(GLUGG_TRIANGLE_FAN);
    gluggNormalv(bn);
    gluggVertexv(center);
    // Walk around edge
    for (float a = 0.0; a <= 2.0 * M_PI + 0.001; a += (float)(2.0 * M_PI / aSlices))
    {
        vec3 p = SetVector(bottomwidth * cos(a), 0, bottomwidth * sin(a));
        gluggVertexv(p);
    }
    // Walk around edge
    gluggMode(GLUGG_TRIANGLE_FAN); // Reset to new fan
    gluggNormalv(tn);
    gluggVertexv(top);
    for (float a = (float)(2.0f * M_PI); a >= -0.001f; a -= (float)(2.0f * M_PI / aSlices))
    {
        vec3 p = SetVector(topwidth * cos(a), height, topwidth * sin(a));
        gluggVertexv(p);
    }
}

void Forest::GenerateTerrain()
{
    std::vector<float>* terrainVertices = new std::vector<float>();
    std::vector<unsigned int>* terrainIndices = new std::vector<unsigned int>();

    std::vector<float> heightMapValues = std::vector<float>();
   
    //Construct height map using perlin noise
    for (float x = 0; x < widthOfTerrain; x += step) {
        for (float z = 0; z < depthOfTerrain; z += step) {
 
            float y = generateFBMNoiseValue(x, z, octaves, 0.5);
            heightMapValues.push_back(y);

            if (y < lowBound)
                lowBound = y;
            else if (y > highBound)
                highBound = y;
        }
    }

    for (int x = 0; x < verticesWidth; x++) {
        for (int z = 0; z < verticesDepth; z++) {

            //Normalized height value (0 - 1) 
            //Scale this value appropiately 
            float y = (heightMapValues[x * verticesWidth + z] - lowBound) / (highBound - lowBound);
            heightMapValues[x * verticesWidth + z] = y;
            //Positions
            terrainVertices->push_back(x * step);           
            terrainVertices->push_back(y * verticalScale); 
            terrainVertices->push_back(z * step);

            //Correct normals
            y = (generateFBMNoiseValue(x * step + step, z * step + step, octaves, 0.5) - lowBound) / (highBound - lowBound);
            vec3 p1 = SetVec3(x * step + step, y, z * step + step);
            y = (generateFBMNoiseValue(x * step + step, z * step - step, octaves, 0.5) - lowBound) / (highBound - lowBound);
            vec3 p2 = SetVec3(x * step + step, y, z * step - step);
            y = (generateFBMNoiseValue(x * step - step, z * step - step, octaves, 0.5) - lowBound) / (highBound - lowBound);
            vec3 p3 = SetVec3(x * step - step, y, z * step - step);
            y = (generateFBMNoiseValue(x * step - step, z * step + step, octaves, 0.5) - lowBound) / (highBound - lowBound);
            vec3 p4 = SetVec3(x * step - step, y, z * step + step);

            vec3 v1 = VectorSub(p1, p3);
            vec3 v2 = VectorSub(p2, p4);

            vec3 normal = CrossProduct(v1, v2);
            terrainVertices->push_back(normal.x);
            terrainVertices->push_back(normal.y);
            terrainVertices->push_back(normal.z);

            //Texture coordinates
            terrainVertices->push_back((float)x);
            terrainVertices->push_back((float)z);

            //Construct and store indices
            if (x < verticesWidth - 1 && z < verticesDepth - 1) {
                terrainIndices->push_back(x + z * verticesDepth);
                terrainIndices->push_back(x + 1 + z * verticesDepth);
                terrainIndices->push_back(x + (z + 1) * verticesDepth);
                terrainIndices->push_back(x + 1 + z * verticesDepth);
                terrainIndices->push_back(x + 1 + (z + 1) * verticesDepth);
                terrainIndices->push_back(x + (z + 1) * verticesDepth);
            }
        }
    }

    terrain = std::make_unique<Geometry>(terrainVertices, terrainIndices);

    //Clean up - these are stored the in geometry class
    delete terrainVertices;
    delete terrainIndices;
}


