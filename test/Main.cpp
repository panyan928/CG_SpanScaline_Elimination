#include<stdio.h>
#include <glut.h>
#include"Obj.h"
#include"Coord.h"
#include"Scanline.h"
#include<opencv2/opencv.hpp>

//帧缓存，存储每位的像素值
vector<vector<Color>> buffer;
//窗口的长宽
int width;
int height;

//void init(int* argc,char** argv,Scanline eliminate)
//{
//	glutInit(argc,argv);
//	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
//	glutInitWindowPosition(0, 0);
//	glutInitWindowSize(width, height);
//	glutCreateWindow("CGHW1");
//
//}
//
//void display(void)
//{
//	glClear(GL_COLOR_BUFFER_BIT);
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	gluOrtho2D(0, width, 0, height);
//	glBegin(GL_POINTS);
//	for (int y = 0; y < height; y++)
//		for (int x = 0; x < width; x++)
//		{
//			glColor3f(buffer[y][x], buffer[y][x], buffer[y][x]);
//			glVertex2i(x, y);
//		}
//	glEnd();
//	glFinish();
//}

int main(int argc,char** argv)
{
	buffer.clear();
	Obj obj("bunny.obj");
	width = 1000;
	height = 800;
	Scanline eliminate(width,height);
	cv::Mat image = cv::Mat::zeros(height, width, CV_8UC1);
	//init(&argc, argv,eliminate);

	obj.scale(width, height);
	eliminate.BuildTable(obj);
	eliminate.scan(obj, buffer,image);

	//glutDisplayFunc(display);
	//glutMainLoop();
	
	/*for (int j = 0; j < height; j++)
	for (int i = 0; i < width; i++)
	{
		image.at<uchar>(j, i) = buffer[height-j-1][i];
	}*/
	cv::imshow("result", image);
	cv::waitKey(0);
	return 1;
}



