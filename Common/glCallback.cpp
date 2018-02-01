#include "glCallback.h"

GLFWwindow * window = nullptr;
ICallbacks *callback = nullptr;

inline void printText(std::string str) 
{ 
	std::cout << str << std::endl; 
}

void keyfunc(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	callback->KeyAction(key, action);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	callback->MouseAction(xpos, ypos);
}

bool initOpenGL(int WIDTH, int HEIGHT)
{
	printText("Shader Start!");
	if (glfwInit() == -1)
		return false;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window = glfwCreateWindow(WIDTH, HEIGHT, "Shader", nullptr, nullptr);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, WIDTH, HEIGHT);
	return true;
}

void glfwInitCallback()
{
	// ×¢²á»Øµ÷º¯Êý
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, keyfunc);
	glfwSetCursorPosCallback(window, mouse_callback);
}

void glfwRun(ICallbacks * c)
{
	callback = c;
	glfwInitCallback();
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		callback->RenderScene();
		glfwSwapBuffers(window);
	}
	glfwTerminate();
}