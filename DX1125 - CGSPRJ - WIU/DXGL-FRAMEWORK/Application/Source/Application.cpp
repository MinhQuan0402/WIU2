
#include "Application.h"

//Include GLEW
#include <GL/glew.h>

//Include GLFW
#include <GLFW/glfw3.h>

//Include the standard C++ headers
#include <stdio.h>
#include <stdlib.h>
#include "KeyboardController.h"
#include "MouseController.h"
#include "GameObjectManager.h"
#include "Time.h"
#include "SceneGame.h"
#include "SceneManager.h"
#include "CollisionManager.h"
#include "ColliderManager.h"
#include "SceneDuckShooting.h"

GLFWmonitor* primaryMonitor;
const GLFWvidmode* videoMode;
GLFWwindow* m_window;		
const unsigned char FPS = 60; // FPS of this game
const unsigned int frameTime = 1.0 / FPS; // time for each frame
constexpr double FIXED_TIME_STEP = 1.0 / 60.0;

int Application::m_consoleHeight = 0;
int Application::m_consoleWidth = 0;
bool Application::enableCursor = false;
bool Application::showCursor = false;

//Define an error callback
static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
	_fgetchar();
}

//Define the key input callback
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	KeyboardController::GetInstance()->Update(key, action);
}

static void mousebtn_callback(GLFWwindow* window, int key, int action, int mods)
{
	if (action == GLFW_PRESS)
		MouseController::GetInstance()->UpdateMouseButtonPressed(key);
	else
		MouseController::GetInstance()->UpdateMouseButtonReleased(key);
}

static void mousescroll_callback(GLFWwindow* window, double offsetX, double offsetY)
{
	MouseController::GetInstance()->UpdateMouseScroll(offsetX, offsetY);
}

static void resize_callback(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, w, h); //update opengl the new window size
	Application::m_consoleHeight = h;
	Application::m_consoleWidth = w;
}

bool Application::IsKeyPressed(unsigned short key)
{
    return ((GetAsyncKeyState(key) & 0x8001) != 0);
}

Application::Application()
{
}

Application::~Application()
{
}

void Application::Init()
{
	//Set the error callback
	glfwSetErrorCallback(error_callback);

	//Initialize GLFW
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	//Set the GLFW window creation hints - these are optional
	glfwWindowHint(GLFW_SAMPLES, 4); //Request 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Request a specific OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //Request a specific OpenGL version
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL

	// Get the primary monitor
	primaryMonitor = glfwGetPrimaryMonitor();

	// Get the video mode of the monitor (which contains resolution, etc.)
	videoMode = glfwGetVideoMode(primaryMonitor);

	m_consoleWidth = 1200;
	m_consoleHeight = 675;

	//Create a window and create its OpenGL context
	m_window = glfwCreateWindow(m_consoleWidth, m_consoleHeight, "DX1118 MyFRAMEWORK", NULL, NULL);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//If the window couldn't be created
	if (!m_window)
	{
		fprintf( stderr, "Failed to open GLFW window.\n" );
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//This function makes the context of the specified window current on the calling thread. 
	glfwMakeContextCurrent(m_window);

	//Sets the key callback
	glfwSetKeyCallback(m_window, key_callback);
	glfwSetMouseButtonCallback(m_window, mousebtn_callback);
	glfwSetScrollCallback(m_window, mousescroll_callback);

	//Sets the resize callback to handle window resizing
	glfwSetWindowSizeCallback(m_window, resize_callback);

	glewExperimental = true; // Needed for core profile
	//Initialize GLEW
	GLenum err = glewInit();

	//If GLEW hasn't initialized
	if (err != GLEW_OK) 
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		//return -1;
	}
}

void Application::Run()
{
	//Main Loop
	CollisionManager::GetInstance()->SetUpDynamicWorld(10.0f);
	SceneManager::GetInstance()->PushState(new SceneDuckShooting);	
	Time::fixedDeltaTime = FIXED_TIME_STEP;
	m_timer.startTimer();    // Start timer to calculate how long it takes to render this frame
	while (!glfwWindowShouldClose(m_window) && !IsKeyPressed(VK_ESCAPE))
	{
		if (enableCursor == false)
			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else if (showCursor == false)
			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		else
			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		Time::deltaTime = (float)m_timer.getElapsedTime();
		Time::time += Time::deltaTime;

		SceneManager::GetInstance()->Update();
		CollisionManager::GetInstance()->UpdateCollision();
		SceneManager::GetInstance()->LateUpdate();
		SceneManager::GetInstance()->Render();
		//Swap buffers
		glfwSwapBuffers(m_window);

		KeyboardController::GetInstance()->PostUpdate();
		MouseController::GetInstance()->PostUpdate();

		double mouse_x, mouse_y;
		glfwGetCursorPos(m_window, &mouse_x, &mouse_y);
		MouseController::GetInstance()->UpdateMousePosition(mouse_x, mouse_y);
		
		//Get and organize events, like keyboard and mouse input, window resizing, etc...
		glfwPollEvents();
        m_timer.waitUntil(frameTime);       // Frame rate limiter. Limits each frame to a specified time in ms.   

	} //Check if the ESC key had been pressed or if the window had been closed
	SceneManager::GetInstance()->PopState();
}

void Application::Exit()
{
	KeyboardController::DestroyInstance();
	MouseController::DestroyInstance();
	GameObjectManager::DestroyInstance();
	SceneManager::DestroyInstance();
	ColliderManager::DestroyInstance();
	CollisionManager::DestroyInstance();

	//Close OpenGL window and terminate GLFW
	glfwDestroyWindow(m_window);

	//Finalize and clean up GLFW
	glfwTerminate();
}
