#pragma once

#include "model.h"
#include <algorithm>
/************************************************************************
   created: 2018/02/01
   class: SegmentModel
   author: linyi
   usage: 导入分割的模型（obj）,展示分割的效果
************************************************************************/

inline int getRandom(int r);

class SegmentModel:public Model
{
public:
	
	SegmentModel(std::string);
	~SegmentModel();
	virtual bool loadModel(std::string str);
	virtual void printInfo();
	virtual void setupModel();
	virtual void drawModel(Shader *shader);

	void loadSegmentData(std::string str);
	void treatSegmentData();
	void modelTransform();
private:
	float maxEdge;
	glm::vec3 box_center;
	GLuint * SVAO;
	GLuint * SVBO;
	GLuint * SEBO;
	/*存取原始的数据*/
	std::vector<glm::vec3> Normal;
	std::vector<glm::vec3> TextureCoord;
	std::vector<glm::vec3> Position;
	std::vector<glm::ivec3> Triangles;
	/*根据类别划分的顶点，三角形索引*/
	int segCnt=0;
	std::vector<std::vector<glm::vec3> > segData;
	std::vector<std::vector<int> > segTrianglesIdx;
	glm::vec3 box_min = glm::vec3(1e10);
	glm::vec3 box_max = glm::vec3(-1e10);
protected:

};