//#include "Shader.hpp" //Read file, create and compile shader //TNM061
#include <GL/glew.h> //Needs to be included before other OpenGL libraries
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

//#include "vendor/imgui/imgui.h"
//#include "vendor/imgui/imgui_impl_glfw.h"
//#include "vendor/imgui/imgui_impl_opengl3.h"
#include "vendor/glugg/glugg.h"

#include "MatrixStack.hpp" //Dynamic (currently static) matrix stack //BASED ON STEGU's code from TMN061
#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
//#include "Geometry.h"

#include <iostream>

void MakeCylinderAlt(int aSlices, float height, float topwidth, float bottomwidth)
{
    gluggMode(GLUGG_TRIANGLE_STRIP);
    glm::vec3 top(0, height, 0);
    glm::vec3 center(0, 0, 0);
    glm::vec3 bn(0, -1, 0); // Bottom normal
    glm::vec3 tn(0, 1, 0); // Top normal

    for (float a = 0.0; a < 2.0 * glm::pi<float>() + 0.0001; a += 2.0 * glm::pi<float>() / aSlices)
    {
        float a1 = a;

        glm::vec3 p1(topwidth * cos(a1), height, topwidth * sin(a1));
        glm::vec3 p2(bottomwidth * cos(a1), 0, bottomwidth * sin(a1));
        glm::vec3 pn(cos(a1), 0, sin(a1));

        // Done making points and normals. Now create polygons!
        gluggNormal(pn.x, pn.y, pn.z);
        gluggTexCoord(height, a1 / glm::pi<float>());
        gluggVertex(p2.x, p2.y, p2.z);
        gluggTexCoord(0, a1 / glm::pi<float>());
        gluggVertex(p1.x, p1.y, p1.z);
    }

    // Then walk around the top and bottom with fans
    gluggMode(GLUGG_TRIANGLE_FAN);
    gluggNormal(bn.x, bn.y, bn.z);
    gluggVertex(center.x, center.y, center.z);
    // Walk around edge
    for (float a = 0.0; a <= 2.0 * glm::pi<float>() + 0.001; a += 2.0 * glm::pi<float>() / aSlices)
    {
        glm::vec3 p(bottomwidth * cos(a), 0, bottomwidth * sin(a));
        gluggVertex(p.x, p.y, p.z);
    }
    // Walk around edge
    gluggMode(GLUGG_TRIANGLE_FAN); // Reset to new fan
    gluggNormal(tn.x, tn.y, tn.z);
    gluggVertex(top.x, top.y, top.z);
    for (float a = 2.0 * glm::pi<float>(); a >= -0.001; a -= 2.0 * glm::pi<float>() / aSlices)
    {
        glm::vec3 p(topwidth * cos(a), height, topwidth * sin(a));
        gluggVertex(p.x, p.y, p.z);
    }
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
    //window = glfwCreateWindow(640, 480, "Procedural Forest", NULL, NULL);
    // Determine the desktop size
    vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    // Open a square window (aspect 1:1) to fill half the screen height
    //window = glfwCreateWindow(vidmode->height / 2, vidmode->height / 2, "Procedural Forest - TNM084", NULL, NULL);

    window = glfwCreateWindow(960, 540, "Procedural Forest - TNM084", NULL, NULL);

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

        glEnable(GL_BLEND); //Transparency
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

        unsigned int vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        float vertices[] = {
       -50.0f, -50.0f, 0.0f, 0.0f,
        50.0f, -50.0f, 1.0f, 0.0f,
        50.0f,  50.0f, 1.0f, 1.0f,
       -50.0f,  50.0f, 0.0f, 1.0f
        };

        unsigned int indices[] = {
           0, 1, 2,
           2, 3, 0
        };

        std::unique_ptr<VertexArray> m_VAO;
        std::unique_ptr<IndexBuffer> m_IndexBuffer;

        m_VAO = std::make_unique<VertexArray>();

        VertexBuffer vb(vertices, 4 * 4 * sizeof(float));

        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);

        m_VAO->AddBuffer(vb, layout);

        m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 6);
        
        //Geometry test;
        //test.CreateCylinder();

        glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f); //Projection matrix
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)); //View matrix
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(200, 200, 0)); //Model matrix

        glm::mat4 mvp = proj * view * model;

        Shader shader("res/shaders/Basic.vert", "res/shaders/Basic.frag");
        shader.Bind();
        shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);
        shader.SetUniformMat4f("u_MVP", mvp);

        Texture texture("res/textures/Poster.png");
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0);

        //Unbind everything
        /*va.UnBind();
        vb.UnBind();
        ib.UnBind();*/
        //shader.UnBind();

        Renderer renderer;

        /*ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); 

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init((char*)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));

        float r = 0.0f;
        float increment = 0.05f;*/

        //TEST
        //MatrixStack hej;
        //GLUquadricObj* quad = gluNewQuadric();
        //gluCylinder(quad, 2.f, 3.f, 3.f, 32, 32);

        // Debug
        glm::vec3 translationA(200, 200, 0);
        glm::vec3 translationB(400, 200, 0);

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer.Clear();

            renderer.Draw(*m_VAO, *m_IndexBuffer, shader);

            //ImGui_ImplOpenGL3_NewFrame();
            //ImGui_ImplGlfw_NewFrame();
            //ImGui::NewFrame();

            //shader.Bind();

            //{
            //    glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA); //Model matrix
            //    glm::mat4 mvp = proj * view * model;
            //    shader.SetUniformMat4f("u_MVP", mvp);
            //    renderer.Draw(va, ib, shader);
            //}
            //
            //{
            //    glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB); //Model matrix
            //    glm::mat4 mvp = proj * view * model;
            //    shader.SetUniformMat4f("u_MVP", mvp);
            //    renderer.Draw(va, ib, shader);
            //}

            ////Color animation
            //if (r > 1.0f)
            //    increment = -0.05f;
            //else if (r < 0.0f)
            //    increment = 0.05f;

            //static float f = 0.0f;
            //static int counter = 0;

            //ImGui::SliderFloat3("Translation A", &translationA.x, 0.0f, 960.0f); 
            //ImGui::SliderFloat3("Translation B", &translationB.x, 0.0f, 960.0f);

            //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            //ImGui::Render();
            //ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());



            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }

    }

    // Cleanup
    /*ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();*/

    glfwTerminate();
    return 0;
}