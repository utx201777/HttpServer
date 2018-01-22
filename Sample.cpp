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
	
	offmodel1 = new offModel("model/房顶_1.off");	
	offmodel1->readyToWork();		
	offmodel1->loadTriangleSDF("model/房顶_1_sdf.txt");
	offmodel1->findLocalMax();
	offmodel1->saveSpecialPoints2File("model/房顶_1_sp.txt");
	offmodel1->setupModel_Edge();

	offmodel2 = new offModel("model/房顶_2.off");
	offmodel2->readyToWork();
	offmodel2->loadTriangleSDF("model/房顶_2_sdf.txt");
	offmodel2->findLocalMax();
	offmodel2->saveSpecialPoints2File("model/房顶_2_sp.txt");
	offmodel2->setupModel_Edge();

	spModel1 = new SpecialPointsModel("model/房顶_1_sp.txt");
	spModel1->readyToWork();
	spModel1->setupModel();

	spModel2 = new SpecialPointsModel("model/房顶_2_sp.txt");
	spModel2->readyToWork();
	spModel2->setupModel();
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
	// 处理输入信息
	this->treatKey();

	// 注意：glm矩阵是右乘，因此后变换的乘载前面

	glm::mat4 modelMatrix, view, projection;
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.3, 0, 0));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5 / offmodel1->maxEdge));
	modelMatrix = glm::translate(modelMatrix, -glm::vec3(offmodel1->box_center.x, offmodel1->box_center.y, offmodel1->box_center.z));
	view = camera->GetViewMatrix();
	
	projection = glm::perspective(45.0f, 800.0f / 600.0f, 0.1f, 150.0f);
	
	// 传入uniform
	shader->Use();
	this->shader->setMat4("model", modelMatrix);
	this->shader->setMat4("view", view);
	this->shader->setMat4("projection", projection);

	spModel1->drawModel(shader);
	offmodel1->drawModel_withoutEBO(shader);

	modelMatrix = glm::mat4();
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.3, 0, 0));	
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5 / offmodel2->maxEdge));
	modelMatrix = glm::translate(modelMatrix, -glm::vec3(offmodel2->box_center.x, offmodel2->box_center.y, offmodel2->box_center.z));
	shader->Use();
	this->shader->setMat4("model", modelMatrix);
	spModel2->drawModel(shader);
	offmodel2->drawModel_withoutEBO(shader);
}