#include"Scanline.h"
#include<algorithm>

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
	ETable.resize(height);
	PTable.resize(height);
	for (int i = 1; i <= obj.n_face; i++)
	{
		//�����߱�
		Edge edge;
		float miny = 999999;
		float maxy = 0;
		for (int j = 0; j < 3; j++)
		{
			edge.polyID = i;
			//edge.flag = 0;
			Coord<float> c1 = obj.vertex[obj.face[i][j]];
			Coord<float> c2 = obj.vertex[obj.face[i][(j + 1) % 3]];
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
			//���︳ֵ�Ǵ�ģ����Ի�߱�û��ɾȥ�Ѿ�ɨ����ı�
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
	sort(AET.begin(), AET.end(), comp);

//û���ǻ�߱�Ϊ�յ��������ʱitΪ�գ����ܸ�ֵ
//��߲���Ҫflag
/*vector<Edge>::iterator it = AET.begin();
(*it).flag = 0;
for (it++; it != AET.end(); it++)
{
	(*it).flag = !((it - 1)->flag);
	//AET[n].flag = !AET[n - 1].flag;
}*/
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
	y = y + 0.5;//ɨ����y��������Ϊy+0.5
	float maxz = -999, z;
	for (vector<Poly>::iterator it = APT.begin(); count > 0 && it != APT.end(); it++)
	{
		//���ж��ǽ�������Ķ����
		if ((*it).flag)
		{
			//����Ƚ��������zֵ
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



void Scanline::ComputeBuffer(int y, vector<Color> &buffer, Mat &image)
{
	float left = 0, right;
	//����μ���
	int count = 0;
	for (int n = 0; n < (int)AET.size(); n++)
	{
		right = AET[n].x;
		Color col = 0;//��ʼ����ֵ��Ϊ����0
		//countΪ0��[left,right]�������޶����
		if (count == 0)
		{
			//����������ֵ����Ϊ����ֵ
			/*while (left < right)
			{
				image.at<uchar>(height - y - 1, left) = col;
				buffer.push_back(col);
				left++;
			}*/
		}
		//��������ֻ��һ�������
		else if (count == 1)
		{
			col = pixelByID(count, left, y);
			//����������ֵ����Ϊ��ǰ����ε�color
			while (left < right)
			{
				image.at<uchar>(height - y - 1, left) = col;
				//buffer.push_back(col);
				left++;
			}
		}
		else//�������ж�������ʱ�����㽻��
		{
			//���㽻��
			//vector<Coord<float>> cross;
			//����������
			vector<float> zl;
			vector<float> zr;
			for (vector<Poly>::iterator it = APT.begin(); it != APT.end(); it++)
			{
				if (zl.size() == count) break;
				if ((*it).flag)
				{
					zl.push_back(-((*it).a*left + (*it).b*(y+0.5) + (*it).d) / (*it).c);
					zr.push_back(-((*it).a*right + (*it).b*(y+0.5) + (*it).d) / (*it).c);
				}
			}
			//for (int i = 0; i < count; i++)
			//{
			//	//���Ҷ˵�Ĳ�ֵ�����෴,������ֹᴩ
			//	if ((zl[i] - zl[(i + 1) % count])*(zr[i] - zr[(i + 1) % count]) < 0)
			//	{
			//		//���㽻��
			//		float cross = 
			//	}
			//}
			
			col = pixelByID(count,left,y);
			while (left < right)
			{
				image.at<uchar>(height - y - 1, left) = col;
				//buffer.push_back(col);
				left++;
			}
		}
		//�����ҵı������Ķ���α�־ȡ��
		//ȡ���Ժ󷵻�1��ʾ����ö���Σ�count+1������0���뿪�ö���Σ�count-1
		count = UpdateFlagByID(AET[n].polyID) ? count + 1 : count - 1;
		left = ceil(right);
		//������һ�����䣬������Ϊ��һ����߱�ı�
	}
	//ѭ��������leftΪ���һ������ɨ���ߵĽ���
	/*while (left < width)
	{
		image.at<uchar>(height - y - 1, left) = col;
		buffer.push_back(0);
		left++;
	}*/
}
void Scanline::scan(Obj &obj, vector<vector<Color>> &buffer,Mat &image)
{
	AET.clear();
	APT.clear();
	//���������������ɨ��
	//�ߵ�ָ����ָ��y��С�ķ�������x+dx��y-1����ֵ
	buffer.resize(height);
	for (int y = height-1; y >= 0; y--)
	{
		//����µĻ�����
		cout << y;
		for (int n1 = 0; n1 < (int)PTable[y].size(); n1++)
		{
			APT.push_back(PTable[y][n1]);
		}
		//����µĻ��
		for (int n = 0; n < (int)ETable[y].size(); n++)
		{

			AET.push_back(ETable[y][n]);
		}
		//�Ի�߱��������
		if(!AET.empty()) SortActiveEdge();
		//�����ڼ���
		cout << "poly:" << APT.size() << " edge:" << AET.size() ;
		if (APT.size() * 2 == AET.size())
			cout << " yes" << endl;
		ComputeBuffer(y,buffer[y],image);

		//���»��������һ��ɨ���ߵı�
		UpdateActivePoly();
		UpdateActiveEdge();
		
		// ���߱���ɨ���ߵĽ���
		// ������˳�������������
		//ÿ���ڲ�����ѭ�� �ж����ֵ
	}
}