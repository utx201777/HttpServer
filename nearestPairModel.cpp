#include "nearestPairModel.h"

NearestPairModel::NearestPairModel(std::vector<SpecialPoint> &sP1,std::vector<SpecialPoint> &sP2)
{
	sPoints1 = sP1;
	sPoints2 = sP2;
}

NearestPairModel::~NearestPairModel()
{

}

bool NearestPairModel::loadModel(std::string str)
{
	return true;
}

void NearestPairModel::printInfo()
{
	std::cout << "points set 1 Cnt: " << sPoints1.size() << std::endl;
	std::cout << "points set 2 Cnt: " << sPoints2.size() << std::endl;
}

void NearestPairModel::findNearestPair()
{
	for (int i = 0; i < sPoints1.size(); ++i)
	{
		auto tmp1 = sPoints1[i];
		float maxRes = 0;
		Line sL;		// 存储最近线
		sL.p1 = sL.p2 = tmp1.pos;
		for (int j = 0; j < sPoints2.size(); ++j)
		{
			auto tmp2 = sPoints2[j];
			auto cosValue = (glm::dot(tmp1.normal, tmp2.normal) / (glm::length(tmp1.normal)*glm::length(tmp2.normal)));
			auto sita = acos(cosValue)/3.14;		// 角度归一化
			auto tmpScore = (1 - sita)*weight + (1-abs(tmp1.sdf[0] - tmp2.sdf[0]))*weight;
			if (tmpScore > maxRes)
			{
				maxRes = tmpScore;
				sL.p2 = tmp2.pos;
			}
		}
		sLines.push_back(sL.p1);
		sLines.push_back(sL.p2);
	}
	sColors.resize(sLines.size(), glm::vec3(1));
}

void NearestPairModel::setupModel()
{
	// 传输数据
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sLines.size()*sizeof(glm::vec3) + sColors.size()*sizeof(glm::vec3), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sLines.size()*sizeof(glm::vec3), &sLines[0]);
	glBufferSubData(GL_ARRAY_BUFFER, sLines.size()*sizeof(glm::vec3), sColors.size()*sizeof(glm::vec3), &sColors[0]);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sLines.size()*sizeof(glm::vec3)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void NearestPairModel::drawModel(Shader *shader)
{
	shader->Use();
	glBindVertexArray(VAO);	
	glDrawArrays(GL_LINES, 0, sLines.size());
	glBindVertexArray(0);
}