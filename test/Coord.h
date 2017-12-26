#pragma once
#define Color unsigned char
template <typename T>
struct Coord
{
	T x, y, z;
	//构造函数初始化类成员
	Coord() {}
	Coord(T ax, T ay, T az) : x(ax), y(ay), z(az) {}
	

	//运算符重载
	Coord<T> operator + (Coord<T> &other)
	{
		return Coord<T>(x + other.x, y + other.y, z + other.z);
	}
	Coord<T> operator - (const Coord<T>& other) {
		return Coord<T>(x - other.x, y - other.y, z - other.z);
	}
	Coord<T> operator /(const float &num)
	{
		return Coord<T>(x / num, y / num, z / num);
	}
	Coord<T> operator *(const Coord<T>& other)
	{
		return Coord<T>(y*other.z - z*other.y, z*other.x - x*other.z, x*other.y - y*other.x);
	}
	Coord<T> operator += (Coord<T> &other)
	{
		return Coord<T>(x + other.x, y + other.y, z + other.z);
	}
	Coord<T> operator *(const float &num)
	{
		return Coord<T>(x*num, y*num, z*num);
	}
};