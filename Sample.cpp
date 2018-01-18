#include "sample.h"

Sample::Sample()
{
	key.resize(512, false);
	firstMouse = true;
	width = 800;
	height = 600;
	deltaTime = 0.0f;
	lastFrame = 0.0f;
}

Sample::~Sample()
{
	delete camera;
	delete shader;
}

void Sample::sampleInit()
{
	initOpenGL(width,height);
	camera = new Camera();
	shader = new Shader("shader/normalshader.vs", "shader/normalshader.fs");
	offmodel = new offModel("model/model_3.off");
	offmodel->readyToWork();		// 准备工作
}

void Sample::sampleRun()
{
	glfwRun(this);
}

void Sample::MouseAction(double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
	camera->ProcessMouseMovement(xoffset, yoffset);
}

void Sample::treatKey()
{
	if (key['W'])
		camera->ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
	if (key['S'])
		camera->ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
	if (key['A'])
		camera->ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
	if (key['D'])
		camera->ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);	
}

void Sample::KeyAction(int k, int action)
{
	if (action == GLFW_PRESS)
		key[k] = true;
	else if (action == GLFW_RELEASE)
		key[k] = false;	
}

void Sample::RenderScene()
{
	GLfloat currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	this->drawModel();
}

void Sample::drawModel()
{
	this->treatKey();

	glm::mat4 modelMatrix, view, projection;
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5 / offmodel->maxEdge));
	modelMatrix = glm::translate(modelMatrix, -glm::vec3(offmodel->box_center.x, offmodel->box_center.y, offmodel->box_center.z));
	view = camera->GetViewMatrix();
	
	projection = glm::perspective(45.0f, 800.0f / 600.0f, 0.1f, 150.0f);
	
	// 传入uniform
	this->shader->setMat4("model", modelMatrix);
	this->shader->setMat4("view", view);
	this->shader->setMat4("projection", projection);

	this->offmodel->drawModel2(shader);
}