#include"Scanline.h"
#include<algorithm>

Color getColor(Coord<float> &normal)
{
	//三维空间坐标下计算z方向的夹角
	if (normal.z < 0) normal.z = -normal.z;
	if (normal.x < 0)normal.x = -normal.x;
	if (normal.y < 0) normal.y = -normal.y;
	float theta = normal.z / sqrt(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
	return 255 * theta;
}

void Scanline::BuildTable(Obj &obj)
{
	ETable.clear();
	PTable.clear();
	ETable.resize(height);
	PTable.resize(height);
	for (int i = 1; i <= obj.n_face; i++)
	{
		//创建边表
		Edge edge;
		float miny = 999999;
		float maxy = 0;
		for (int j = 0; j < 3; j++)
		{
			edge.polyID = i;
			//edge.flag = 0;
			Coord<float> c1 = obj.vertex[obj.face[i][j]];
			Coord<float> c2 = obj.vertex[obj.face[i][(j + 1) % 3]];
			//根据边的上端点坐标，ymax将边放入相应的位置
			//找y值大，在上面的点作为边的起点，点c1在点c2的上面，y值小
			if (c1.y<c2.y)
			{
				edge.x = c2.x;
				//dx是斜率导数的相反数，y+1时x的增量
				edge.dy = ceil(c2.y) - ceil(c1.y);
				if (edge.dy == 0) continue;
				//edge.z = c2.z;
				//edge.dz = (c1.z - c2.z) / edge.dy;//z方向的增量
				edge.dx = (c1.x - c2.x) / edge.dy;
				ETable[ceil(c2.y)].push_back(edge);
			}
			else//c2点在上面
			{
				edge.x = c1.x;
				edge.dy = ceil(c1.y) - ceil(c2.y);
				if (edge.dy == 0) continue;
				edge.dx = (c2.x - c1.x) / edge.dy;
				//edge.z = c1.z;
				//edge.dz = (c2.z - c1.z) / edge.dy;//z方向的增量
				//第一次出错因为这里y坐标可能为负！！！，先将模型中心移到窗口中间
				ETable[ceil(c1.y)].push_back(edge);
			}
			miny = miny < c1.y ? miny : c1.y;
			maxy = maxy > c1.y ? maxy : c1.y;
		}
		//创建多边形表
		//根据多边形的最大y坐标放入相应位置
		if (maxy == 0) continue;
		Poly poly;
		poly.polyID = i;
		poly.dy = ceil(maxy) - ceil(miny);
		poly.a = obj.normal[i].x;
		poly.b = obj.normal[i].y;
		poly.c = obj.normal[i].z;
		Coord<float> temp = obj.vertex[obj.face[i][0]];
		poly.d = -(poly.a*temp.x + poly.b*temp.y + poly.c*temp.z);
		poly.flag = 0;
		poly.color = getColor(obj.normal[i]);
		PTable[ceil(maxy)].push_back(poly);
	}
}

void Scanline::UpdateActiveEdge()
{
	int n2 = 0;

	for (vector<Edge>::iterator it = AET.begin(); it != AET.end(); it++)
	{
		(*it).dy--;
		//dy==0表示该活化边已经到末端，删去
		//0<dy<1时该活化边仍存在，所以出现活化边表中一个多边形有三条边的情况，将dy改为int
		if ((*it).dy == 0) continue;
		else
		{
			(*it).x += (*it).dx;
			//(*it).z += (*it).dz;
			*(AET.begin()+n2) = *(it);
			n2++;
		}
	}
	AET.resize(n2);
}

void Scanline::UpdateActivePoly()
{
	//判断活化多边形表，已扫描完的多边形删去
	int n2 = 0;
	for (vector<Poly>::iterator it = APT.begin(); it != APT.end(); it++)
	{
		(*it).dy--;
		if ((*it).dy == 0) continue;
		else
		{
			*(APT.begin() + n2) = *(it);
			n2++;
		}
	}
	APT.resize(n2);
}

bool comp(const Edge &a, const Edge &b)
{
	return a.x < b.x;
}

void Scanline::SortActiveEdge()
{
	std::sort(AET.begin(), AET.end(), comp);
}
//根据边的id找到它所属的多边形，将多边形的标志修改
int Scanline::UpdateFlagByID(int id)
{
	for (vector<Poly>::iterator it = APT.begin(); it != APT.end(); it++)
	{
		if ((*it).polyID == id)
		{
			(*it).flag = !(*it).flag;
			return (*it).flag;
		}
	}
}
//根据ID找到活化多边形的像素值，根据count决定找几个
Color  Scanline::pixelByID(int count, float x, float y)
{
	Color col = 0;
	y = y - 0.5;//扫描线y的中心线为y+0.5
	float maxz = -999, z;
	for (vector<Poly>::iterator it = APT.begin(); count > 0 && it != APT.end(); it++)
	{
		//先判断是进入区间的多边形
		if ((*it).flag)
		{
			//计算比较区间左的z值
			//z = AET[n_edge].z;
			z = -((*it).a*x + (*it).b*y + (*it).d) / (*it).c;
			if (z > maxz)
			{
				maxz = z;
				col = (*it).color;
			}
			count--;
		}
	}
	return col;
}

struct endpoint
{
	float zl;
	float zr;
	int order;
};

void Scanline::ComputeBuffer(int y, vector<Color> &buffer)
{
	float left = 0, right;
	//多边形计数
	int count = 0;
	buffer.resize(width);
	for (int n = 0; n < (int)AET.size(); n++)
	{
		right = AET[n].x;
		if (left >= right)
		{
			count = UpdateFlagByID(AET[n].polyID) ? count + 1 : count - 1;
			continue;
		}
		Color col = 0;//初始像素值设为背景0
		//count为0，[left,right]区段内无多边形
		if (count == 0)
		{
			//区间内像素值都设为背景值
			while (left < right)
			{
				//image.at<uchar>(height - y - 1, left) = col;
				buffer[left]=0;
				left++;
			}
		}
		//该区间内只有一个多边形
		else if (count == 1)
		{
			col = pixelByID(count, left, y);
			//区间内像素值都设为当前多边形的color
			while (left < right)
			{
				//image.at<uchar>(height - y - 1, floor(left)) = col;
				buffer[left] = col;
				left++;
			}
		}
		else//区间内有多个多边形时，计算交点
		{
			//计算交点
			//vector<Coord<float>> cross;
			//先两两计算,成对存储边界的z值
			vector<endpoint> points;
			Poly p1, p2;//代表贯穿的两个多边形
			vector<float> cross;//交点的x值
			//计算区间左右的深度值，把他们所属的多边形在APT表中的位置存下来
			for (int i = 0; i < APT.size();i++)
			{
				if (points.size() == count) break;
				if (APT[i].flag)
				{
					//这里不能拿left去算，left永远只是整数，不是真正的扫描线与边的交点，会有偏差
					/*points.push_back(  { float(-(APT[i].a*left + APT[i].b*(y + 0.5) + APT[i].d) / APT[i].c), 
									   float( -(APT[i].a*right + APT[i].b*(y + 0.5) + APT[i].d) / APT[i].c),
									    i}  );*/
					points.push_back( { float(-(APT[i].a*left + APT[i].b*(y + 0.5) + APT[i].d) / APT[i].c),
									    float(-(APT[i].a*right + APT[i].b*(y + 0.5) + APT[i].d) / APT[i].c),
									    i });
				}
			}
			//两两计算是否交叉，交叉的话则计算出他们的交点，并把交点入交点栈中
			for (int i = 0; i < points.size(); i++)
			{
				//左右端点的差值符号相反,代表出现贯穿
				for (int j = i+1;j < points.size(); j++)
				{
					if ((points[i].zl - points[j].zl) * (points[i].zr - points[j].zr) < 0)
					{
						p1 = APT[points[i].order];//代表相交的这两条线的APT索引
						p2 = APT[points[j].order];//
						cross.push_back((p2.c*p1.d + p2.c*p1.b*y - p1.c*p2.d - p1.c*p2.b*y) / (p1.c*p2.a - p2.c*p1.a));
					}
				}
			}
			//把现在这个大区间的区间右入栈
			cross.push_back(right);
			std::sort(cross.begin(), cross.end());
			for (int i = 0; i < cross.size();i++)
			{
				right = cross[i];
				//这里可以改一下，既然points中已知活化APT的序号，就可以不用再pixelByID依次寻找了
				col = pixelByID(count, left, y);
				while (left < right)
				{
					//image.at<uchar>(height - y - 1, floor(left)) = col;
					buffer[left] = col;
					left++;
				}
			}
		}
		//区间右的边所属的多边形标志取补
		//取补以后返回1表示进入该多边形，count+1，返回0则离开该多边形，count-1
		count = UpdateFlagByID(AET[n].polyID) ? count + 1 : count - 1;
		left = right;
		//换到下一个区间，区间右为下一条活化边表的边
	}
	//循环结束，left为最后一条边与扫描线的交点
	while (left < width)
	{
		//image.at<uchar>(height - y - 1, left) = col;
		buffer[left] = 0;
		left++;
	}
}
void Scanline::scan(Obj &obj, vector<vector<Color>> &buffer)
{
	AET.clear();
	APT.clear();
	buffer.clear();
	//建立活化表，从上向下扫描
	//边的指向是指向y减小的方向，所以x+dx是y-1处的值
	buffer.resize(height);
	for (int y = height-1; y >= 0; y--)
	{
		//cout << y << endl;
		//添加新的活化边
		for (int n = 0; n < (int)ETable[y].size(); n++)
		{
			AET.push_back(ETable[y][n]);
		}
		//添加新的活化多边形
		for (int n = 0; n < (int)PTable[y].size(); n++)
		{
			APT.push_back(PTable[y][n]);
		}
		//对活化边表进行排序
		if(!AET.empty()) SortActiveEdge();
		//区间内计算
		ComputeBuffer(y,buffer[y]);

		//更新活化表，产生下一条扫描线的表
		UpdateActivePoly();
		UpdateActiveEdge();
		
	}
}