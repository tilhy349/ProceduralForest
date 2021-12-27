
#include <GL/glew.h> //Needs to be included before other OpenGL libraries
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

//TNM084 lab3 code, Ingemars
#include "vendor/glugg/glugg.h"
//#include "vendor/glugg/MicroGlut.h"
//#include "vendor/glugg/LittleOBJLoader.h"

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include <iostream>
#include <vector>

const unsigned int width = 800;
const unsigned int height = 800;

const char* pos = "inPosition";

void MakeCylinderAlt(int aSlices, float height, float topwidth, float bottomwidth)
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

void MakeBranches(const int maxDepth, int currentDepth, float currentHeight, int branches) {
    branches += rand() % (3 + 1 - 0) + 0;
    for (int i = 0; i < branches; ++i) {
        if (currentDepth < maxDepth) {
            gluggPushMatrix();
            gluggTranslate(0, currentHeight, 0);
            gluggScale(0.5, 0.5, 0.5);
            gluggRotate(i * 3.14 / branches, 0.0, 1.0, 0.0);

            float random = rand() % (7 + 1 - 2) + 2;
            gluggRotate(3.14 / random, 0.0, 0.0, 1.0);


            //printf("current height: %f\n", currentHeight);
            MakeCylinderAlt(20, currentHeight, 0.1, 0.15);

            MakeBranches(maxDepth, currentDepth + 1, currentHeight, branches);
        }
    }

    gluggPopMatrix();

}

GLuint MakeTree(int* count, GLuint program)
{
    gluggSetPositionName("inPosition");
    gluggSetNormalName("inNormal");
    gluggSetTexCoordName("inTexCoord");

    gluggBegin(GLUGG_TRIANGLES);
    float height = 2.0;
    MakeCylinderAlt(20, height, 0.1, 0.15);

    int maxDepth = 2;
    int branches = 4;
    MakeBranches(maxDepth, 0, height, branches);

    //gluggPopMatrix();
    return gluggEnd(count, program, 0);
}

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

        unsigned int vao;
        GLCall(glGenVertexArrays(1, &vao));
        GLCall(glBindVertexArray(vao));

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
        
        //Projection, view and model matrices
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)(width / height), 0.1f, 1000.0f);

        glm::mat4 view = view = glm::translate(glm::mat4(1.0f), glm::vec3(0, -1.5, -7.0)); //Model matrix;
        glm::mat4 model = glm::mat4(1.0f);

        //Testing Ingemars frustum
        //mat4 proj = perspective(45.0f, (float)(width / height), 0.1f, 1000.0f);
        //mat4 proj = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 300.0);

        //Shader shader("res/shaders/Basic.vert", "res/shaders/Basic.frag");
        Shader shader("res/shaders/textured.vert", "res/shaders/textured.frag");
        shader.Bind();
        
        shader.SetUniformMat4f("projectionMatrix", proj);
        shader.SetUniformMat4f("modelviewMatrix", view * model);

        Texture texture("res/textures/bark.png");
        texture.Bind();
        shader.SetUniform1i("tex", 0);

        //MAKING A TREE
        int treecount;
        GLuint tree = MakeTree(&treecount, shader.GetRendererID());

        Renderer renderer;

        // View
        GLfloat a = 0.0;
        vec3 campos = { 0, 1.5, 10 };
        vec3 forward = { 0, 0, -4 };
        vec3 up = { 0, 1, 0 };
        mat4 worldToView, m;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer.Clear();

            Texture textureGrass("res/textures/grass.png");
            textureGrass.Bind();
            shader.SetUniform1i("tex", 0);

            shader.Bind();

            renderer.Draw(*m_VAO, *m_IndexBuffer, shader); //Draw pyramid

            Texture texture("res/textures/bark.png");
            texture.Bind();
            shader.SetUniform1i("tex", 0);

            glBindVertexArray(tree);	// Select VAO
            glDrawArrays(GL_TRIANGLES, 0, treecount);

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