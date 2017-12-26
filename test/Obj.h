#pragma once
#include<iostream>
#include<fstream>
#include<vector>
#include"Coord.h"
#include<string>

#define Color unsigned char
#define INF 99999
using namespace std;

class Obj
{
public:
	Obj(const char* file_path);
	Obj(){}

	vector< Coord<float> > vertex;//顶点
	vector< Coord<float> > normal;//法线
	vector<vector<int>> face;
	int n_vertex = 0;//顶点数量
	int n_face = 0; //面数量
	Coord<float> center;//物体中心坐标
	float minx = INF;
	float miny = INF;
	float maxx = 0;
	float maxy = 0;

	//计算法向量
	void CalcProp();
	//平移顶点，将模型移到窗口中间
	void scale(int ,int );
};