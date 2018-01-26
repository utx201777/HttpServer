#pragma once
#include "model.h"
#include <cmath>
/************************************************************************
   created: 2018/01/23
   class: 
   author: linyi
   usage: 寻找最接近的特征点，进行匹配
************************************************************************/

struct Line
{
	glm::vec3 p1;
	glm::vec3 p2;
};

class NearestPairModel :public Model
{
public:
	NearestPairModel(std::vector<SpecialPoint> &sP1, std::vector<SpecialPoint> &sP2);
	~NearestPairModel();
	virtual bool loadModel(std::string str);
	virtual void printInfo();
	virtual void setupModel();
	virtual void drawModel(Shader *shader);
	void findNearestPair();
private:
	std::vector<SpecialPoint> sPoints1;
	std::vector<SpecialPoint> sPoints2;
	std::vector<glm::vec3> sLines;
	std::vector<glm::vec3> sColors;
	float weight = 0.8;
};