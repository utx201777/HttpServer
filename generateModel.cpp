#include "generateModel.h"

GenerateModel::GenerateModel(std::string str)
{
	if (!loadModel(str))
	{
		std::cout << "load model error\n";
		exit(0);
	}
}

GenerateModel::~GenerateModel()
{

}

bool GenerateModel::loadModel(std::string str)
{
	std::ifstream infile(str);
	if (!infile.is_open())
	{
		std::cerr << "open file error\n";
		return false;
	}
	std::string s;
	while (infile >> s)
	{
		glm::ivec3 total;		// 顶点数，三角形数，边数
		infile >> total.x >> total.y >> total.z;
		for (int i = 0; i < total.x; i++)
		{
			glm::vec3 p;
			infile >> p.x >> p.y >> p.z;
			box_min.x = std::min(p.x, box_min.x);
			box_min.y = std::min(p.y, box_min.y);
			box_min.z = std::min(p.z, box_min.z);
			box_max.x = std::max(p.x, box_max.x);
			box_max.y = std::max(p.y, box_max.y);
			box_max.z = std::max(p.z, box_max.z);
			Position.push_back(p);
		}
		for (int i = 0; i < total.y; i++)
		{
			glm::ivec3 idx;
			int tmp;
			infile >> tmp >> idx.x >> idx.y >> idx.z;
			Triangles.push_back(idx);
		}
	}
	this->maxEdge = std::max(box_max.x - box_min.x, std::max(box_max.y - box_min.y, box_max.z - box_min.z));
	this->box_center = glm::vec3((box_max.x + box_min.x) / 2, (box_max.y + box_min.y) / 2, (box_max.z + box_min.z) / 2);
	infile.close();
	return true;
}

void GenerateModel::modelTransform()
{
	glm::mat4 modelMat;
	modelMat = glm::scale(modelMat, glm::vec3(1.0 / maxEdge));
	modelMat = glm::translate(modelMat, -glm::vec3(box_center.x, box_center.y, box_center.z));
	for (int i = 0; i < Position.size(); ++i)
	{
		glm::vec4 p = modelMat*glm::vec4(Position[i], 1.0f);
		Position[i] = glm::vec3(p.x, p.y, p.z);
	}
}

void GenerateModel::printInfo()
{
	printf("Points Cnt: %d\n", Position.size());
	printf("Triangles Cnt: %d\n", Triangles.size());
	printf("max Edge: %f\n", maxEdge);
	printf("box_center: (%f,%f,%f)\n", box_center.x, box_center.y, box_center.z);
}

void GenerateModel::setupModel()
{
	std::vector<glm::vec3> color(Position.size(), glm::vec3(1, 0, 0));	// 颜色
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, Position.size()*sizeof(glm::vec3) + color.size()*sizeof(glm::vec3), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, Position.size()*sizeof(glm::vec3), &Position[0]);
	glBufferSubData(GL_ARRAY_BUFFER, Position.size()*sizeof(glm::vec3), color.size()*sizeof(glm::vec3), &color[0]);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Triangles.size()*sizeof(glm::ivec3), &Triangles[0][0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(Position.size()*sizeof(glm::vec3)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void GenerateModel::drawModel(Shader *shader)
{
	shader->Use();
	glm::mat4 view, projection;
	view = glm::lookAt(glm::vec3(1, 0, 0), glm::vec3(0), glm::vec3(0, 1, 0));
	projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.01f, 10.0f);
	shader->setMat4("view", view);
	shader->setMat4("projection", projection);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, Triangles.size() * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	/*	
	glReadPixels(0, 0, WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, colorArr);	
	string url = s2+ "_" + to_string(mod) + ".bmp";
	SOIL_save_screenshot(url.c_str(), SOIL_SAVE_TYPE_BMP, 0, 0, WIDTH, HEIGHT);
	*/
}