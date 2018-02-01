#ifndef _SEGMENT_SAMPLE_
#define _SEGMENT_SAMPLE_

#include "callback.h"
#include "camera.h"
#include "shader.h"
#include "glCallback.h"
#include "segmentModel.h"
#include "generateModel.h"

/************************************************************************
   created: 2018/02/01
   class: SegmentSample
   author: linyi
   usage: 用来查看分割的效果
************************************************************************/

class SegmentSample :public ICallbacks
{
public:
	SegmentSample();
	virtual ~SegmentSample();
	virtual void MouseAction(double x, double y);
	virtual void KeyAction(int key, int action);
	virtual void RenderScene();
	void sampleInit();
	void sampleRun();
	void drawModel();
	void treatKey();
private:
	Camera * camera;
	Shader * shader;
	SegmentModel *segModel;
	GenerateModel * genModel;
	bool firstMouse;
	double lastX, lastY;
	int width, height;
	GLfloat deltaTime;
	GLfloat lastFrame;
	std::vector<bool> key;
};


#endif