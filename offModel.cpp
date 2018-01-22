#include "offmodel.h"

offModel::offModel(std::string str)
{
	if (!loadModel(str))
	{
		std::cout << "load model error\n";
		exit(0);
	}
}

offModel::~offModel()
{

}

// 准备工作，如果输入文件名则存储特征点
void offModel::readyToWork()
{
	printInfo();		// 打印信息
	findTrianglesNormal();	// 计算三角形法线
	findAdjactRelation();	// 计算三角形连接性
	findDihedral();			// 计算二面角(需要先计算法线)	
}

// 导入函数：导入模型
bool offModel::loadModel(std::string str)
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
		this->pointsCnt = total.x;
		this->trianglesCnt = total.y;
		this->edgesCnt = total.z;
		this->Points.resize(total.x);
		this->Triangles.resize(total.y);
		// 读入顶点并且计算包围盒
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

	this->maxEdge = std::max(box_max.x - box_min.x, std::max(box_max.y - box_min.y, box_max.z - box_min.z));
	this->box_center = glm::vec3((box_max.x + box_min.x) / 2, (box_max.y + box_min.y) / 2, (box_max.z + box_min.z) / 2);
	return true;
}

// 打印信息
void offModel::printInfo()
{
	std::cout << "模型信息如下：\n";
	std::cout << "顶点数： " << this->pointsCnt << std::endl;
	std::cout << "三角形数： " << this->trianglesCnt << std::endl;
	std::cout << "包围盒max：" << this->box_max[0] << " " << this->box_max[1] << " " << this->box_max[2] << std::endl;
	std::cout << "包围盒min：" << this->box_min[0] << " " << this->box_min[1] << " " << this->box_min[2] << std::endl;
	std::cout << "包围盒中心：" << this->box_center[0] << " " << this->box_center[1] << " " << this->box_center[2] << std::endl;
	std::cout << "最长边：" << this->maxEdge << std::endl << std::endl;
}

// 导入函数：导入三角形SDF
void offModel::loadTriangleSDF(std::string filename)
{
	trianglesSDF.resize(trianglesCnt);
	// 读取sdf值
	int index = 0;
	std::ifstream infs(filename);
	if (!infs.is_open())
	{
		std::cout << "open file error!\n";
		exit(0);
	}
	int cnt;
	float p;
	infs >> cnt;
	for (int i = 0; i < cnt; ++i)
	{
		infs >> p;
		trianglesSDF[index++] = p;
	}
	infs.close();
}

// 计算函数：获得邻接关系
void offModel::findAdjactRelation()
{
	// 判断是否相邻
	auto isAdjacent = [](glm::ivec3 p1, glm::ivec3 p2)->bool
	{
		int cnt = 0;
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
				if (p1[i] == p2[j])
					cnt++;
		}
		if (cnt >= 2)
			return true;
		return false;
	};

	// 计算邻接关系
	for (int i = 0; i < trianglesCnt; ++i)
	{
		for (int j = 0; j < trianglesCnt; ++j)
		{
			if (isAdjacent(Triangles[i], Triangles[j]))
				adjactRelation[i].push_back(j);
		}
	}
}

// 计算函数：计算三角形法线
void offModel::findTrianglesNormal()
{
	trianglesNormal.resize(trianglesCnt);
	int index = 0;
	// 计算法线
	for (auto tr : Triangles)
	{
		glm::vec3 p1 = Points[tr[0]];
		glm::vec3 p2 = Points[tr[1]];
		glm::vec3 p3 = Points[tr[2]];
		glm::vec3 n1 = p2 - p1;
		glm::vec3 n2 = p3 - p2;
		glm::vec3 normal = glm::normalize(glm::cross(n1, n2));
		trianglesNormal[index++] = normal;
	}
}

// 计算函数：计算最大二面角，需要先计算邻接性
void offModel::findDihedral()
{
	Dihedral.resize(trianglesCnt);
	// 计算二面角
	for (int i = 0; i < trianglesCnt; ++i)
	{
		// 计算相邻的面片之间的法线差异
		float angleMax = 0;
		auto vec = adjactRelation[i];
		for (auto j : vec)
		{
			float cosValue = glm::dot(trianglesNormal[i], trianglesNormal[j]) / (glm::length(trianglesNormal[i])*glm::length(trianglesNormal[j]));
			angleMax = std::max(angleMax, acos(cosValue));
		}
		Dihedral[i] = angleMax;
	}
}

// 计算函数：寻找局部最大点
void offModel::findLocalMax()
{
	std::random_device rd;
	std::unordered_set<int> specialPoints;
	for (int i = 0; i < 1000; ++i)
	{
		int currentIdx = rd() % trianglesCnt;	// 从一个三角形随机出发
		std::unordered_set<int> sIndex;
		sIndex.insert(currentIdx);
		while (true)
		{
			auto vec = adjactRelation[currentIdx];
			float initSDF = 1;
			int minIdx = -1;
			for (auto idx : vec)		// 寻找相邻里的节点中的最小值
			{
				// 未遍历过
				if (specialPoints.count(idx) == 0 && sIndex.count(idx) == 0 && trianglesSDF[idx] < initSDF)
				{
					initSDF = trianglesSDF[idx];
					minIdx = idx;
				}
			}
			// 如果边上的点的最小值比当前点小，则更新当前点，注意判断可能minIdx未赋值
			if (minIdx != -1 && initSDF <= trianglesSDF[currentIdx])
			{
				currentIdx = minIdx;
				sIndex.insert(currentIdx);
			}
			else
			{
				break;
			}
		}
		if (Dihedral[currentIdx] > 0.5)
			specialPoints.insert(currentIdx);
	}

	// 点的位置是三角形三个点的均值
	for (auto it = specialPoints.begin(); it != specialPoints.end(); ++it)
	{
		glm::ivec3 v = Triangles[*it];
		glm::vec3 p = (Points[v[0]] + Points[v[1]] + Points[v[2]]);
		p = p / 3.0f;
		sPoints.push_back(p);
		sColor.push_back(glm::vec3(trianglesSDF[*it]));	// 用对应的sdf值	
		sNormal.push_back(trianglesNormal[*it]);
	}

	specialPointsCnt = sPoints.size();
	std::cout << "special Points Cnt :" << specialPointsCnt << std::endl<<std::endl;
}

// 功能函数：保存sdf值到文件
void offModel::saveSpecialPoints2File(std::string str)
{
	std::ofstream onfs(str);
	onfs << sPoints.size() << std::endl;
	for (int i = 0; i < sPoints.size(); ++i)
	{
		onfs << sPoints[i][0] << " " << sPoints[i][1] << " " << sPoints[i][2] << std::endl;
		onfs << sNormal[i][0] << " " << sNormal[i][1] << " " << sNormal[i][2] << std::endl;
		onfs << sColor[i][0] << std::endl;
	}
	onfs.close();
}

// 传输函数：使用EBO
void offModel::setupModel()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	
	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, Points.size()*sizeof(glm::vec3), &Points[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Triangles.size()*sizeof(glm::ivec3), &Triangles[0][0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

// 传输函数：展开三角形，不使用EBO的结果，绘制SDF【使用offshader】
void offModel::setupModel_SDF()
{
	std::vector<glm::vec3> pointsWithoutEBO(trianglesCnt * 3);
	std::vector<float> pointsSDF(trianglesCnt * 3);
	// 展开三角形
	auto index = 0;
	for (auto tr : Triangles)
	{
		pointsWithoutEBO[index++] = Points[tr[0]];
		pointsWithoutEBO[index++] = Points[tr[1]];
		pointsWithoutEBO[index++] = Points[tr[2]];
	}

	// 读取sdf值
	index = 0;
	for (int i = 0; i < trianglesCnt; ++i)
	{
		pointsSDF[index++] = trianglesSDF[i];
		pointsSDF[index++] = trianglesSDF[i];
		pointsSDF[index++] = trianglesSDF[i];
	}

	// 传输数据
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, pointsWithoutEBO.size()*sizeof(glm::vec3) + pointsSDF.size()*sizeof(float), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, pointsWithoutEBO.size()*sizeof(glm::vec3), &pointsWithoutEBO[0]);
	glBufferSubData(GL_ARRAY_BUFFER, pointsWithoutEBO.size()*sizeof(glm::vec3), pointsSDF.size()*sizeof(float), &pointsSDF[0]);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, (void*)(pointsWithoutEBO.size()*sizeof(glm::vec3)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

// 传输函数：绘制法线
void offModel::setupModel_Normal()
{
	std::vector<glm::vec3> pointsWithoutEBO(trianglesCnt * 3);
	std::vector<glm::vec3> pointsNormal(trianglesCnt * 3);
	// 展开三角形
	auto index = 0;
	for (auto tr : Triangles)
	{
		pointsWithoutEBO[index++] = Points[tr[0]];
		pointsWithoutEBO[index++] = Points[tr[1]];
		pointsWithoutEBO[index++] = Points[tr[2]];
	}
	index = 0;
	// 计算法线
	for (int i = 0; i < trianglesCnt;++i)
	{
		glm::vec3 normal = trianglesNormal[i];
		normal /= 2;
		normal += glm::vec3(0.5);	// 矫正范围		
		pointsNormal[index++] = normal;
		pointsNormal[index++] = normal;
		pointsNormal[index++] = normal;
	}

	// 传输数据
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, pointsWithoutEBO.size()*sizeof(glm::vec3) + pointsNormal.size()*sizeof(glm::vec3), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, pointsWithoutEBO.size()*sizeof(glm::vec3), &pointsWithoutEBO[0]);
	glBufferSubData(GL_ARRAY_BUFFER, pointsWithoutEBO.size()*sizeof(glm::vec3), pointsNormal.size()*sizeof(glm::vec3), &pointsNormal[0]);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(pointsWithoutEBO.size()*sizeof(glm::vec3)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

// 传输函数：提取边缘，需要先计算二面角
void offModel::setupModel_Edge()
{
	std::vector<glm::vec3> pointsWithoutEBO(trianglesCnt * 3);	// 三角形展开
	std::vector<glm::vec3> pointsNormal(trianglesCnt * 3);	// 点的法线
	std::vector<glm::vec3> triangleNormal(trianglesCnt);	// 三角形法线
	std::vector<glm::vec3> triangleAngle(trianglesCnt * 3);	// 二面角（以点记录）
	std::vector<float> pointsSDF(trianglesCnt * 3);			// sdf

	// 展开三角形
	auto index = 0;
	for (auto tr : Triangles)
	{
		pointsWithoutEBO[index++] = Points[tr[0]];
		pointsWithoutEBO[index++] = Points[tr[1]];
		pointsWithoutEBO[index++] = Points[tr[2]];
	}

	// 计算法线
	index = 0;
	int triangleIdx = 0;
	for (int i = 0; i < trianglesCnt;++i)
	{
		glm::vec3 normal = trianglesNormal[i];
		normal /= 2;
		normal += glm::vec3(0.5);		
		pointsNormal[index++] = normal;
		pointsNormal[index++] = normal;
		pointsNormal[index++] = normal;
		triangleNormal[triangleIdx++] = normal;
	}

	// 读取sdf值
	index = 0;	
	for (int i = 0; i < trianglesCnt; ++i)
	{
		pointsSDF[index++] = trianglesSDF[i];
		pointsSDF[index++] = trianglesSDF[i];
		pointsSDF[index++] = trianglesSDF[i];
	}

	// 计算二面角
	for (int i = 0; i < trianglesCnt; ++i)
	{
		// 计算相邻的面片之间的法线差异
		float angleMax = Dihedral[i];
		angleMax /= 3.1416;		// 归一化		
		triangleAngle[i * 3] = glm::vec3(angleMax, 0, pointsSDF[i * 3]);
		triangleAngle[i * 3 + 1] = glm::vec3(angleMax, 0, pointsSDF[i * 3 + 1]);
		triangleAngle[i * 3 + 2] = glm::vec3(angleMax, 0, pointsSDF[i * 3 + 2]);
	}

	// 传输数据
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, pointsWithoutEBO.size()*sizeof(glm::vec3) + triangleAngle.size()*sizeof(glm::vec3), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, pointsWithoutEBO.size()*sizeof(glm::vec3), &pointsWithoutEBO[0]);
	glBufferSubData(GL_ARRAY_BUFFER, pointsWithoutEBO.size()*sizeof(glm::vec3), triangleAngle.size()*sizeof(glm::vec3), &triangleAngle[0]);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(pointsWithoutEBO.size()*sizeof(glm::vec3)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

// 传输函数：特征点
void offModel::setupModel_SpecialPoints()
{
	glGenVertexArrays(1, &pVAO);
	glGenBuffers(1, &pVBO);

	glBindVertexArray(pVAO);

	glBindBuffer(GL_ARRAY_BUFFER, pVBO);
	glBufferData(GL_ARRAY_BUFFER, sPoints.size()*sizeof(glm::vec3) + sColor.size()*sizeof(glm::vec3), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sPoints.size()*sizeof(glm::vec3), &sPoints[0]);
	glBufferSubData(GL_ARRAY_BUFFER, sPoints.size()*sizeof(glm::vec3), sColor.size()*sizeof(glm::vec3), &sColor[0]);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sPoints.size()*sizeof(glm::vec3)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

// 绘制函数：使用EBO
void offModel::drawModel(Shader * shader)
{
	shader->Use();
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, Triangles.size()*3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

// 绘制函数：不使用EBO
void offModel::drawModel_withoutEBO(Shader * shader)
{	
	shader->Use();
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0,trianglesCnt*3);
	glBindVertexArray(0);
}

// 绘制函数：绘制点
void offModel::drawModel_specialPoints(Shader * shader)
{
	shader->Use();
	glBindVertexArray(pVAO);
	glPointSize(8);
	glDrawArrays(GL_POINTS, 0, sPoints.size());
	glBindVertexArray(0);
}