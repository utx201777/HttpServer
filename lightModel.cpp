#include "lightModel.h"

LightModel::LightModel(std::string str)
{
	if (!loadModel(str))
	{
		std::cout << "load model error\n";
		exit(0);
	}
}

LightModel::~LightModel()
{

}

/*读取画的球*/
bool LightModel::loadModel(std::string str)
{
	std::ifstream infile(str);
	if (!infile.is_open())
	{
		return false;
	}
	std::string s;
	while (infile >> s)
	{
		glm::ivec3 total;
		infile >> total.x >> total.y >> total.z;
		// 设定参数
		this->Points.resize(total.x);
		this->Triangles.resize(total.y);
		// 读入顶点并且计算包围盒
		for (int i = 0; i < total.x; i++)
		{
			glm::vec3 p;
			infile >> p.x >> p.y >> p.z;		
			this->Points[i] = p;
		}
		// 读取三角索引
		for (int i = 0; i < total.y; i++)
		{
			glm::ivec3 idx;
			int tmp;
			infile >> tmp >> idx.x >> idx.y >> idx.z;
			this->Triangles[i] = idx;
		}
	}
	infile.close();
	return true;
}

void LightModel::printInfo()
{
	std::cout << "灯的数量： " << lightPos.size() << std::endl;
}

void LightModel::setupModel()
{
	std::vector<glm::vec3> color(Points.size(), glm::vec3(1,1,1));	// 颜色
	// 传输数据
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, Points.size()*sizeof(glm::vec3) + color.size()*sizeof(glm::vec3), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, Points.size()*sizeof(glm::vec3), &Points[0]);
	glBufferSubData(GL_ARRAY_BUFFER, Points.size()*sizeof(glm::vec3), color.size()*sizeof(glm::vec3), &color[0]);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Triangles.size()*sizeof(glm::ivec3), &Triangles[0][0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(Points.size()*sizeof(glm::vec3)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void LightModel::drawModel(Shader *shader)
{
	shader->Use();
	for (int i = 0; i < lightPos.size(); ++i)
	{
		glm::mat4 modelMatrix;
		modelMatrix = glm::translate(modelMatrix, lightPos[i]);
		//modelMatrix = glm::scale(modelMatrix, glm::vec3(100, 100, 100));
		shader->setMat4("model", modelMatrix);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, Triangles.size() * 3, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		shader->setMat4("model", glm::mat4());
	}
	
}

void LightModel::loadPos(std::string str)
{
	std::ifstream infile(str);
	if (!infile.is_open())
	{
		std::cout << "文件导入失败\n";
		exit(0);
		return;
	}
	int cnt=0;
	infile >> cnt;
	lightPos.resize(cnt);
	for (int i = 0; i < cnt; ++i)
	{
		glm::vec3 pos;
		infile >> pos[0] >> pos[1] >> pos[2];
		lightPos[i] = pos;
	}
	infile.close();
}


void LightModel::modelTransform(glm::mat4 modelMat)
{
	for (int i = 0; i < lightPos.size(); ++i)
	{
		glm::vec4 p = modelMat*glm::vec4(lightPos[i], 1.0f);
		lightPos[i] = glm::vec3(p.x, p.y, p.z);
	}
}
