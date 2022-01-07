
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
        Texture textureSnow("res/textures/snow.png");
        textureBark.Bind();

        Forest theForest(shader.GetRendererID(), widthOfTerrain, depthOfTerrain);
        //std::cout << "number of leaves " << theForest.leafPositions.size();

        Renderer renderer;
        
        //Test with own classes

        //Snow
        std::vector<float> snowflakeVertices{
            -0.05f,  0.05f,  0.0f, 0.0f, 1.0f,
            -0.05f, -0.05f,  0.0f, 0.0f, 0.0f,
             0.05f, -0.05f,  0.0f, 1.0f, 0.0f,

            -0.05f,  0.05f,  0.0f, 0.0f, 1.0f,
             0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
             0.05f,  0.05f,  0.0f, 1.0f, 1.0f
        };

        //Generate positions
        std::vector<vec3> snowflakePositions;
        const float widthOfPatch = widthOfTerrain / 12;
        const float depthOfPatch = depthOfTerrain / 12;

        //Divide terrain into rectangular patches, generate a random position in that patch. Spawn random snowflake
        for (float i = 0; i < widthOfTerrain; i += widthOfPatch) {
            for (float j = 0; j < depthOfTerrain; j += depthOfPatch) {
                float xPos = random<float>(i, i + widthOfPatch);
                float zPos = random<float>(j, j + depthOfPatch);

                snowflakePositions.push_back(vec3{ xPos, 3.0, zPos });
                snowflakePositions.push_back(vec3{ xPos, 4.5, zPos });
            }
        }

        int numberOfSnowflakes = snowflakePositions.size();

        VertexBuffer instanceSnowPos(static_cast<void*>(snowflakePositions.data()),
            numberOfSnowflakes * 3 * sizeof(float));

        std::unique_ptr<VertexArray> snowflakeVAO = std::make_unique<VertexArray>();
        VertexBuffer snowflakeVB(static_cast<void*>(snowflakeVertices.data()), snowflakeVertices.size() * sizeof(float));

        VertexBufferLayout layoutSnowflake;
        layoutSnowflake.Push<float>(3);
        layoutSnowflake.Push<float>(2);

        snowflakeVAO->AddBuffer(snowflakeVB, layoutSnowflake); //Sets vertexAttribs
        snowflakeVAO->Bind();

        // also set instance data
        glEnableVertexAttribArray(2);
        instanceSnowPos.Bind(); // this attribute comes from a different vertex buffer
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        instanceSnowPos.UnBind();
        glVertexAttribDivisor(2, 1); // tell OpenGL this is an instanced vertex attribute.

        SeasonHandler* seasonManager = &window.seasonsManager;

        //Variables for season handling, might move these to windowHandler
        float time = 0.0f;
        float accelaration = -0.05f;
        float accSnow = -0.5f;
        float updatedYPos = 0.0f;
        float currentVelocity = 0.0f;
       
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

        Shader shaderSnow("res/shaders/snowflake.vert", "res/shaders/snowflake.frag");
        shaderSnow.Bind();
        shaderSnow.SetUniformMat4f("projectionMatrix", window.proj);
        shaderSnow.SetUniformMat4f("viewMatrix", window.viewMatrix);
        shaderSnow.SetUniform1i("u_Texture", 0);
        shaderSnow.SetUniform1f("updatedYPos", updatedYPos);

        ////Test with rendering to texture
        //unsigned int frameBuffer = 0;
        //glGenFramebuffers(1, &frameBuffer);
        //glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

        //// The texture we're going to render to //Might be able to use texture-class
        //unsigned int renderedTexture;
        //glGenTextures(1, &renderedTexture);

        //// "Bind" the newly created texture : all future texture functions will modify this texture
        //glBindTexture(GL_TEXTURE_2D, renderedTexture);

        //// Give an empty image to OpenGL ( the last "0" )
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 768, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

        //// Poor filtering. Needed !
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        //// The depth buffer
        //unsigned int depthrenderbuffer;
        //glGenRenderbuffers(1, &depthrenderbuffer);
        //glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
        //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 768);
        //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

        //// Set "renderedTexture" as our colour attachement #0
        //glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

        //// Set the list of draw buffers.
        //GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
        //glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
         
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

            //Reset values
            model = glm::mat4(1.0f);
            float degree = 0.0f;
            updatedYPos = 0.0f;
            winter = 0.0f;
            timeSeasonCurrent = seasonManager->GetSeasonTime();
            
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
            //model = glm::scale(model, glm::vec3(2, 2, 2));
            shader.SetUniformMat4f("modelviewMatrix", window.viewMatrix * model);

            theForest.Render(); //Render trees

            //---- Render leaves -----
            glDepthMask(GL_FALSE); //Disable writing into the depth buffer (not really sure why this is needed)

            textureLeaf.Bind();
            shaderLeaf.Bind();

            switch (seasonManager->GetSeason()) {
                case Season::Winter:
                    winter = 0.5f * sin(timeSeasonCurrent * (float)M_PI / 9.0f + 3 * (float)M_PI / 2) + 0.5f;
                    model = glm::scale(model, glm::vec3(0.0, 0.0, 0.0));
                    if (timeSeasonCurrent == 0)
                        currentVelocity = 0.0; //Reset velocity to resting state

                    //Update yPosition of the leaves to simulate falling 
                    //Update currentVelocity according to accelaration
                    updatedYPos = currentVelocity * timeSeasonCurrent;
                    currentVelocity = accSnow * timeSeasonCurrent;
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
            
            //Render leaves
            theForest.RenderLeaves();

            glDepthMask(GL_TRUE); //Enable writing into the depth buffer

            if (seasonManager->GetSeason() == Season::Winter && timeSeasonCurrent < 12.0f) {
                //TODO: Create and bind the right shader
                textureSnow.Bind();
                shaderSnow.Bind();
                shaderSnow.SetUniform1f("updatedYPos", updatedYPos);
                shaderSnow.SetUniformMat4f("viewMatrix", window.viewMatrix);
                snowflakeVAO->Bind();
                glDrawArraysInstanced(GL_TRIANGLES, 0, 6, numberOfSnowflakes); //Render snowflakes
            }

            shaderPhong.Bind();
            shaderPhong.SetUniform1f("winter", winter);

            //If time has surpassed a certain mark --> change season
            if (timeSeasonCurrent > 18.0f) //Change this value to your liking
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