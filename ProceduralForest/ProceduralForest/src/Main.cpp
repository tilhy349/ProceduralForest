
#include <GL/glew.h> //Needs to be included before other OpenGL libraries
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "Geometry.h"
#include "Forest.h"

#include <iostream>
#include <vector>

const unsigned int width = 800;
const unsigned int height = 800;

GLFWwindow* init() {
    GLFWwindow* window;
    const GLFWvidmode* vidmode;  // GLFW struct to hold information about the display

    /* Initialize the library */
    if (!glfwInit())
        return nullptr;

    //Version 3.3 and profile to core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    // Determine the desktop size
    vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    // Open a square window (aspect 1:1) to fill half the screen height
    //window = glfwCreateWindow(vidmode->height / 2, vidmode->height / 2, "Procedural Forest - TNM084", NULL, NULL);

    window = glfwCreateWindow(width, height, "Procedural Forest - TNM084", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        return nullptr;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1); //Frame something

    if (glewInit() != GLEW_OK)
        std::cout << "Error with glew \n";

    return window;
}

int main(void)
{
    GLFWwindow* window = init();

    if (window == nullptr)
        return -1;

    {
        glEnable(GL_DEPTH_TEST); //Render things according to depth
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND); //Transparency
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //FLOOR QUAD
        std::vector<float>* vertices = new std::vector<float>{
        -20.0f, 0.0f, -20.0f, 0.0f, 1.0f, 0.0f, 0.0f, 10.0f,
         20.0f, 0.0f, -20.0f, 0.0f, 1.0f, 0.0f, 10.0f, 10.0f,
         20.0f, 0.0f,  20.0f, 0.0f, 1.0f, 0.0f, 10.0f, 0.0f,
        -20.0f, 0.0f,  20.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f
        };

        std::vector<unsigned int>* indices = new std::vector<unsigned int>{
           0, 3, 2,
           0, 2, 1
        };

        std::unique_ptr<VertexArray> m_VAO;
        std::unique_ptr<IndexBuffer> m_IndexBuffer;

        m_VAO = std::make_unique<VertexArray>();

        VertexBuffer vb(static_cast<void*>(vertices->data()), vertices->size() * sizeof(float));

        VertexBufferLayout layout;
        layout.Push<float>(3);
        layout.Push<float>(3);
        layout.Push<float>(2);

        m_VAO->AddBuffer(vb, layout);

        m_IndexBuffer = std::make_unique<IndexBuffer>(static_cast<unsigned int*>(indices->data()), indices->size());

        //std::unique_ptr<Geometry> ground = std::make_unique<Geometry>(vertices, indices); 

        //Projection, view and model matrices
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)(width / height), 0.1f, 1000.0f);

        glm::mat4 view = view = glm::translate(glm::mat4(1.0f), glm::vec3(0, -1.5, -8.0)); //View matrix;
        glm::mat4 model = glm::mat4(1.0f);

        //Shader shader("res/shaders/Basic.vert", "res/shaders/Basic.frag");
        Shader shader("res/shaders/textured.vert", "res/shaders/textured.frag");
        shader.Bind();

        shader.SetUniformMat4f("projectionMatrix", proj);
        shader.SetUniformMat4f("modelviewMatrix", view * model);
        shader.SetUniform1i("tex", 0);

        Texture textureGrass("res/textures/grass.png");
        Texture textureBark("res/textures/bark.png");
        textureBark.Bind();
        shader.SetUniform1i("tex", 0);

        Forest theForest = Forest(shader.GetRendererID());
        //std::cout << "number of leaves " << theForest.leafPositions.size();

        Renderer renderer;

        //Test rendering leafs
        std::vector<float>* leafPositions = new std::vector<float>{
             0.0f, 0.0f, 1.0f,
             1.0f, 0.0f, 1.0f

        };

        unsigned int instanceVBO;
        glGenBuffers(1, &instanceVBO);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * theForest.leafPositions->size(), theForest.leafPositions->data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        std::vector<float>* leafVertices = new std::vector<float>{
            -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,
             0.5f, 0.0f,  0.0f, 0.0f, 1.0f,
             0.0f, 0.25f,  0.0f, 0.0f, 1.0f
        };

        //Test with own classes
        std::unique_ptr<VertexArray> leafVAO;
        leafVAO = std::make_unique<VertexArray>();
        VertexBuffer leafVB(static_cast<void*>(leafVertices->data()), leafVertices->size() * sizeof(float));

        VertexBufferLayout layoutLeaf;
        layoutLeaf.Push<float>(3);
        layoutLeaf.Push<float>(2);

        leafVAO->AddBuffer(leafVB, layoutLeaf);

        //unsigned int quadVAO, quadVBO;
        unsigned int quadVBO;
        //glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        //glBindVertexArray(quadVAO);
        leafVAO->Bind();
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, leafVertices->size(), leafVertices, GL_STATIC_DRAW);

        /*glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));*/
        // also set instance data
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO); // this attribute comes from a different vertex buffer
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glVertexAttribDivisor(2, 1); // tell OpenGL this is an instanced vertex attribute.

        Shader shaderLeaf("res/shaders/leaf.vert", "res/shaders/leaf.frag");
        shaderLeaf.Bind();
        shaderLeaf.SetUniformMat4f("projectionMatrix", proj);
        shaderLeaf.SetUniformMat4f("modelviewMatrix", view * model);

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer.Clear();

            textureGrass.Bind();

            shader.Bind();

            ////renderer.DrawModel(*ground, shader);
            renderer.Draw(*m_VAO, *m_IndexBuffer, shader);
            ////renderer.Draw(*ground->m_VAO, *ground->m_IndexBuffer, shader);
            shaderLeaf.Bind();

            //glBindVertexArray(quadVAO);
            renderer.Clear();

            leafVAO->Bind();
            glDrawArraysInstanced(GL_TRIANGLES, 0, 3, theForest.leafPositions->size());

            textureBark.Bind();
            shader.Bind();

            theForest.Render();

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }

    }

    // Cleanup

    glfwTerminate();
    return 0;
}