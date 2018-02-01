#pragma once

#include "segment.h"

//using namespace Segment;
namespace Segment
{
	istream & operator>>(istream & infs, Point3 & p)
	{
		infs >> p.posIdx >> p.textureIdx >> p.normalIdx;
		return infs;
	}


	ostream & operator<<(ostream & onfs, const Point3 & p)
	{
		onfs << p.posIdx << p.textureIdx << p.normalIdx;
		return onfs;
	}

	inline int getRandom(int r)
	{
		return rand() % r;
	}

	SegmentProcess::SegmentProcess(string fname,int id) :initK(id),filename(fname)
	{
		loadModel();
	}

	SegmentProcess::~SegmentProcess()
	{

	}

	/*初始化操作：导入模型*/
	void SegmentProcess::loadModel()
	{
		ifstream infs(filename);
		if (!infs.is_open())
		{
			cerr << "open file error!\n";
			exit(0);
		}
		string tmp;
		glm::vec3 v;
		while (!infs.eof())
		{
			infs >> tmp;
			if (tmp == "v")
			{
				infs >> v[0] >> v[1] >> v[2];
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
				meshId++;		// 改变meshid
				infs >> tmp;
				mMatrial[meshId].meshId = meshId;
				mMatrial[meshId].meshName = tmp;
			}
			else if (tmp == "usemtl")
			{
				infs >> tmp;
				mMatrial[meshId].mMatrial = tmp;
			}
			else if (tmp == "f")
			{
				vector<string> vs(3);
				infs >> vs[0] >> vs[1] >> vs[2];
				glm::ivec3 iv;
				if (sscanf(vs[0].c_str(), "%d/%d/%d", &iv[0], &iv[1], &iv[2]) == 1)	// 无纹理坐标
				{
					vector<Point3> vp;
					for (auto s : vs)
					{
						Point3 p(-1, -1, -1);
						sscanf(s.c_str(), "%d//%d", &p.posIdx, &p.normalIdx);
						vp.push_back(p);
					}
					Triangle3 t;
					t.meshId = meshId;		// 设定这个三角形的类别
					t.vp = vp;
					Triangles.push_back(t);
				}
				else if (sscanf(vs[0].c_str(), "%d/%d/%d", &iv[0], &iv[1], &iv[2]) == 3)	// 三个坐标都有
				{
					vector<Point3> vp;
					for (auto s : vs)
					{
						Point3 p(-1, -1, -1);
						sscanf(s.c_str(), "%d/%d/%d", &p.posIdx, &p.textureIdx, &p.normalIdx);
						vp.push_back(p);
					}
					Triangle3 t;
					t.meshId = meshId;
					t.vp = vp;
					Triangles.push_back(t);
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
		infs.close();
	}

	/*聚类函数：调用聚类操作*/
	void SegmentProcess::clusterModel(string filename)
	{
		genClusterSeed(initK);
		cluster();
		printCluster(filename);
	}

	/*聚类函数：生成原始聚类的中心*/
	void SegmentProcess::genClusterSeed(int k)
	{
		int cnt = 0;
		printf("k:%d\n", k);
		int size = Triangles.size();
		for (int i = 0; i < k; ++i)
		{
			int idx = getRandom(size);
			if (Triangles[idx].cluster != -1)		// 已经选过了，退出
				continue;
			set<int> si;
			si.insert(Triangles[idx].vp[0].posIdx);
			si.insert(Triangles[idx].vp[1].posIdx);
			si.insert(Triangles[idx].vp[2].posIdx);
			point3Cluster.push_back(si);
			Triangles[idx].cluster = point3Cluster.size();		// 设定类别，标志这个三角形已经选过
			cnt++;
		}
		for (int i = 0; i < Triangles.size(); ++i)		// 将未分类放入到一个set中
		{
			if (Triangles[i].cluster == -1)
			{
				unsolveTriangles.insert(i);
			}
		}
		printf("cluster size: %d\n", cnt);
		printf("unsolveTriangles size: %d\n", unsolveTriangles.size());
		unionCluster();
		printf("generator complete!\n");
	}

	/*聚类函数：生成新的一个中心*/
	bool SegmentProcess::genClusterOrder()
	{
		printf("unsolveTrianglesCnt: %d\n", unsolveTriangles.size());
		if (unsolveTriangles.size() == 0)		// 已经没有未分类的
			return false;
		int idx = *(unsolveTriangles.begin());
		// 处理类别
		set<int> si;
		si.insert(Triangles[idx].vp[0].posIdx);
		si.insert(Triangles[idx].vp[1].posIdx);
		si.insert(Triangles[idx].vp[2].posIdx);
		point3Cluster.push_back(si);
		Triangles[idx].cluster = point3Cluster.size();		// 设定类别
		// 删除该元素
		unsolveTriangles.erase(idx);
		return true;
	}

	/*聚类函数：具体进行聚类操作*/
	void SegmentProcess::cluster()
	{
		while (true)
		{
			while (true)
			{
				bool f = false;
				set<int> clusterIdx;
				// 从未分类的三角形中选出一个
				for (auto it = unsolveTriangles.begin(); it != unsolveTriangles.end(); ++it)
				{
					int idx = *it;
					Triangle3 tr = Triangles[idx];		// 对应的三角形			
					for (int i = 0; i < point3Cluster.size(); ++i)
					{
						set<int> &s = point3Cluster[i];	// 每一个类别
						if (s.size() == 0)
							continue;
						set<int> p;						// 将该三角形的三个点放到集合之中，进行求交集
						p.insert(tr.vp[0].posIdx);
						p.insert(tr.vp[1].posIdx);
						p.insert(tr.vp[2].posIdx);
						set<int> s3;
						set_intersection(s.begin(), s.end(), p.begin(), p.end(), inserter(s3, s3.begin()));
						if (s3.size() > 0)		// 有交集
						{
							// 更新cluster
							set_union(s.begin(), s.end(), p.begin(), p.end(), inserter(s, s.begin()));
							f = true;
							Triangles[idx].cluster = i;	// 设置类别标志
							clusterIdx.insert(idx);	// 用于记录哪一些已经分类了，在未分类集合中去除
							break;
						}
					}
				}
				if (f)		// 这一轮有更新，更新未分类的三角形集合
				{
					set<int> tmp;
					set_difference(unsolveTriangles.begin(), unsolveTriangles.end(), clusterIdx.begin(), clusterIdx.end(), inserter(tmp, tmp.begin()));
					unsolveTriangles = tmp;
					unionCluster();		// 更新集合
				}
				else
				{
					break;	// 如果没有一个分类成功，就要再产生一个分类
				}
			}
			if (!genClusterOrder())
				break;
		}
	}

	/*聚类操作：合并集合*/
	void SegmentProcess::unionCluster()
	{
		for (int i = 0; i < point3Cluster.size() - 1; ++i)
		{
			if (point3Cluster[i].size() == 0)
				continue;
			for (int j = i + 1; j < point3Cluster.size(); ++j)
			{
				set<int> tmp;
				set_intersection(point3Cluster[i].begin(), point3Cluster[i].end(), point3Cluster[j].begin(), point3Cluster[j].end(), inserter(tmp, tmp.begin()));
				if (tmp.size() > 0)	// 如果有交集
				{
					set_union(point3Cluster[i].begin(), point3Cluster[i].end(), point3Cluster[j].begin(), point3Cluster[j].end(), inserter(point3Cluster[i], point3Cluster[i].begin()));
					point3Cluster[j].clear();
				}
			}
		}
	}

	/*输出操作：将类别顶点处理成三角形的索引*/
	void SegmentProcess::clusterTriangle(string filename, string clustername)
	{
		vector<set<int> > pointsCluster;
		ifstream infs(filename);
		int clusterCnt;
		infs >> clusterCnt;
		for (int i = 0; i < clusterCnt; ++i)
		{
			int pointsCnt;
			infs >> pointsCnt;
			set<int> v;
			int tmp;
			for (int j = 0; j < pointsCnt; ++j)
			{
				infs >> tmp;
				v.insert(tmp);
			}
			pointsCluster.push_back(v);		// 存储每一类的顶点
		}
		infs.close();
		ofstream onfs(clustername);
		onfs << pointsCluster.size() << endl;
		for (int i = 0; i < Triangles.size(); ++i)
		{
			Triangle3 tr = Triangles[i];
			set<int> si;
			si.insert(tr.vp[0].posIdx);
			si.insert(tr.vp[1].posIdx);
			si.insert(tr.vp[2].posIdx);
			for (int j = 0; j < pointsCluster.size(); ++j)
			{
				set<int> &tmps = pointsCluster[j];
				set<int> s3;
				set_intersection(tmps.begin(), tmps.end(), si.begin(), si.end(), inserter(s3, s3.begin()));
				if (s3.size() > 0)		// 有交集，输出类别
				{
					onfs << j << endl;
					break;
				}
			}
		}
		onfs.close();
	}

	/*输出操作：打印每个类别下的顶点*/
	void SegmentProcess::printCluster(string filename)
	{
		int cnt = 0;
		FILE * f = fopen(filename.c_str(), "w");
		/*统计数量*/
		for (int i = 0; i < point3Cluster.size(); ++i)
		{
			set<int> &tmp = point3Cluster[i];
			if (tmp.size() == 0)
				continue;
			++cnt;
		}
		fprintf(f, "%d\n", cnt);
		for (int i = 0; i < point3Cluster.size(); ++i)
		{
			set<int> &tmp = point3Cluster[i];
			if (tmp.size() == 0)
				continue;
			fprintf(f, "%d\n", tmp.size());
			for (auto it = tmp.begin(); it != tmp.end(); ++it)
			{
				fprintf(f, "%d ", *it);
			}
			fprintf(f, "\n");
		}
		fclose(f);
		printf("cluster cnt: %d\n", cnt);
	}

	/*功能函数：打印模型信息*/
	void SegmentProcess::printModel(ostream & f)
	{
		for (auto it = mMatrial.begin(); it != mMatrial.end(); it++)
		{
			f << it->first << endl;
			f << it->second.meshId << " " << it->second.meshName << " " << it->second.mMatrial << endl;
		}
		f << "Triangles count: " << this->Triangles.size() << endl;
		f << "Position count: " << this->Position.size() << endl;
		f << "Normal count: " << this->Normal.size() << endl;
		f << "TextureCoord count: " << this->TextureCoord.size() << endl;
	}
}

