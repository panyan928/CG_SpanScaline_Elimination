#include<iostream>
#include<io.h>
#include<Windows.h>
#include <glut.h>
#include"Obj.h"
#include"Coord.h"
#include"Scanline.h"
#include<ctime>
//#include<opencv2/opencv.hpp>
//帧缓存，存储每位的像素值
vector<vector<Color>> buffer;
vector<string> files;
//窗口的长宽
int width;
int height;
Obj obj;
Scanline eliminate;

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);
	clock_t start = clock();
	obj.scale(width, height);
	eliminate.setSize(width, height);
	eliminate.BuildTable(obj);
	eliminate.scan(obj, buffer);
	clock_t stop = clock();
	cout << "The time is " << float(stop - start) << "ms" << endl;
	glBegin(GL_POINTS);
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			//显示这里一直有问题，只显示白色
			//DisplayMode 设置为GLUT_SINGLE，Color为3f,所以像素值应该改为0-1之间
			//color3ub 像素值为无符号整形
			glColor3ub(buffer[y][x], buffer[y][x] , buffer[y][x]);
			glVertex2i(x, y);
		}
	}
	glEnd();

	glFinish();
}
void reshape(int w,int h)
{
	glViewport(0, 0, w, h);
	width = w;
	height = h;
	
	//display();
}

int getModels()
{
	WIN32_FIND_DATA ffd;
	LARGE_INTEGER filesize;
	char cDir[MAX_PATH];
	
	size_t length_of_arg;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0; 
	GetModuleFileNameA(NULL, cDir, MAX_PATH);
	string sDir = cDir;
	sDir = sDir.substr(0, sDir.find_last_of('\\'));
	//cout << sDir << endl;
	sDir = sDir.substr(0, sDir.find_last_of('\\'));
	//cout << sDir << endl;
	long   hFile = 0;
	//文件信息
	struct _finddata_t fileinfo;
	string p;
	//找到当前运行文件夹下的所有obj文件
	if ((hFile = _findfirst(p.assign(sDir).append("\\models\\*.obj").c_str(), &fileinfo)) != -1)
	{
		do
		{
			files.push_back(p.assign(sDir).append("\\models\\").append(fileinfo.name));
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	for (int i = 0; i < files.size(); i++)
	{
		cout << i << " " << files[i].substr(files[i].find_last_of('\\')+1) << endl;
	}
	cout << "Please choose one model's number:";
	int number;
	cin >> number;
	while (number < 0 || number >= files.size())
	{
		cout << "Input Error\nPlease choose again:";
		cin >> number;
	}
	return number;
}

int main(int argc,char** argv)
{
	buffer.clear();
	//getModels();

	if(obj.load(files[getModels()])==0) return 0;
	//显示模型信息
	obj.info();
	width = 1000;
	height = 800;
	
	//cv::Mat image = cv::Mat::zeros(height, width, CV_8UC1);
	//gl初始化
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(width, height);
	glutCreateWindow("CGHW1");
	/*clock_t start = clock();
	eliminate.setSize(width, height);
	obj.scale(width, height);
	eliminate.BuildTable(obj);
	eliminate.scan(obj, buffer);
	clock_t stop = clock();
	cout << "The time is " << float(stop - start) << "ms" << endl;*/
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();
	
	/*for (int j = 0; j < height; j++)
	for (int i = 0; i < width; i++)
	{
		image.at<uchar>(j, i) = buffer[height-j-1][i];
	}
	cv::imshow("result", image);
	cv::waitKey();*/
	return 1;
}



