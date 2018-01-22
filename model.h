#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include "shader.h"

/************************************************************************
   created: 2018/01/18
   class: 
   author: linyi
   usage: model的抽象基类，抽象出以下几个方法：
		1. 导入模型
		2. 打印信息
		3. 传输数据
		4. 绘制模型
		5. 计算连接性
************************************************************************/
class Model
{
public:
	virtual bool loadModel(std::string str) =0;
	virtual void printInfo()=0;
	virtual void setupModel()=0;	
	virtual void drawModel(Shader *shader)=0;	
protected:
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;	
};