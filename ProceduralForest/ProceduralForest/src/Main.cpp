
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

enum Seasons { Winter, Spring, Summer, Fall };
int currentSeason = 0;

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

        glm::mat4 model = glm::mat4(1.0f);

        //Shader shader("res/shaders/Basic.vert", "res/shaders/Basic.frag");
        Shader shader("res/shaders/textured.vert", "res/shaders/textured.frag");
        shader.Bind();

        shader.SetUniformMat4f("projectionMatrix", window.proj);
        shader.SetUniformMat4f("modelviewMatrix", window.viewMatrix * model);
        shader.SetUniform1i("tex", 0);

        Shader shaderPhong("res/shaders/phong.vert", "res/shaders/phong.frag");
        shaderPhong.Bind();

        shaderPhong.SetUniformMat4f("projectionMatrix", window.proj);
        //shaderPhong.SetUniformMat4f("modelviewMatrix", view * model);

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

        std::vector<float>* leafVertices = new std::vector<float>{
            -0.05f,  0.05f,  0.0f, 0.0f, 1.0f,
            -0.05f, -0.05f,  0.0f, 0.0f, 0.0f,
             0.05f, -0.05f,  0.0f, 1.0f, 0.0f, 

            -0.05f,  0.05f,  0.0f, 0.0f, 1.0f,
             0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
             0.05f,  0.05f,  0.0f, 1.0f, 1.0f
        };
        
        std::unique_ptr<VertexArray> leafVAO = std::make_unique<VertexArray>();
        VertexBuffer leafVB(static_cast<void*>(leafVertices->data()), leafVertices->size() * sizeof(float));

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

        float time = 0;

        Shader shaderLeaf("res/shaders/leaf.vert", "res/shaders/leaf.frag");
        shaderLeaf.Bind();
        shaderLeaf.SetUniformMat4f("projectionMatrix", window.proj);
        //shaderLeaf.SetUniformMat4f("modelviewMatrix", view * model);
        shaderLeaf.SetUniform1f("time", time);
        shaderLeaf.SetUniform1i("season", currentSeason);
        shaderLeaf.SetUniform1i("u_Texture", 0);       

        //float 
        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window.GetWindow()))
        {
            time = (float)glfwGetTime();
            //std::cout << "TIME: " << time << "\n";
            /* Render here */
            renderer.Clear();

            //----Render ground-----
            textureGrass.Bind();

            shaderPhong.Bind();
            //model = glm::scale(model, glm::vec3(4, 4, 4));
            shaderPhong.SetUniformMat4f("modelviewMatrix", window.viewMatrix * model);

            //renderer.DrawModel(*ground, shader);
            //renderer.Draw(*m_VAO, *m_IndexBuffer, shader);

            //renderer.Clear();

            //----Render forest-----
            theForest.terrain->Render(shaderPhong); //Render terrain

            textureBark.Bind();
            shader.Bind();
            model = glm::mat4(1.0f);
            shader.SetUniformMat4f("modelviewMatrix", window.viewMatrix * model);

            theForest.Render(); //Render trees

            //----Render leaves-----
            glDepthMask(GL_FALSE); //Disable writing into the depth buffer (not really sure why this is needed)

            textureLeaf.Bind();
            shaderLeaf.Bind();
            
            float degree = (float)fmod(time * M_PI / 10, 2 * M_PI);
            if (degree < M_PI/2 || degree > 3*M_PI/2) {
                shaderLeaf.SetUniform1f("time", time);
            }
            
            shaderLeaf.SetUniform1i("season", currentSeason);
            
            shaderLeaf.SetUniformMat4f("modelviewMatrix", window.viewMatrix * model);
            leafVAO->Bind();
            glDrawArraysInstanced(GL_TRIANGLES, 0, 6, numberOfInstances); //Render leaves

            glDepthMask(GL_TRUE); //Enable writing into the depth buffer

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