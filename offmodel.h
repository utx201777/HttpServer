#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <glm/glm.hpp>
#include <cmath>
#include <random>
#include <unordered_set>
#include "shader.h"
#include "model.h"


/************************************************************************
   created: 2018/01/18
   class: 
   author: linyi
   usage: 导入off文件的model，继承于Model基类
************************************************************************/
class offModel:public Model
{
public:
	float maxEdge;
	glm::vec3 box_center;
	offModel(std::string str);
	~offModel();
	virtual bool loadModel(std::string str);
	virtual void printInfo();
	virtual void setupModel();	
	virtual void drawModel(Shader *shader);
	
	void readyToWork();		

	void setupModel_SDF();		// 转化为列表，不使用EBO		
	void setupModel_Normal();	// 使用法线
	void setupModel_Edge();		// 提取边缘，同时话SDF在另外一个通道
	void setupModel_SpecialPoints();	// 特征点

	void loadTriangleSDF(std::string);
	void findLocalMax();	// 寻找局部最大点
	void saveSpecialPoints2File(std::string);

	void drawModel_withoutEBO(Shader *shader);
	void drawModel_specialPoints(Shader *shader);	

	
private:
	std::vector<glm::vec3> Points;	// 点信息
	std::vector<glm::ivec3> Triangles;	// 三角面片
	std::vector<float> trianglesSDF;	// 三角形SDF
	std::vector<glm::vec3> trianglesNormal;	// 三角形法线
	std::vector<float> Dihedral;	// 二面角
	std::vector<glm::vec3> sPoints;		// 特征点
	std::vector<glm::vec3> sColor;
	std::vector<glm::vec3> sNormal;
	int pointsCnt;
	int trianglesCnt;
	int edgesCnt;
	int specialPointsCnt;
	GLuint pVAO;		// 画点的VAO
	GLuint pVBO;
	glm::vec3 box_min = glm::vec3(1e10);
	glm::vec3 box_max = glm::vec3(-1e10);	
	std::unordered_map<int, std::vector<int> > adjactRelation;	// 存储邻接关系

	void findTrianglesNormal();	// 计算三角形法线
	void findAdjactRelation();	// 计算连接性
	void findDihedral();		// 计算二面角
};