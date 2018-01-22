#include "specialPointsModel.h"

SpecialPointsModel::SpecialPointsModel(std::string str)
{
	if (!loadModel(str))
	{
		std::cout << "load model error\n";
		exit(0);
	}
}

SpecialPointsModel::~SpecialPointsModel()
{

}

void SpecialPointsModel::readyToWork()
{
	printInfo();	
}

bool SpecialPointsModel::loadModel(std::string str)
{
	std::ifstream infile(str);
	if (!infile.is_open())
	{
		return false;
	}
	int cnt;
	infile >> cnt;
	specialPoints.resize(cnt);
	for (int i = 0; i < cnt; ++i)
	{
		glm::vec3 pos, normal;
		float sdf;
		infile >> pos[0] >> pos[1] >> pos[2];
		infile >> normal[0] >> normal[1] >> normal[2];
		infile >> sdf;
		SpecialPoint sp;
		sp.normal = normal;
		sp.pos = pos;
		sp.sdf = glm::vec3(sdf);
		specialPoints[i] = sp;
	}
	infile.close();
	return true;
}

void SpecialPointsModel::printInfo()
{
	std::cout << "special points Cnt: " << specialPoints.size() << std::endl << std::endl;
}

void SpecialPointsModel::setupModel()
{	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, specialPoints.size()*sizeof(SpecialPoint), &specialPoints[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SpecialPoint), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SpecialPoint), (GLvoid*)offsetof(SpecialPoint, sdf));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void SpecialPointsModel::drawModel(Shader *shader)
{
	shader->Use();	
	glBindVertexArray(VAO);
	glPointSize(8);
	glDrawArrays(GL_POINTS,0, specialPoints.size());
	glBindVertexArray(0);
}