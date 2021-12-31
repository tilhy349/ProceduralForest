
#include <GL/glew.h> //Needs to be included before other OpenGL libraries
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "Forest.h"

#include <iostream>
#include <vector>

const unsigned int width = 800;
const unsigned int height = 800;

const float widthOfTerrain = 40.0f;
const float depthOfTerrain = 40.0f;

glm::mat4 view;

// camera
glm::vec3 cameraPos = glm::vec3(0.f, -1.5f, -8.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

double lastMousePosX, lastMousePosY;
bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;

//struct Window {
//    GLFWwindow* window;
//
//    glm::mat4 projectionMatrix;
//    glm::mat4 viewMatrix;
//};

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

    //glfwSetWindowUserPointer()
    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1); //Frame something

    if (glewInit() != GLEW_OK)
        std::cout << "Error with glew \n";

    return window;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    glm::vec3 origin = glm::vec3(0.f, -1.5f, -8.0f);

    glm::vec3 right = glm::vec3(-0.5f, 0.0f, 0.0f);
    glm::vec3 left = glm::vec3(0.5f, 0.0f, 0.0f);
    glm::vec3 forward = glm::vec3(0.0f, 0.0f, 0.5f);
    glm::vec3 backward = glm::vec3(0.0f, 0.0f, -0.5f);

    float speed = 3.0f;
    float mouseSpeed = 0.005f;

    double currMousePosX, currMousePosY;

    //glm::vec3 position(view[3][0], view[3][1], view[3][2]);

    if (action == GLFW_REPEAT || action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_W:
            view = glm::translate(view, forward);
            break;
        case GLFW_KEY_A:
            view = glm::translate(view, left);
            break;
        case GLFW_KEY_S:
            view = glm::translate(view, backward);
            break;
        case GLFW_KEY_D:
            view = glm::translate(view, right);
            break;
        }
        
    }   
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    //if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    //{
        //double xpos, ypos;
        //glfwGetCursorPos(window, &xpos, &ypos);

        //if (firstMouse)
        //{
        //    lastMousePosX = xpos;
        //    lastMousePosY = ypos;
        //    firstMouse = false;
        //}

        //glm::vec3 position(view[3][0], view[3][1], view[3][2]);

        //float xoffset = xpos - lastMousePosX;
        //float yoffset = lastMousePosY - ypos; // reversed since y-coordinates go from bottom to top
        //lastMousePosX = xpos;
        //lastMousePosX = ypos;

        //float sensitivity = 0.1f; // change this value to your liking
        //xoffset *= sensitivity;
        //yoffset *= sensitivity;

        //yaw += xoffset;
        //pitch += yoffset;

        //// make sure that when pitch is out of bounds, screen doesn't get flipped
        //if (pitch > 89.0f)
        //    pitch = 89.0f;
        //if (pitch < -89.0f)
        //    pitch = -89.0f;

        //glm::vec3 front;
        //front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        //front.y = sin(glm::radians(pitch));
        //front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        ////cameraFront = glm::normalize(front);
        //cameraFront = front / glm::length(front);

        //view = glm::lookAt(position, position + cameraFront, cameraUp);
    //}
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    //view = glm::rotate(view, )
    //double xpos, ypos;
    //glfwGetCursorPos(window, &xpos, &ypos);

    if (firstMouse)
    {
        lastMousePosX = xpos;
        lastMousePosY = ypos;
        firstMouse = false;
    }

    glm::vec3 position(view[3][0], view[3][1], view[3][2]);

    float xoffset = xpos - lastMousePosX;
    float yoffset = lastMousePosY - ypos; // reversed since y-coordinates go from bottom to top
    lastMousePosX = xpos;
    lastMousePosX = ypos;

    float sensitivity = 0.00001f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    //cameraFront = glm::normalize(front);
    cameraFront = front; /// glm::length(front);

    view = glm::lookAt(position, position + cameraFront, cameraUp);
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

        

        //Projection, view and model matrices
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)(width / height), 0.1f, 1000.0f);

        view = glm::translate(glm::mat4(1.0f), glm::vec3(0, -1.5, -8.0)); //View matrix;
        glm::mat4 model = glm::mat4(1.0f);

        //Shader shader("res/shaders/Basic.vert", "res/shaders/Basic.frag");
        Shader shader("res/shaders/textured.vert", "res/shaders/textured.frag");
        shader.Bind();

        shader.SetUniformMat4f("projectionMatrix", proj);
        shader.SetUniformMat4f("modelviewMatrix", view * model);
        shader.SetUniform1i("tex", 0);

        Texture textureGrass("res/textures/grass.png");
        Texture textureBark("res/textures/bark.png");
        Texture textureLeaf("res/textures/leaf2.png");
        textureBark.Bind();

        Forest theForest(shader.GetRendererID(), widthOfTerrain, depthOfTerrain);
        //std::cout << "number of leaves " << theForest.leafPositions.size();

        Renderer renderer;
        
        //Test with own classes
        //TODO: REMOVE THIS FROM MAIN INTO A FUNCTION
        int numberOfInstances = theForest.leafMatrixRow1->size();

        VertexBuffer instanceVBrow1(static_cast<void*>(theForest.leafMatrixRow1->data()),
            numberOfInstances * 4 * sizeof(float));
        VertexBuffer instanceVBrow2(static_cast<void*>(theForest.leafMatrixRow2->data()),
            numberOfInstances * 4 * sizeof(float));
        VertexBuffer instanceVBrow3(static_cast<void*>(theForest.leafMatrixRow3->data()),
            numberOfInstances * 4 * sizeof(float));
        VertexBuffer instanceVBrow4(static_cast<void*>(theForest.leafMatrixRow4->data()),
            numberOfInstances * 4 * sizeof(float));

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
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        instanceVBrow1.UnBind();
        glVertexAttribDivisor(2, 1); // tell OpenGL this is an instanced vertex attribute.

        glEnableVertexAttribArray(3);
        instanceVBrow2.Bind(); // this attribute comes from a different vertex buffer
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        instanceVBrow2.UnBind();
        glVertexAttribDivisor(3, 1); // tell OpenGL this is an instanced vertex attribute.

        glEnableVertexAttribArray(4);
        instanceVBrow3.Bind(); // this attribute comes from a different vertex buffer
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        instanceVBrow3.UnBind();
        glVertexAttribDivisor(4, 1); // tell OpenGL this is an instanced vertex attribute.

        glEnableVertexAttribArray(5);
        instanceVBrow4.Bind(); // this attribute comes from a different vertex buffer
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        instanceVBrow4.UnBind();
        glVertexAttribDivisor(5, 1); // tell OpenGL this is an instanced vertex attribute.

        Shader shaderLeaf("res/shaders/leaf.vert", "res/shaders/leaf.frag");
        shaderLeaf.Bind();
        shaderLeaf.SetUniformMat4f("projectionMatrix", proj);
        shaderLeaf.SetUniformMat4f("modelviewMatrix", view * model);
        shaderLeaf.SetUniform1i("u_Texture", 0);

        double time = 0;

        //SETUP KEY MANAGEMENT & CURSOR MANAGEMENT

        glfwSetKeyCallback(window, key_callback);
        //glfwSetMouseButtonCallback(window, mouse_button_callback);
        //glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);
        //glfwSetCursorPosCallback(window, cursor_position_callback);

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            time = glfwGetTime();
            //std::cout << "TIME: " << time << "\n";
            /* Render here */
            renderer.Clear();

            //----Render ground-----
            textureGrass.Bind();

            shader.Bind();
            shader.SetUniformMat4f("modelviewMatrix", view * model);

            //renderer.DrawModel(*ground, shader);
            //renderer.Draw(*m_VAO, *m_IndexBuffer, shader);

            //renderer.Clear();

            //----Render forest-----
            theForest.terrain->Render(shader);

            textureBark.Bind();
            shader.Bind();

            theForest.Render();

            //----Render leaves-----
            glDepthMask(GL_FALSE); //Disable writing into the depth buffer (not really sure why this is needed)

            textureLeaf.Bind();
            shaderLeaf.Bind();
            shaderLeaf.SetUniformMat4f("modelviewMatrix", view * model);
            leafVAO->Bind();
            glDrawArraysInstanced(GL_TRIANGLES, 0, 6, numberOfInstances);

            glDepthMask(GL_TRUE); //Enable writing into the depth buffer

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