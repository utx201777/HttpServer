#pragma once
#ifndef _GL_CALLBACK_
#define _GL_CALLBACK_

#include <iostream>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <string>
#include "callback.h"

/************************************************************************
   created: 2018/01/18
   class: 
   author: linyi
   usage: OpenGL初始化及控制基类，变量定义在cpp中
************************************************************************/

inline void printText(std::string str);

void keyfunc(GLFWwindow* window, int key, int scancode, int action, int mode);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

bool initOpenGL(int WIDTH, int HEIGHT);

void glfwInitCallback();

void glfwRun(ICallbacks * c);
#endif