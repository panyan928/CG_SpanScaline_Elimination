#include"Obj.h"


Obj::Obj(const char* file_path) {
	//未找到模型时的输出
	ifstream file(file_path);
	char* str = "";
	char* sub;
	//Coord<float> min(INF,INF,INF), max(0, 0, 0);
	Coord<float> temp_c = Coord<float>(0, 0, 0);
	vector<int> temp_f;
	//vertex,和face的索引从1开始
	vertex.push_back(temp_c);
	face.push_back(temp_f);
	normal.push_back(temp_c);
	str = (char*)malloc(200 * sizeof(char));
	while (file.getline(str, 200))
	{
		//读取顶点信息
		if (str[0]=='v'&&str[1]==' ')
		{
			sscanf(str + 2 * sizeof(char), "%f %f %f", &temp_c.x, &temp_c.y, &temp_c.z);
			vertex.push_back(temp_c);
			n_vertex++;
			minx = minx < temp_c.x ? minx : temp_c.x;
			maxx = maxx > temp_c.x ? maxx : temp_c.x;
			miny = miny < temp_c.y ? miny : temp_c.y;
			maxy = maxy > temp_c.y ? maxy : temp_c.y;
		}

		//读取面信息
		else if (str[0] == 'f')
		{
			temp_f.clear();
			//这里一开始让str=str。。 str指向的内存空间会改变，导致读取下一行时str放不下
			sub = strtok(str, " ");
			//str="f"
			sub = strtok(NULL, " ");
			
			while (sub)
			{
				string s(sub);
				//str是第一个数
				int pos = s.find_first_of("/");
				if(pos>0) temp_f.push_back(atoi(s.substr(0, pos).data()));
				else temp_f.push_back(atoi(sub));
				sub = strtok(NULL, " ");
			}
			face.push_back(temp_f);
			n_face++;
		}
	}
	CalcProp();
}
//计算法线、中心坐标
void Obj::CalcProp()
{
	normal.resize(face.size());
	for (int i = 1; i <= n_face; i++)
	{
		normal[i] = (vertex[face[i][1]] - vertex[face[i][0]])*(vertex[face[i][2]] - vertex[face[i][0]]);
		normal[i] = normal[i] / sqrt(normal[i].x*normal[i].x + normal[i].y*normal[i].y + normal[i].z*normal[i].z);
	}
}

void Obj::scale(int width, int height)
{
	//width,height 初始化不对，需要改！
	float scale = 1;
	scale = maxx - minx;//先计算x方向的尺度变换
	if ((maxy - miny) > scale)//如果y方向范围更大的话改变scale,将scale增大
	{
		scale = maxy - miny;
		scale = (height - 400) / scale;
	}
	else scale = (width - 400) / scale;
	for (int i = 1; i <= n_vertex; i++)
	{
		vertex[i] = vertex[i] * scale;
		vertex[i].x += width / 2;
		vertex[i].y += height / 2;
		if (vertex[i].x <= 0 || vertex[i].x >= width) cout << "x方向出界" << endl;
		if (vertex[i].y <= 0 || vertex[i].y >= height) cout << "y方向出界" << endl;
	}
	minx = minx*scale+width / 2;
	maxx = maxx*scale+width / 2;
	miny = miny*scale+height / 2;
	maxy = maxy*scale+height / 2;
	//模型的中心为窗口的中间
	center = Coord<float>(width / 2, height / 2, 0);
}