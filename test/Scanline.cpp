#include"Scanline.h"
#include<algorithm>

struct endpoint
{
	float zl;
	float zr;
	int order;
};

Color getColor(Coord<float> &normal)
{
	//��ά�ռ������¼���z����ļн�
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
		//�����߱�
		Edge edge;
		float miny = 999999;
		float maxy = 0;
		//���ﲻֻ��3��������������ȫ��һ�㰡����������Ϊ�����������˶�ã���
		//for (int j = 0; j < 3; j++)
		for (int j = 0; j < obj.face[i].size();j++)
		{
			edge.polyID = i;
			//edge.flag = 0;
			Coord<float> c1 = obj.vertex[obj.face[i][j]];
			Coord<float> c2 = obj.vertex[obj.face[i][(j + 1) % obj.face[i].size()]];
			//���ݱߵ��϶˵����꣬ymax���߷�����Ӧ��λ��
			//��yֵ��������ĵ���Ϊ�ߵ���㣬��c1�ڵ�c2�����棬yֵС
			if (c1.y<c2.y)
			{
				edge.x = c2.x;
				//dx��б�ʵ������෴����y+1ʱx������
				edge.dy = ceil(c2.y) - ceil(c1.y);
				if (edge.dy == 0) continue;
				//edge.z = c2.z;
				//edge.dz = (c1.z - c2.z) / edge.dy;//z���������
				edge.dx = (c1.x - c2.x) / edge.dy;
				ETable[ceil(c2.y)].push_back(edge);
			}
			else//c2��������
			{
				edge.x = c1.x;
				edge.dy = ceil(c1.y) - ceil(c2.y);
				if (edge.dy == 0) continue;
				edge.dx = (c2.x - c1.x) / edge.dy;
				//edge.z = c1.z;
				//edge.dz = (c2.z - c1.z) / edge.dy;//z���������
				//��һ�γ�����Ϊ����y�������Ϊ�����������Ƚ�ģ�������Ƶ������м�
				ETable[ceil(c1.y)].push_back(edge);
			}
			miny = miny < c1.y ? miny : c1.y;
			maxy = maxy > c1.y ? maxy : c1.y;
		}
		//��������α�
		//���ݶ���ε����y���������Ӧλ��
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
		//dy==0��ʾ�û���Ѿ���ĩ�ˣ�ɾȥ
		//0<dy<1ʱ�û���Դ��ڣ����Գ��ֻ�߱���һ��������������ߵ��������dy��Ϊint
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
	//�жϻ����α���ɨ����Ķ����ɾȥ
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
//���ݱߵ�id�ҵ��������Ķ���Σ�������εı�־�޸�
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
//����ID�ҵ������ε�����ֵ������count�����Ҽ���
Color  Scanline::pixelByID(int count, float x, float y)
{
	Color col = 0;
	y = y - 0.5;//ɨ����y��������Ϊy+0.5
	float maxz = -999, z;
 
	for (vector<Poly>::iterator it = APT.begin(); count > 0 && it != APT.end(); it++)
	{
		//���ж��ǽ�������Ķ����
		if ((*it).flag)
		{
			//����Ƚ��������zֵ
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

void Scanline::ComputeBuffer(int y, vector<Color> &buffer)
{
	float left = 0, right;
	//����μ���
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
		Color col = 0;//��ʼ����ֵ��Ϊ����0
		//countΪ0��[left,right]�������޶����
		if (count == 0)
		{
			//����������ֵ����Ϊ����ֵ
 
			for(float pixel=left;left<right;left++)
			{
				//image.at<uchar>(height - y - 1, left) = col;
				buffer[left]=0;
			}
		}
		//��������ֻ��һ�������
		else if (count == 1)
		{
			col = pixelByID(count, left, y);
			//����������ֵ����Ϊ��ǰ����ε�color
 
			for(float pixel=left;left<right;left++)
			{
				//image.at<uchar>(height - y - 1, floor(left)) = col;
				buffer[left] = col;
			}
		}
		else//�������ж�������ʱ�����㽻��
		{
			//���㽻��
			//vector<Coord<float>> cross;
			//����������,�ɶԴ洢�߽��zֵ
			vector<endpoint> points;
			Poly p1, p2;//����ᴩ�����������
			vector<float> cross;//�����xֵ
			//�����������ҵ����ֵ�������������Ķ������APT���е�λ�ô�����
 
			for (int i = 0; i < APT.size();i++)
			{
				if (points.size() == count) break;
				if (APT[i].flag)
				{
					points.push_back( { float(-(APT[i].a*left + APT[i].b*(y + 0.5) + APT[i].d) / APT[i].c),
									    float(-(APT[i].a*right + APT[i].b*(y + 0.5) + APT[i].d) / APT[i].c),
									    i });
				}
			}
			//���������Ƿ񽻲棬����Ļ����������ǵĽ��㣬���ѽ����뽻��ջ��
 
			for (int i = 0; i < points.size(); i++)
			{
				//���Ҷ˵�Ĳ�ֵ�����෴,������ֹᴩ
				for (int j = i+1;j < points.size(); j++)
				{
					if ((points[i].zl - points[j].zl) * (points[i].zr - points[j].zr) < 0)
					{
						p1 = APT[points[i].order];//�����ཻ���������ߵ�APT����
						p2 = APT[points[j].order];//
						cross.push_back((p2.c*p1.d + p2.c*p1.b*y - p1.c*p2.d - p1.c*p2.b*y) / (p1.c*p2.a - p2.c*p1.a));
					}
				}
			}
			//������������������������ջ
			cross.push_back(right);
			std::sort(cross.begin(), cross.end());
			for (int i = 0; i < cross.size();i++)
			{
				right = cross[i];
				//������Ը�һ�£���Ȼpoints����֪�APT����ţ��Ϳ��Բ�����pixelByID����Ѱ����
				col = pixelByID(count, left, y);
 
				for(float pixel=left;left<right;left++)
				{
					//image.at<uchar>(height - y - 1, floor(left)) = col;
					buffer[left] = col;
				}
			}
		}
		//�����ҵı������Ķ���α�־ȡ��
		//ȡ���Ժ󷵻�1��ʾ����ö���Σ�count+1������0���뿪�ö���Σ�count-1
		count = UpdateFlagByID(AET[n].polyID) ? count + 1 : count - 1;
		left = right;
		//������һ�����䣬������Ϊ��һ����߱�ı�
	}
	//ѭ��������leftΪ���һ������ɨ���ߵĽ���
 
	for (float pixel = left; left < width; left++)
	{
		//image.at<uchar>(height - y - 1, left) = col;
		buffer[left] = 0;
	}
}

void Scanline::scan(Obj &obj, vector<vector<Color>> &buffer)
{
	AET.clear();
	APT.clear();
	buffer.clear();
	//���������������ɨ��
	//�ߵ�ָ����ָ��y��С�ķ�������x+dx��y-1����ֵ
	buffer.resize(height);
	for (int y = height-1; y >= 0; y--)
	{
		//cout << y << endl;
		//����µĻ��
		for (int n = 0; n < (int)ETable[y].size(); n++)
		{
			AET.push_back(ETable[y][n]);
		}
		//����µĻ�����
		for (int n = 0; n < (int)PTable[y].size(); n++)
		{
			APT.push_back(PTable[y][n]);
		}
		//�Ի�߱��������
		if(!AET.empty()) SortActiveEdge();
		//�����ڼ���
		ComputeBuffer(y,buffer[y]);

		//���»��������һ��ɨ���ߵı�
		UpdateActivePoly();
		UpdateActiveEdge();
		
	}
}