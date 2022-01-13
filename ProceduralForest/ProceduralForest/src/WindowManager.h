#pragma once
#include <GLFW/glfw3.h>
#include <gtc/matrix_transform.hpp>
#include <glm.hpp>
#include "Season.h"

class WindowManager {
private:
	GLFWwindow* window;
	const GLFWvidmode* vidmode; // GLFW struct to hold information about the display

	const int width;
	const int height;

	int state;	

	// camera
	glm::vec3 cameraPos = glm::vec3(10.0f / 2, 1.5f, 10.0f / 2);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::vec3 position = glm::vec3(0.0f, 2.0f, 2.0f);

	double lastMousePosX = width / 2;
	double lastMousePosY = height / 2;
	bool firstMouse = true;
	float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
	float pitch = 0.0f;

	void Init();

	// season
public: SeasonHandler seasonsManager;

public:
	WindowManager(const int windowWidth, const int windowHeight);

	GLFWwindow* GetWindow() const { return window; }
	int CheckState() const { return state; }

	void ProcessCursorPosition(double xpos, double ypos);
	void ProcessKeyEvent(int key, int action);

	glm::mat4 viewMatrix;
	glm::mat4 proj;
	int treeTextureMode;
};
