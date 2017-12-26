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
	//float z, dz;//�������������ֵ
	int polyID;
	//bool flag;
};
struct Poly {
	int polyID;
	float a, b, c, d;
	int dy;//��Խɨ���ߵĸ���
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
	//�߱�
	vector<vector<Edge>> ETable;
	//����α�
	vector<vector<Poly>> PTable;
	//��߱�
	vector<Edge> AET;
	//�����α�
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