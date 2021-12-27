

//#include "Shader.hpp" //Read file, create and compile shader //TNM061
#include <GL/glew.h> //Needs to be included before other OpenGL libraries
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"
#include "vendor/glugg/glugg.h"
#include "vendor/glugg/MicroGlut.h"
#include "vendor/glugg/LittleOBJLoader.h"

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
#include <vector>

const unsigned int width = 800;
const unsigned int height = 800;

const char* pos = "inPosition";

void MakeCylinderAlt(int aSlices, float height, float topwidth, float bottomwidth)
{
    gluggMode(GLUGG_TRIANGLE_STRIP);
    glm::vec3 top(0, height, 0);
    glm::vec3 center(0, 0, 0);
    glm::vec3 bn(0, -1, 0); // Bottom normal
    glm::vec3 tn(0, 1, 0); // Top normal

    for (float a = 0.0f; a < 2.0f * glm::pi<float>() + 0.0001f; a += 2.0f * glm::pi<float>() / aSlices)
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
    for (float a = 0.0f; a <= 2.0f * glm::pi<float>() + 0.001f; a += 2.0f * glm::pi<float>() / aSlices)
    {
        glm::vec3 p(bottomwidth * cos(a), 0, bottomwidth * sin(a));
        gluggVertex(p.x, p.y, p.z);
    }
    // Walk around edge
    gluggMode(GLUGG_TRIANGLE_FAN); // Reset to new fan
    gluggNormal(tn.x, tn.y, tn.z);
    gluggVertex(top.x, top.y, top.z);
    for (float a = 2.0f * glm::pi<float>(); a >= -0.001f; a -= 2.0f * glm::pi<float>() / aSlices)
    {
        glm::vec3 p(topwidth * cos(a), height, topwidth * sin(a));
        gluggVertex(p.x, p.y, p.z);
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
    //gluggPushMatrix();
    //gluggTranslate(400, 400, -30);
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

    // Floor quad
    //float vertices[] = {-20.5, 0.0, -20.5, 0, 1.0, 0, 1.0f, 1.0f,
    //                     20.5, 0.0, -20.5, 0, 1.0, 0, 0.0f,  1.0f,
    //                     20.5, 0.0,  20.5, 0, 1.0, 0, 0.0f,  0.0f,
    //                    -20.5, 0.0,  20.5, 0, 1.0, 0, 1.0f, 0.0f };

    //unsigned int indices[] = { 0,3,2, 0,2,1 };
    // Floor quad
    GLfloat vertices2[] = { -20.5,0.0,-20.5,
                            20.5,0.0,-20.5,
                            20.5,0.0,20.5,
                            -20.5,0.0,20.5 };
    GLfloat normals2[] = { 0,1.0,0,
                            0,1.0,0,
                            0,1.0,0,
                            0,1.0,0 };
    GLfloat texcoord2[] = { 50.0f, 50.0f,
                            0.0f, 50.0f,
                            0.0f, 0.0f,
                            50.0f, 0.0f };
    GLuint indices2[] = { 0,3,2, 0,2,1 };

    // Upload geometry to the GPU:
    Model* floormodel = LoadDataToModel((vec3*)vertices2, (vec3*)normals2, (vec2*)texcoord2, NULL,
        indices2, 4, 6);
    
    {
        glEnable(GL_DEPTH_TEST); //Render things according to depth
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND); //Transparency
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        unsigned int vao;
        GLCall(glGenVertexArrays(1, &vao));
        GLCall(glBindVertexArray(vao));

        /*std::vector<float>* vertices = new std::vector<float>{
       -0.5f, 0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
       -0.5f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.8f,  0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f
        };

        std::vector<unsigned int>* indices= new std::vector<unsigned int>{
           0, 1, 2,
           0, 2, 3,
           0, 1, 4,
           1, 2, 4,
           2, 3, 4,
           3, 0, 4
        };*/

        std::vector<float>* vertices = new std::vector<float>{
        -0.8f, 0.0f, -0.8f, 0.0f, 1.0f, 0.0f, 0.0f, 10.0f,
         0.8f, 0.0f, -0.8f, 0.0f, 1.0f, 0.0f, 10.0f, 10.0f,
         0.8f, 0.0f,  0.8f, 0.0f, 1.0f, 0.0f, 10.0f, 0.0f,
        -0.8f, 0.0f,  0.8f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f
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
        
        //OLD Projection, view and model matrices
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)(width / height), 0.1f, 1000.0f);
        //glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 30)); //View matrix
        //glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(500, 300, 0)); //Model matrix
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 mvp = proj * view * model;

        //Testing Ingemars
        //mat4 proj = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 300.0);
        //mat4 mvp = proj;

        Shader shader("res/shaders/Basic.vert", "res/shaders/Basic.frag");
        shader.Bind();
        
        shader.SetUniformMat4f("u_MVP", mvp);
        //shader.SetUniformMat4fVU("u_MVP", mvp);

        Texture texture("res/textures/Poster.png");
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0);

        //MAKING A TREE
        int treecount;
        GLuint tree = MakeTree(&treecount, shader.GetRendererID());

        //Unbind everything
        /*va.UnBind();
        vb.UnBind();
        ib.UnBind();*/
        //shader.UnBind();

        Renderer renderer;

        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init((char*)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));

        // Debug
        glm::vec3 translationA(0, -0.5, -2.0);
        glm::vec3 translationB(400, 200, 0);

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

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            shader.Bind();
            //glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA); //Model matrix
            //view = glm::mat4(1.0f);
            //view = glm::translate(view, glm::vec3(0.0f, -0.5f, -2.0f));
            view = glm::translate(glm::mat4(1.0f), translationA); //Model matrix
            
            mvp = proj * view * model;
            shader.SetUniformMat4f("u_MVP", mvp);
            renderer.Draw(*m_VAO, *m_IndexBuffer, shader); //Draw pyramid

            //----- TESTAR, hur fungerar detta ens? ----
            
            /*if (glutKeyIsDown('a'))
                forward = MultMat3Vec3(mat4tomat3(Ry(0.05)), forward);
            if (glutKeyIsDown('d'))
                forward = MultMat3Vec3(mat4tomat3(Ry(-0.05)), forward);
            if (glutKeyIsDown('w'))
                campos = VectorAdd(campos, ScalarMult(forward, 0.1));
            if (glutKeyIsDown('s'))
                campos = VectorSub(campos, ScalarMult(forward, 0.1));

            worldToView = lookAtv(campos, VectorAdd(campos, forward), up);
            m = Mult(worldToView, T(0, 0, 0));
            a += 0.1;

            mvp = proj * m;
            shader.Bind();
            texture.Bind();
            shader.SetUniformMat4fVU("u_MVP", mvp);

            DrawModel(floormodel, shader.GetRendererID(), "inPosition", "inNormal", "inTexCoord");

            shader.Bind();
            texture.Bind();
            m = Mult(worldToView, T(0, 0, 0));
            mvp = proj * m;
            shader.SetUniformMat4fVU("u_MVP", mvp);*/

            //OLD
            //mvp = proj * view * model;
            //shader.SetUniformMat4f("u_MVP", mvp);

            //When I switch from pyramid to tree, is everything bound? Probably not.. DrawModel in littleOBJLoader
            //GLCall(glBindVertexArray(tree));	// Select VAO
            //GLCall(glDrawArrays(GL_TRIANGLES, 0, treecount));

            //GUI
            ImGui::SliderFloat3("Translation A", &translationA.x, -5.0f, 5.0f); 
            //ImGui::SliderFloat3("Translation B", &translationB.x, 0.0f, 960.0f);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }

    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}