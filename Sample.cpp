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
	
	offmodel1 = new offModel("model/cube_5_without_points.off");	
	offmodel1->setOffset(-0.3);
	offmodel1->modelTransform();
	offmodel1->readyToWork();
	offmodel1->saveModelMatrix("model/cube_standard_matrix.txt");
	//offmodel1->loadTriangleSDF("model/cylinder_1_sdf.txt");
	//offmodel1->findLocalMax();
	//offmodel1->saveSpecialPoints2File("model/cylinder_1_sp.txt");
	//offmodel1->saveAllPoints("model/cylinder_1_allpoints_.txt");
	//offmodel1->setupModel_Edge();	
	//offmodel1->setupModel_Normal();
	//offmodel1->setupModel_SDF();
	offmodel1->setupModel();


	offmodel2 = new offModel("model/cube_4.off");
	offmodel2->setOffset(0.3);
	offmodel2->modelTransform();
	offmodel2->readyToWork();
	offmodel2->saveModelMatrix("model/cube_4_matrix.txt");
	//offmodel2->loadTriangleSDF("model/cylinder_standard_sdf.txt");
	//offmodel2->findLocalMax();
	//offmodel2->saveSpecialPoints2File("model/cylinder_standard_sp.txt");
	//offmodel2->saveAllPoints("model/cylinder_standard_allpoints_.txt");
	//offmodel2->setupModel_Edge();	
	//offmodel1->setupModel_Normal();
	//offmodel2->setupModel_SDF();
	offmodel2->setupModel();

	
	lightModel1 = new LightModel("model/sphere.off");
	lightModel1->loadPos("model/cube_5_points_pos_4.txt");
	lightModel1->printInfo();
	lightModel1->modelTransform(offmodel1->modelMat);
	lightModel1->setupModel();

	lightModel2 = new LightModel("model/sphere.off");
	lightModel2->loadPos("model/cube_4_pos_4.txt");
	lightModel2->printInfo();
	lightModel2->modelTransform(offmodel2->modelMat);
	lightModel2->setupModel();

	/*
	offmodel2 = new offModel("model/cube_4.off");
	offmodel2->setOffset(0.0);
	offmodel2->modelTransform();
	offmodel2->readyToWork();
	offmodel2->loadTriangleSDF("model/cube_4_sdf.txt");
	offmodel2->findLocalMax();
	offmodel2->saveSpecialPoints2File("model/cube_4_sp.txt");
	offmodel2->saveAllPoints("model/cube_4_allpoints.txt");
	offmodel2->setupModel_Edge();

	spModel1 = new SpecialPointsModel("model/cube_3_sp.txt");
	spModel1->modelTransform(offmodel1->modelMat);
	spModel1->readyToWork();
	spModel1->setupModel();

	spModel2 = new SpecialPointsModel("model/cube_4_sp.txt");
	spModel2->modelTransform(offmodel2->modelMat);
	spModel2->readyToWork();
	spModel2->setupModel();

	nearModel = new NearestPairModel(spModel1->returnSpecialPoints(), spModel2->returnSpecialPoints());
	nearModel->findNearestPair();
	nearModel->setupModel();
	*/
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
	view = camera->GetViewMatrix();	
	projection = glm::perspective(45.0f, 800.0f / 600.0f, 0.1f, 150.0f);
	
	// 传入uniform
	shader->Use();
	this->shader->setMat4("model", modelMatrix);
	this->shader->setMat4("view", view);
	this->shader->setMat4("projection", projection);

	offmodel1->drawModel(shader);
	offmodel2->drawModel(shader);
	lightModel1->drawModel(shader);
	lightModel2->drawModel(shader);
	//lightModel2->drawModel(shader);
	//spModel1->drawModel(shader);	
	//spModel2->drawModel(shader);
	//offmodel1->drawModel_withoutEBO(shader);
	//offmodel2->drawModel_withoutEBO(shader);
	//nearModel->drawModel(shader);
}