#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <glm/glm.hpp>
#include <map>
#include <set>
#include <ctime>
#include <algorithm>  
#include <iterator>

/************************************************************************
   created: 2018/02/01
   class: 
   author: linyi
   usage: 对obj文件进行分割，按照点的连通性进行分割
************************************************************************/
namespace Segment
{
	using namespace std;
	inline int getRandom(int r);

	struct Point3
	{
		int posIdx;
		int textureIdx;
		int normalIdx;
		Point3() = default;
		Point3(int i, int j, int k) :posIdx(i), textureIdx(j), normalIdx(k){}
		Point3(const Point3 &p) 
		{
			posIdx = p.posIdx;
			textureIdx = p.textureIdx;
			normalIdx = p.normalIdx;
		}
		Point3 & operator=(const Point3 & p)
		{
			posIdx = p.posIdx;
			textureIdx = p.textureIdx;
			normalIdx = p.normalIdx;
			return *this;
		}
	};

	istream & operator>>(istream & infs, Point3 & p);

	ostream & operator<<(ostream & onfs, const Point3 & p);

	struct Triangle3
	{
		vector<Point3> vp;
		int meshId;
		int cluster;
		Triangle3() :cluster(-1){}
	};

	struct MeshInfo
	{
		int meshId;
		string meshName;
		string mMatrial;
	};

	class SegmentProcess
	{
	public:
		string filename;
		/*存取原始的数据*/
		vector<glm::vec3> Normal;
		vector<glm::vec3> TextureCoord;
		vector<glm::vec3> Position;
		/*三角形索引，类别*/
		vector<Triangle3> Triangles;
		/*类别与材质的对应关系*/
		map<int, MeshInfo> mMatrial;	// 记录meshid映射关系
		/*存储每个类别下的顶点*/
		vector<set<int> > point3Cluster;
		vector<set<int> > triangle3Cluster;
		set<int> unsolveTriangles;			// 未分类的三角形
		int meshId = -1;
		int initK;

		SegmentProcess(string fname,int id);
		~SegmentProcess();

		/*初始化操作：导入模型*/
		void loadModel();

		/*聚类函数：调用聚类操作*/
		void clusterModel(string filename);

		/*聚类函数：生成原始聚类的中心*/
		void genClusterSeed(int k);

		/*聚类函数：生成一个中心*/
		bool genClusterOrder();

		/*聚类函数：具体进行聚类操作*/
		void cluster();

		/*聚类操作：合并集合*/
		void unionCluster();

		/*输出操作：将类别顶点处理成三角形的索引*/
		void clusterTriangle(string filename, string clustername);

		/*输出操作：打印每个类别下的顶点*/
		void printCluster(string filename);

		/*功能函数：打印模型信息*/
		void printModel(ostream & f);
	private:
	};

}
