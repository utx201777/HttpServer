#include "segmentModel.h"

inline int getRandom(int r)
{
	return rand() % r;
}

SegmentModel::SegmentModel(std::string str)
{
	if (!loadModel(str))
	{
		std::cout << "load model error\n";
		exit(0);
	}
}

SegmentModel::~SegmentModel()
{

}

/*导入obj文件*/
bool SegmentModel::loadModel(std::string str)
{
	std::ifstream infs(str);
	if (!infs.is_open())
	{
		std::cerr << "open file error!\n";
		return false;
	}
	std::string tmp;
	glm::vec3 v;
	while (!infs.eof())
	{
		infs >> tmp;
		if (tmp == "v")
		{
			infs >> v[0] >> v[1] >> v[2];
			box_min.x = std::min(box_min.x, v[0]);
			box_min.y = std::min(box_min.y, v[1]);
			box_min.z = std::min(box_min.z, v[2]);
			box_max.x = std::max(box_max.x, v[0]);
			box_max.y = std::max(box_max.y, v[1]);
			box_max.z = std::max(box_max.z, v[2]);
			this->Position.push_back(v);
		}
		else if (tmp == "vt")
		{
			infs >> v[0] >> v[1] >> v[2];
			this->TextureCoord.push_back(v);
		}
		else if (tmp == "vn")
		{
			infs >> v[0] >> v[1] >> v[2];
			this->Normal.push_back(v);
		}
		else if (tmp == "g")
		{
			infs >> tmp;		
		}
		else if (tmp == "usemtl")
		{
			infs >> tmp;
		}
		else if (tmp == "f")
		{
			std::vector<std::string> vs(3);
			infs >> vs[0] >> vs[1] >> vs[2];
			glm::ivec3 iv;
			if (sscanf(vs[0].c_str(), "%d/%d/%d", &iv[0], &iv[1], &iv[2]) == 1)	// 无纹理坐标
			{
				glm::ivec3 p;
				for (int id = 0; id < 3;++id)
				{
					glm::ivec3 tmp_;
					sscanf(vs[id].c_str(), "%d//%d", &p[id], &tmp_.z);					
				}
				Triangles.push_back(p);
				//printf("%d %d %d\n", p.x, p.y,p.z);
			}
			else if (sscanf(vs[0].c_str(), "%d/%d/%d", &iv[0], &iv[1], &iv[2]) == 3)	// 三个坐标都有
			{
				glm::ivec3 p;
				for (int id = 0; id < 3; ++id)
				{
					glm::ivec3 tmp_;
					sscanf(vs[id].c_str(), "%d/%d/%d", &p[id], &tmp_.y, &tmp_.z);
				}
				Triangles.push_back(p);
				//printf("%d %d %d\n", p.x, p.y, p.z);
			}
			else
			{

			}
		}
		else
		{
			getline(infs, tmp);
		}
	}
	this->maxEdge = std::max(box_max.x - box_min.x, std::max(box_max.y - box_min.y, box_max.z - box_min.z));
	this->box_center = glm::vec3((box_max.x + box_min.x) / 2, (box_max.y + box_min.y) / 2, (box_max.z + box_min.z) / 2);
	infs.close();
	return true;
}

/*对点进行归一化*/
void SegmentModel::modelTransform()
{
	glm::mat4 modelMat;
	modelMat = glm::scale(modelMat, glm::vec3(0.5 / maxEdge));
	modelMat = glm::translate(modelMat, -glm::vec3(box_center.x, box_center.y, box_center.z));
	for (int i = 0; i < Position.size(); ++i)
	{
		glm::vec4 p = modelMat*glm::vec4(Position[i], 1.0f);
		Position[i] = glm::vec3(p.x, p.y, p.z);
	}
}

void SegmentModel::printInfo()
{
	printf("Points Cnt: %d\n", Position.size());
	printf("Triangles Cnt: %d\n", Triangles.size());
	printf("Segment Cnt: %d\n", segCnt);
	printf("max Edge: %f\n", maxEdge);
	printf("box_center: (%f,%f,%f)\n", box_center.x, box_center.y, box_center.z);
}

void SegmentModel::setupModel()
{
	SVAO = new GLuint[segCnt];
	SVBO = new GLuint[segCnt];
	glGenVertexArrays(segCnt, SVAO);
	glGenBuffers(segCnt, SVBO);

	for (int i = 0; i < segCnt; ++i)
	{
		auto v = segData[i];		
		float c = getRandom(10) / 10.0;
		std::vector<glm::vec3> color(v.size(), glm::vec3(c,c,1));
		glBindVertexArray(SVAO[i]);
		glBindBuffer(GL_ARRAY_BUFFER, SVBO[i]);

		glBufferData(GL_ARRAY_BUFFER, v.size()*sizeof(glm::vec3) + color.size()*sizeof(glm::vec3), NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, v.size()*sizeof(glm::vec3), &v[0]);
		glBufferSubData(GL_ARRAY_BUFFER, v.size()*sizeof(glm::vec3), color.size()*sizeof(glm::vec3), &color[0]);
		
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(v.size()*sizeof(glm::vec3)));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

void SegmentModel::drawModel(Shader *shader)
{
	shader->Use();

	for (int i = 0; i < segCnt; ++i)
	{
		glBindVertexArray(SVAO[i]);
		glDrawArrays(GL_TRIANGLES, 0, segData[i].size());
		glBindVertexArray(0);
	}
}

/*导入分割结果，按照三角形的索引*/
void SegmentModel::loadSegmentData(std::string str)
{
	std::ifstream infs(str);
	if (!infs.is_open())
	{
		std::cerr << "open segment file error!\n";
		exit(0);
	}
	int tmp;
	infs >> tmp;
	segCnt = tmp;
	segData.resize(tmp);
	segTrianglesIdx.resize(tmp);
	int idx = 0;
	while (infs >> tmp)
	{
		segTrianglesIdx[tmp].push_back(idx++);
	}
	infs.close();
}

/*处理三角形的索引到点中*/
void SegmentModel::treatSegmentData()
{
	for (int i = 0; i < segCnt; ++i)
	{
		auto &v = segTrianglesIdx[i];
		for (int j = 0; j < v.size(); ++j)
		{
			auto &tr = Triangles[v[j]];
			segData[i].push_back(Position[tr.x-1]);
			segData[i].push_back(Position[tr.y-1]);
			segData[i].push_back(Position[tr.z-1]);
		}
	}
}