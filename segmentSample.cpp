#include "segmentSample.h"

SegmentSample::SegmentSample()
{
	key.resize(512, false);
	firstMouse = true;
	width = 800;
	height = 600;
	deltaTime = 0.0f;
	lastFrame = 0.0f;
}

SegmentSample::~SegmentSample()
{
	delete camera;
	delete shader;
}

void SegmentSample::sampleInit()
{
	initOpenGL(width, height);
	camera = new Camera();
	shader = new Shader("shader/normalshader.vs", "shader/normalshader.fs");
	segModel = new SegmentModel("model/分割模型/home1.obj");
	segModel->loadSegmentData("model/分割模型/home1triangle.txt");
	segModel->modelTransform();
	segModel->treatSegmentData();	
	segModel->printInfo();
	segModel->setupModel();	
	
	genModel = new GenerateModel("model/三视图模型/cube_4.off");
	genModel->modelTransform();
	genModel->printInfo();
	genModel->setupModel();
}

void SegmentSample::sampleRun()
{
	glfwRun(this);
}

void SegmentSample::MouseAction(double xpos, double ypos)
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

void SegmentSample::treatKey()
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

void SegmentSample::KeyAction(int k, int action)
{
	if (action == GLFW_PRESS)
		key[k] = true;
	else if (action == GLFW_RELEASE)
		key[k] = false;
}

void SegmentSample::RenderScene()
{
	GLfloat currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	this->drawModel();
}

void SegmentSample::drawModel()
{
	// 处理输入信息
	this->treatKey();

	// 注意：glm矩阵是右乘，因此后变换的乘载前面
	glm::mat4 modelMatrix, view, projection;
	view = camera->GetViewMatrix();
	projection = glm::perspective(45.0f, 800.0f / 600.0f, 0.1f, 150.0f);

	// 传入uniform
	shader->Use();
	this->shader->setMat4("model", modelMatrix);
	this->shader->setMat4("view", view);
	this->shader->setMat4("projection", projection);
	//segModel->drawModel(shader);
	genModel->drawModel(shader);
}