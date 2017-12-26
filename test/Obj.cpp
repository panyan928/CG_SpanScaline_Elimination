#include"Obj.h"


Obj::Obj(const char* file_path) {
	//δ�ҵ�ģ��ʱ�����
	ifstream file(file_path);
	char* str = "";
	char* sub;
	//Coord<float> min(INF,INF,INF), max(0, 0, 0);
	Coord<float> temp_c = Coord<float>(0, 0, 0);
	vector<int> temp_f;
	//vertex,��face��������1��ʼ
	vertex.push_back(temp_c);
	face.push_back(temp_f);
	normal.push_back(temp_c);
	str = (char*)malloc(200 * sizeof(char));
	while (file.getline(str, 200))
	{
		//��ȡ������Ϣ
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

		//��ȡ����Ϣ
		else if (str[0] == 'f')
		{
			temp_f.clear();
			//����һ��ʼ��str=str���� strָ����ڴ�ռ��ı䣬���¶�ȡ��һ��ʱstr�Ų���
			sub = strtok(str, " ");
			//str="f"
			sub = strtok(NULL, " ");
			
			while (sub)
			{
				string s(sub);
				//str�ǵ�һ����
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
//���㷨�ߡ���������
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
	//width,height ��ʼ�����ԣ���Ҫ�ģ�
	float scale = 1;
	scale = maxx - minx;//�ȼ���x����ĳ߶ȱ任
	if ((maxy - miny) > scale)//���y����Χ����Ļ��ı�scale,��scale����
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
		if (vertex[i].x <= 0 || vertex[i].x >= width) cout << "x�������" << endl;
		if (vertex[i].y <= 0 || vertex[i].y >= height) cout << "y�������" << endl;
	}
	minx = minx*scale+width / 2;
	maxx = maxx*scale+width / 2;
	miny = miny*scale+height / 2;
	maxy = maxy*scale+height / 2;
	//ģ�͵�����Ϊ���ڵ��м�
	center = Coord<float>(width / 2, height / 2, 0);
}