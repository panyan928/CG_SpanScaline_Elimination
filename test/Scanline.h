#pragma once
#include "Obj.h"
#include "Coord.h"
#include<opencv2\opencv.hpp>
using namespace cv;
struct Edge
{
	float x;
	float dy;
	float dx;
	//float z, dz;//用增量法求深度值
	int polyID;
	//bool flag;
};
struct Poly {
	int polyID;
	float a, b, c, d;
	int dy;//跨越扫描线的个数
	Color color;
	//bool flag; //in/out
};
struct ActivePoly
{
	Poly poly;
	bool flag;
	vector<Edge> ActiveEdge;
};
class Scanline
{
public:
	//边表
	vector<vector<Edge>> ETable;
	//多边形表
	vector<vector<Poly>> PTable;
	//活化边表
	vector<Edge> AET;
	//活化多边形表
	vector<ActivePoly> APT;
	int height;
	int width;
	Scanline(int _width,int _height):width(_width),height(_height){}

	void BuildTable(Obj &obj);
	void UpdateActiveEdge();
	void UpdateActivePoly();
	void SortActiveEdge();
	void ComputeBuffer(int, vector<Color> &, Mat &image);
	void scan(Obj &obj, vector<vector<Color>> &,Mat &);
	int UpdateFlagByID(int id);
	Color pixelByID(int,float ,float);
};