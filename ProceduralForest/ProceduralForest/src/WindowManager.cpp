#include "WindowManager.h"
#include <iostream>

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    WindowManager* windowManager =
        static_cast<WindowManager*>(glfwGetWindowUserPointer(window));

    windowManager->ProcessCursorPosition(xpos, ypos);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    WindowManager* windowManager =
        static_cast<WindowManager*>(glfwGetWindowUserPointer(window));

    windowManager->ProcessKeyEvent(key, action);
}

void WindowManager::Init()
{    
    /* Initialize the library */
    if (!glfwInit())
        state = -1;

    //Version 3.3 and profile to core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    // Determine the desktop size
    vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    window = glfwCreateWindow(width, height, "Procedural Forest - TNM084", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        state = -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1); //Frame something

    //Associate window with this instance of WindowManager
    glfwSetWindowUserPointer(window, static_cast<void*>(this));

    //SETUP KEY MANAGEMENT & CURSOR MANAGEMENT
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetKeyCallback(window, key_callback);
}

WindowManager::WindowManager(const int windowWidth, const int windowHeight): width{windowWidth}, height{windowHeight}
{
    Init();
    viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f / 2, -1.5, -10.0f / 2));
    proj = glm::perspective(glm::radians(45.0f), (float)(width / height), 0.1f, 1000.0f);
    treeTextureMode = 0;
}

void WindowManager::ProcessCursorPosition(double xpos, double ypos){
    if (firstMouse)
    {
        lastMousePosX = xpos;
        lastMousePosY = ypos;
        firstMouse = false;
    }

    double xoffset = xpos - lastMousePosX;
    double yoffset = lastMousePosY - ypos; // reversed since y-coordinates go from bottom to top
    lastMousePosX = xpos;
    lastMousePosY = ypos;

    float sensitivity = 0.1f; 
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
    cameraFront = glm::normalize(front);

    //Also recalculate right and up vectors
    //And normalize them

    cameraRight = glm::normalize(glm::cross(cameraFront, glm::vec3(0, 1.0f, 0)));  
    cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));

    //Update viewMatrix
    viewMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

void WindowManager::ProcessKeyEvent(int key, int action) {

    glm::vec3 right = glm::vec3(-0.5f, 0.0f, 0.0f);
    glm::vec3 left = glm::vec3(0.5f, 0.0f, 0.0f);
    glm::vec3 forward = glm::vec3(0.0f, 0.0f, 0.5f);
    glm::vec3 backward = glm::vec3(0.0f, 0.0f, -0.5f);

    float velocity = 0.05f; //Modify this for faster/slower movement

    if (action == GLFW_REPEAT || action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_W:
            cameraPos += cameraFront * velocity;
            break;
        case GLFW_KEY_A:
            cameraPos += -cameraRight * velocity;
            break;
        case GLFW_KEY_S:
            cameraPos += -cameraFront * velocity;
            break;
        case GLFW_KEY_D:
            cameraPos += cameraRight * velocity;
            break;
        case GLFW_KEY_1:
            seasonsManager.UpdateSeason();
            std::cout << "Switching seasons. Current season is: " << seasonsManager.CurrentSeasonToString() << "\n";
            break;
        case GLFW_KEY_2:
            if (treeTextureMode == 0)
                treeTextureMode = 1;
            else
                treeTextureMode = 0;
            break;
        }

        //UPDATE VIEW MATRIX
        viewMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    }
}
