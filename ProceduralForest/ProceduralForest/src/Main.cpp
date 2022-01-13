// Procedural Forest with Seasons
// Developed for the course 
// TNM084 - Procedural Images at Linköping University
// By Tilda Hylander
// Navigate in the window using the keys
// 'w', 'a', 's' and 'd' to translate camera
// and move the mouse cursor to rotate the view.
// Press the key '1' to switch seasons and '2' to change shader on the trees

#include <GL/glew.h> //Needs to be included before other OpenGL libraries

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "Forest.h"
#include "WindowManager.h"

#include <iostream>
#include <vector>

const unsigned int width = 1080;
const unsigned int height = 1080;

const float widthOfTerrain = 8.0f;
const float depthOfTerrain = 8.0f;

const float seasonDuration = 26.0f;

int main(void)
{
    WindowManager window(width, height);

    //Set up glew
    if (glewInit() != GLEW_OK)
        std::cout << "Error with glew \n";

    //Set up glfw window
    if (window.CheckState() == -1)
        return -1;

    SeasonHandler* seasonManager = &window.seasonsManager;
    
    glEnable(GL_DEPTH_TEST); //Render things according to depth
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND); //Transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);         

    glm::mat4 model = glm::mat4(1.0f); //Reset model matrix  

    //Variables for season handling, might move these to windowHandler
    float time = 0.0f;
    float accelaration = -0.05f;
    float accSnow = -0.5f;
    float updatedYPos = 0.0f;
    float currentVelocity = 0.0f;
    bool startFall = true;

    float winter = 0.0f;
    float summer = 0.0f;

    float timeSeasonStart = 0.0f; //Time passed since season begun
    float timeSeasonCurrent = seasonManager->GetSeasonTime();

    //Load all shaders need for the program
    Shader shader("res/shaders/textured.vert", "res/shaders/textured.frag");
    Shader shaderPhong("res/shaders/phong.vert", "res/shaders/phong.frag");
    Shader shaderLeaf("res/shaders/leaf.vert", "res/shaders/leaf.frag");
    Shader shaderSnow("res/shaders/snowflake.vert", "res/shaders/snowflake.frag");
    Shader shaderTree("res/shaders/tree.vert", "res/shaders/tree.frag");

    //Load all textures needed for the program
    Texture textureGrass("res/textures/grass.png");
    Texture textureBark("res/textures/tree.png");
    Texture textureLeaf("res/textures/leaf2.png");
    Texture textureSnow("res/textures/snow.png");

    //Set uniforms for all shaders
    shader.Bind();
    shader.SetUniformMat4f("projectionMatrix", window.proj);
    shader.SetUniformMat4f("modelviewMatrix", window.viewMatrix * model);
    shader.SetUniform1i("tex", 0);

    shaderTree.Bind();
    shaderTree.SetUniformMat4f("projectionMatrix", window.proj);
    shaderTree.SetUniformMat4f("modelviewMatrix", window.viewMatrix * model);
        
    shaderPhong.Bind();
    shaderPhong.SetUniformMat4f("projectionMatrix", window.proj);
    shaderPhong.SetUniformMat4f("modelviewMatrix", window.viewMatrix * model);
    shaderPhong.SetUniform1f("winter", winter);
    shaderPhong.SetUniform1f("summer", summer);

    shaderLeaf.Bind();
    shaderLeaf.SetUniformMat4f("projectionMatrix", window.proj);
    shaderLeaf.SetUniformMat4f("viewMatrix", window.viewMatrix);
    shaderLeaf.SetUniformMat4f("modelMatrix", model);
    shaderLeaf.SetUniform1f("updatedYPos", updatedYPos);
    shaderLeaf.SetUniform1f("time", timeSeasonCurrent);
    shaderLeaf.SetUniform1i("u_Texture", 0);

    shaderSnow.Bind();
    shaderSnow.SetUniformMat4f("projectionMatrix", window.proj);
    shaderSnow.SetUniformMat4f("viewMatrix", window.viewMatrix);
    shaderSnow.SetUniform1i("u_Texture", 0);
    shaderSnow.SetUniform1f("updatedYPos", updatedYPos);
        
    textureBark.Bind();

    Forest theForest(shader.GetRendererID(), widthOfTerrain, depthOfTerrain);

    Renderer renderer;

    //Vertices for the snowflake
    std::vector<float> snowflakeVertices{
        -0.025f,  0.025f,  0.0f, 0.0f, 1.0f,
        -0.025f, -0.025f,  0.0f, 0.0f, 0.0f,
         0.025f, -0.025f,  0.0f, 1.0f, 0.0f,

        -0.025f,  0.025f,  0.0f, 0.0f, 1.0f,
         0.025f, -0.025f,  0.0f, 1.0f, 0.0f,
         0.025f,  0.025f,  0.0f, 1.0f, 1.0f
    };

    //Generate positions
    std::vector<vec3> snowflakePositions;
    const float widthOfPatch = widthOfTerrain / 20;
    const float depthOfPatch = depthOfTerrain / 20;

    //Divide terrain into rectangular patches, generate a random position in that patch. 
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

    //Set the buffer layout
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
         
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window.GetWindow()))
    {
        time = (float)glfwGetTime();

        if (seasonManager->GetSeasonState()) { //If this is the beginning of a new season, reset season time
            seasonManager->SetSeasonState(false);
            timeSeasonStart = time;
            seasonManager->SetSeasonTime(0.0f);
        }
        else
            seasonManager->SetSeasonTime(time - timeSeasonStart); //Update season time
            
        /* Render here */
        renderer.Clear();

        //Reset values
        model = glm::mat4(1.0f);
        float degree = 0.0f;
        updatedYPos = 0.0f;
        winter = 0.0f;
        summer = 0.0f;
        timeSeasonCurrent = seasonManager->GetSeasonTime();
            
        //---- Render terrain -----
        textureGrass.Bind();

        shaderPhong.Bind();
        shaderPhong.SetUniformMat4f("modelviewMatrix", window.viewMatrix * model);

        //---- Render forest -----
        //Render terrain      
        theForest.terrain->Render(shaderPhong); 

        //Render trees
        if (window.treeTextureMode == 0) {
            shaderTree.Bind();
            shaderTree.SetUniformMat4f("modelviewMatrix", window.viewMatrix * model);
        }
        else {
            shader.Bind();
            shader.SetUniformMat4f("modelviewMatrix", window.viewMatrix * model);
            textureBark.Bind();
        }     
        
        theForest.Render(); 

        //Render leaves
        glDepthMask(GL_FALSE); //Disable writing into the depth buffer 

        textureLeaf.Bind();
        shaderLeaf.Bind();

        switch (seasonManager->GetSeason()) {
            case Season::Winter:
                winter = 0.5f * sin(timeSeasonCurrent * (float)M_PI / (seasonDuration/2) + 3 * (float)M_PI / 2) + 0.5f;
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
                    shaderLeaf.SetUniform1f("time", timeSeasonCurrent); 

                model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f) * 
                    0.5f * sin(timeSeasonCurrent * (float)M_PI / seasonDuration + 3 * (float)M_PI / 2) + 0.5f);
                break;
            case Season::Summer:
                summer = 0.5f * sin(timeSeasonCurrent * (float)M_PI / (seasonDuration/2) + 3 * (float)M_PI / 2) + 0.5f;
                    
                break;
            case Season::Fall:
                if (timeSeasonCurrent < seasonDuration / 2) {
                    shaderLeaf.SetUniform1f("time", timeSeasonCurrent);
                    startFall = true;
                }                      
                else {
                    if (startFall) {
                        currentVelocity = 0.0; //Reset velocity to resting state
                        startFall = false;
                    }
                            
                    //Update yPosition of the leaves to simulate falling 
                    //Update currentVelocity according to accelaration
                    if (timeSeasonCurrent > seasonDuration - 4.0f)
                        accelaration -= 0.0005f;
                    updatedYPos = currentVelocity * (timeSeasonCurrent - seasonDuration / 2);
                    currentVelocity = accelaration * (timeSeasonCurrent - seasonDuration / 2);
                }
                break;
        }

        shaderLeaf.SetUniform1f("updatedYPos", updatedYPos);
        shaderLeaf.SetUniformMat4f("viewMatrix", window.viewMatrix);
        shaderLeaf.SetUniformMat4f("modelMatrix", model);
            
        theForest.RenderLeaves();

        glDepthMask(GL_TRUE); //Enable writing into the depth buffer

        //If winter season, render snowflakes
        if (seasonManager->GetSeason() == Season::Winter && timeSeasonCurrent < seasonDuration - 6.0f) {
                
            textureSnow.Bind();
            shaderSnow.Bind();
            shaderSnow.SetUniform1f("updatedYPos", updatedYPos);
            shaderSnow.SetUniformMat4f("viewMatrix", window.viewMatrix);
            snowflakeVAO->Bind();
            glDrawArraysInstanced(GL_TRIANGLES, 0, 6, numberOfSnowflakes); //Render snowflakes
        }

        shaderPhong.Bind();
        shaderPhong.SetUniform1f("winter", winter);
        shaderPhong.SetUniform1f("summer", summer);

        //If time has surpassed a certain mark --> change season
        if (timeSeasonCurrent > seasonDuration) 
            seasonManager->UpdateSeason();

        /* Swap front and back buffers */
        glfwSwapBuffers(window.GetWindow());

        /* Poll for and process events */
        glfwPollEvents();  
    }

    

    glfwTerminate();
    return 0;
}