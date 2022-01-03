#include "WindowManager.h"

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

    // Open a square window (aspect 1:1) to fill half the screen height

    window = glfwCreateWindow(width, height, "Procedural Forest - TNM084", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        state = -1;
    }

    //glfwSetWindowUserPointer()
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
}



void WindowManager::ProcessCursorPosition(double xpos, double ypos){
    if (firstMouse)
    {
        lastMousePosX = xpos;
        lastMousePosY = ypos;
        firstMouse = false;
    }

    //glm::vec3 position(view[3][0], view[3][1], view[3][2]);

    double xoffset = xpos - lastMousePosX;
    double yoffset = lastMousePosY - ypos; // reversed since y-coordinates go from bottom to top
    lastMousePosX = xpos;
    lastMousePosY = ypos;

    float sensitivity = 0.05f; // change this value to your liking
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

    //Where is lookAt?
    viewMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

void WindowManager::ProcessKeyEvent(int key, int action) {

    glm::vec3 right = glm::vec3(-0.5f, 0.0f, 0.0f);
    glm::vec3 left = glm::vec3(0.5f, 0.0f, 0.0f);
    glm::vec3 forward = glm::vec3(0.0f, 0.0f, 0.5f);
    glm::vec3 backward = glm::vec3(0.0f, 0.0f, -0.5f);

    //glm::vec3 position(view[3][0], view[3][1], view[3][2]);

    if (action == GLFW_REPEAT || action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_W:
            viewMatrix = glm::translate(viewMatrix, -cameraFront);
            break;
        case GLFW_KEY_A:
            viewMatrix = glm::translate(viewMatrix, left);
            break;
        case GLFW_KEY_S:
            viewMatrix = glm::translate(viewMatrix, cameraFront);
            break;
        case GLFW_KEY_D:
            viewMatrix = glm::translate(viewMatrix, right);
            break;
        }

        //UPDATE CAMERA POSITION/VIEW MATRIX
        //cameraPos = glm::vec3(view[3][0], view[3][1], view[3][2]);
    }
}
