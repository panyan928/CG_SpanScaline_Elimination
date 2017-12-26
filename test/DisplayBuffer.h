#pragma once
#include <glut.h>
#include "Obj.h"
#include "Scanline.h"


//用于显示窗口
class DisplayBuffer {
public:
	int width, height;
	
	DisplayBuffer(){}
	DisplayBuffer(int _width,int _height):width(_width),height(_height){}
};