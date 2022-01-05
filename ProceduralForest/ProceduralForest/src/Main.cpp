
#include <GL/glew.h> //Needs to be included before other OpenGL libraries
//#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "Forest.h"
#include "WindowManager.h"

#include <iostream>
#include <vector>

const unsigned int width = 800;
const unsigned int height = 800;

const float widthOfTerrain = 10.0f;
const float depthOfTerrain = 10.0f;

int main(void)
{
    WindowManager window(width, height);

    if (glewInit() != GLEW_OK)
        std::cout << "Error with glew \n";

    if (window.CheckState() == -1)
        return -1;

    {
        glEnable(GL_DEPTH_TEST); //Render things according to depth
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND); //Transparency
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);         

        glm::mat4 resetScale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
        glm::mat4 scalingMat = resetScale;
        //glm::mat4 resetTranslation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f));
        glm::mat4 model = glm::mat4(1.0f);

        //Shader shader("res/shaders/Basic.vert", "res/shaders/Basic.frag");
        Shader shader("res/shaders/textured.vert", "res/shaders/textured.frag");
        shader.Bind();

        shader.SetUniformMat4f("projectionMatrix", window.proj);
        shader.SetUniformMat4f("modelviewMatrix", window.viewMatrix * model);
        shader.SetUniform1i("tex", 0);

        Shader shaderPhong("res/shaders/phong.vert", "res/shaders/phong.frag");
        shaderPhong.Bind();

        float winter = 0.0f;

        shaderPhong.SetUniformMat4f("projectionMatrix", window.proj);
        shaderPhong.SetUniform1f("winter", winter);

        Texture textureGrass("res/textures/grass.png");
        Texture textureBark("res/textures/bark.png");
        Texture textureLeaf("res/textures/leaf2.png");
        textureBark.Bind();

        Forest theForest(shader.GetRendererID(), widthOfTerrain, depthOfTerrain);
        //std::cout << "number of leaves " << theForest.leafPositions.size();

        Renderer renderer;
        
        //Test with own classes
        //TODO: REMOVE THIS FROM MAIN INTO A FUNCTION
        int numberOfInstances = theForest.leafMatrixCol1->size();

        VertexBuffer instanceVBrow1(static_cast<void*>(theForest.leafMatrixCol1->data()),
            numberOfInstances * 3 * sizeof(float));
        VertexBuffer instanceVBrow2(static_cast<void*>(theForest.leafMatrixCol2->data()),
            numberOfInstances * 3 * sizeof(float));
        VertexBuffer instanceVBrow3(static_cast<void*>(theForest.leafMatrixCol3->data()),
            numberOfInstances * 3 * sizeof(float));
        VertexBuffer instanceVBrow4(static_cast<void*>(theForest.leafMatrixCol4->data()),
            numberOfInstances * 3 * sizeof(float));

        theForest.FreeMatrixData();

        std::vector<float> leafVertices{
            -0.025f,  0.025f,  0.0f, 0.0f, 1.0f,
            -0.025f, -0.025f,  0.0f, 0.0f, 0.0f,
             0.025f, -0.025f,  0.0f, 1.0f, 0.0f, 

            -0.025f,  0.025f,  0.0f, 0.0f, 1.0f,
             0.025f, -0.025f,  0.0f, 1.0f, 0.0f,
             0.025f,  0.025f,  0.0f, 1.0f, 1.0f
        };
        
        std::unique_ptr<VertexArray> leafVAO = std::make_unique<VertexArray>();
        VertexBuffer leafVB(static_cast<void*>(leafVertices.data()), leafVertices.size() * sizeof(float));

        VertexBufferLayout layoutLeaf;
        layoutLeaf.Push<float>(3);
        layoutLeaf.Push<float>(2);

        leafVAO->AddBuffer(leafVB, layoutLeaf); //Sets vertexAttribs
        leafVAO->Bind();

        // also set instance data
        glEnableVertexAttribArray(2);
        instanceVBrow1.Bind(); // this attribute comes from a different vertex buffer
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        instanceVBrow1.UnBind();
        glVertexAttribDivisor(2, 1); // tell OpenGL this is an instanced vertex attribute.

        glEnableVertexAttribArray(3);
        instanceVBrow2.Bind(); // this attribute comes from a different vertex buffer
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        instanceVBrow2.UnBind();
        glVertexAttribDivisor(3, 1); // tell OpenGL this is an instanced vertex attribute.

        glEnableVertexAttribArray(4);
        instanceVBrow3.Bind(); // this attribute comes from a different vertex buffer
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        instanceVBrow3.UnBind();
        glVertexAttribDivisor(4, 1); // tell OpenGL this is an instanced vertex attribute.

        glEnableVertexAttribArray(5);
        instanceVBrow4.Bind(); // this attribute comes from a different vertex buffer
        glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        instanceVBrow4.UnBind();
        glVertexAttribDivisor(5, 1); // tell OpenGL this is an instanced vertex attribute.

        float time = 0.0f;
        float accelaration = -0.05f;
        float updatedYPos = 0.0f;
        float currentVelocity = 0.0f;
        float beginFall = 0.0f;
        float beginSpring = 0.0f;

        SeasonHandler* seasonManager = &window.seasonsManager;

        //Variables for season handling, might move these to windowHandler
        float timeSeasonStart = 0.0f; //Time passed since season begun
        float timeSeasonCurrent = seasonManager->GetSeasonTime();

        Shader shaderLeaf("res/shaders/leaf.vert", "res/shaders/leaf.frag");
        shaderLeaf.Bind();
        shaderLeaf.SetUniformMat4f("projectionMatrix", window.proj);
        shaderLeaf.SetUniformMat4f("viewMatrix", window.viewMatrix);
        shaderLeaf.SetUniformMat4f("modelMatrix", model);
        shaderLeaf.SetUniform1f("updatedYPos", updatedYPos);

        shaderLeaf.SetUniform1f("time", timeSeasonCurrent);
        shaderLeaf.SetUniform1i("u_Texture", 0);     

        

        //float 
        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window.GetWindow()))
        {
            time = (float)glfwGetTime();

            if (seasonManager->GetSeasonState()) {
                seasonManager->SetSeasonState(false);
                timeSeasonStart = time;
                seasonManager->SetSeasonTime(0.0f);
            }
            else
                seasonManager->SetSeasonTime(time - timeSeasonStart);
            
            /* Render here */
            renderer.Clear();

            //---- Render terrain -----
            textureGrass.Bind();

            shaderPhong.Bind();
            //model = glm::scale(model, glm::vec3(4, 4, 4));
            model = glm::mat4(1.0f);
            shaderPhong.SetUniformMat4f("modelviewMatrix", window.viewMatrix * model);

            //---- Render forest -----
            theForest.terrain->Render(shaderPhong); //Render terrain

            textureBark.Bind();
            shader.Bind();
            model = glm::mat4(1.0f);
            //model = glm::scale(model, glm::vec3(2, 2, 2));
            shader.SetUniformMat4f("modelviewMatrix", window.viewMatrix * model);

            theForest.Render(); //Render trees

            //---- Render leaves -----
            glDepthMask(GL_FALSE); //Disable writing into the depth buffer (not really sure why this is needed)

            model = glm::mat4(1.0f);
            float degree = 0.0f;
            updatedYPos = 0.0f;
            winter = 0.0f;
            timeSeasonCurrent = seasonManager->GetSeasonTime();
            textureLeaf.Bind();
            shaderLeaf.Bind();

            switch (seasonManager->GetSeason()) {
                case Season::Winter:
                    winter = 0.5f * sin(timeSeasonCurrent * (float)M_PI / 9.0f + 3 * (float)M_PI / 2) + 0.5f;
                    model = glm::scale(model, glm::vec3(0.0, 0.0, 0.0));
                    break;
                case Season::Spring:
                    if(timeSeasonCurrent == 0)
                        shaderLeaf.SetUniform1f("time", timeSeasonCurrent); //TODO: Figure out why this isnt working

                    model = glm::scale(model, glm::vec3(0.9f, 0.9f, 0.9f) * 
                        0.5f * sin(timeSeasonCurrent * (float)M_PI / 18.0f + 3 * (float)M_PI / 2) + 0.5f);
                    break;
                case Season::Summer:
                    shaderLeaf.SetUniform1f("time", timeSeasonCurrent);

                    break;
                case Season::Fall:
                    if (timeSeasonCurrent == 0)
                        currentVelocity = 0.0; //Reset velocity to resting state
                       
                    //Update yPosition of the leaves to simulate falling 
                    //Update currentVelocity according to accelaration
                    updatedYPos = currentVelocity * timeSeasonCurrent;
                    currentVelocity = accelaration * timeSeasonCurrent;
                    break;
            }

            shaderLeaf.SetUniform1f("updatedYPos", updatedYPos);
            shaderLeaf.SetUniformMat4f("viewMatrix", window.viewMatrix);
            shaderLeaf.SetUniformMat4f("modelMatrix", model);
            leafVAO->Bind();
            glDrawArraysInstanced(GL_TRIANGLES, 0, 6, numberOfInstances); //Render leaves

            shaderPhong.Bind();
            shaderPhong.SetUniform1f("winter", winter);

            glDepthMask(GL_TRUE); //Enable writing into the depth buffer

            //If time has surpassed a certain mark --> change season
            if (seasonManager->GetSeasonTime() > 18.0f) //Change this value to your liking
                seasonManager->UpdateSeason();

            /* Swap front and back buffers */
            glfwSwapBuffers(window.GetWindow());

            /* Poll for and process events */
            glfwPollEvents();  
        }

    }

    // Cleanup

    glfwTerminate();
    return 0;
}