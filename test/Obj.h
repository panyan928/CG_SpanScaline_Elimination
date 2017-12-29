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
	vector< Coord<float> > vertex;//����
	vector< Coord<float> > normal;//����
	vector<vector<int>> face;
	int n_vertex = 0;//��������
	int n_face = 0; //������
	Coord<float> center=Coord<float>(0,0,0);//������������
	float minx = INF;
	float miny = INF;
	float maxx = 0;
	float maxy = 0;

	int load(string);
	//���㷨����
	void CalcProp();
	//ƽ�ƶ��㣬��ģ���Ƶ������м�
	void scale(int ,int );
	void info();
};