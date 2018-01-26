#pragma once

#include "model.h"

/************************************************************************
   created: 2018/01/25
   class: 
   author: linyi
   usage: ”√¿¥ª≠µ∆
************************************************************************/

class LightModel:public Model
{
public:
	LightModel(std::string str);
	~LightModel();
	virtual bool loadModel(std::string str);
	virtual void printInfo();
	virtual void setupModel();
	virtual void drawModel(Shader *shader);
	void loadPos(std::string str);
	void modelTransform(glm::mat4);
private:
	std::vector<glm::vec3> lightPos;
	std::vector<glm::vec3> Points;	
	std::vector<glm::ivec3> Triangles;	
};