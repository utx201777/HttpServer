#pragma once

#include "model.h"
#include <algorithm>

/************************************************************************
   created: 2018/02/01
   class: GenerateModel
   author: linyi
   usage: 生成三视图
************************************************************************/

class GenerateModel :public Model
{
public:
	GenerateModel(std::string);
	~GenerateModel();
	virtual bool loadModel(std::string str);
	virtual void printInfo();
	virtual void setupModel();
	virtual void drawModel(Shader *shader);
	void modelTransform();
private:
	std::vector<glm::vec3> Position;
	std::vector<glm::ivec3> Triangles;
	glm::vec3 box_min = glm::vec3(1e10);
	glm::vec3 box_max = glm::vec3(-1e10);
	float maxEdge;
	glm::vec3 box_center;
};