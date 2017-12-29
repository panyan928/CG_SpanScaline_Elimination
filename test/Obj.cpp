#include"Obj.h"
int Obj::load(string file_path)
{
	//δ�ҵ�ģ��ʱ�����
	//cout << file_path << endl;
	ifstream file(file_path);
	if (!file.is_open())
	{
		cout << "can't open model" << endl;
		return 0; 
	}
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
		if (str[0] == 'v'&&str[1] == ' ')
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
				if (pos>0) temp_f.push_back(atoi(s.substr(0, pos).data()));
				else temp_f.push_back(atoi(sub));
				sub = strtok(NULL, " ");
			}
			face.push_back(temp_f);
			n_face++;
		}
	}
	CalcProp();
	return 1;
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
	float scale = 1;
	//�����߽�����

	scale = maxx - minx;//�ȼ���x����ĳ߶ȱ任
	//ģ��ѡ���ȴ�ķ���
	scale = (maxy - miny) > scale ? maxy - miny : scale;
	//���ڵĴ�СΪ���ӣ�ѡ���С�ķ���
	if (width<height)
	{
		if (width>300)
			scale = (width - 200) / scale;
		else scale = width / scale;
	}
		
	else
	{
		if (height>300)
			scale = (height - 200) / scale;
		else scale = height / scale;
	}
	
	for (int i = 1; i <= n_vertex; i++)
	{
		//�ػ�ʱ���ƶ�������������
		vertex[i] = (vertex[i] - center) * scale;
		//�ػ�ʱ��ƫ������center��أ��״λ���ʱcenterΪ0
		vertex[i].x = vertex[i].x  + width / 2;
		vertex[i].y = vertex[i].y  + height / 2;
		//if (vertex[i].x <= 0 || vertex[i].x >= width) cout << "x�������" << endl;
		//if (vertex[i].y <= 0 ) cout << "y<0" << endl;
	}
	minx = (minx - center.x)*scale + width / 2;
	maxx = (maxx - center.x)*scale + width / 2;
	miny = (miny - center.y)*scale + height / 2;
	maxy = (maxy - center.y)*scale + height / 2;
	//ģ�͵�����Ϊ���ڵ��м�
	center = Coord<float>(width / 2, height / 2, 0);
}

void Obj::info()
{
	cout << "The number of vertexs: " << n_vertex << endl;
	cout << "The number of faces: " << n_face << endl;
}