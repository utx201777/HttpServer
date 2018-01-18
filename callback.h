#ifndef _CALL_BACK_
#define _CALL_BACK_

/************************************************************************
   created: 2018/01/17
   class: 
   author: linyi
   usage: 虚基类，用于鼠标键盘操作
************************************************************************/

class ICallbacks
{
public:
	// 鼠标操作
	virtual void MouseAction(double x, double y){};
	// 键盘操作
	virtual void KeyAction(int key,int action){};
	// 渲染场景
	virtual void RenderScene(){};

};

#endif