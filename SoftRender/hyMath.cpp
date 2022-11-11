#ifndef HY_MATH_CPP
#define HY_MATH_CPP
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <vector>
#include <map>
#include <initializer_list>
#include <stdexcept>
#include <functional>
#include <ostream>
#include <sstream>
#include <iostream>



using namespace std;
// 通用矢量：N 是矢量维度，T 为数据类型
template <size_t N, typename T>
struct Vector
{
	T m[N]; // 元素数组
	inline Vector()
	{
		for (size_t i = 0; i < N; i++)
			m[i] = T();
	}
	inline Vector(const T *ptr)
	{
		for (size_t i = 0; i < N; i++)
			m[i] = ptr[i];
	}
	inline Vector(const Vector<N, T> &u)
	{
		for (size_t i = 0; i < N; i++)
			m[i] = u.m[i];
	}
	inline Vector(const initializer_list<T> &u)
	{
		auto it = u.begin();
		for (size_t i = 0; i < N; i++)
			m[i] = *it++;
	}
	inline const T &operator[](size_t i) const
	{
		assert(i < N);
		return m[i];
	}
	inline T &operator[](size_t i)
	{
		assert(i < N);
		return m[i];
	}
	inline void load(const T *ptr)
	{
		for (size_t i = 0; i < N; i++)
			m[i] = ptr[i];
	}
	inline void save(T *ptr)
	{
		for (size_t i = 0; i < N; i++)
			ptr[i] = m[i];
	}
};

// 特化二维矢量
template <typename T>
struct Vector<2, T>
{
	union {
		struct
		{
			T x, y;
		}; // 元素别名
		struct
		{
			T u, v;
		};		// 元素别名
		T m[2]; // 元素数组
	};
	inline Vector() : x(T()), y(T()) {}
	inline Vector(T X, T Y) : x(X), y(Y) {}
	inline Vector(const Vector<2, T> &u) : x(u.x), y(u.y) {}
	inline Vector(const T *ptr) : x(ptr[0]), y(ptr[1]) {}
	inline const T &operator[](size_t i) const
	{
		assert(i < 2);
		return m[i];
	}
	inline T &operator[](size_t i)
	{
		assert(i < 2);
		return m[i];
	}
	inline void load(const T *ptr)
	{
		for (size_t i = 0; i < 2; i++)
			m[i] = ptr[i];
	}
	inline void save(T *ptr)
	{
		for (size_t i = 0; i < 2; i++)
			ptr[i] = m[i];
	}
	inline Vector<2, T> xy() const { return *this; }
	inline Vector<3, T> xy1() const { return Vector<3, T>(x, y, 1); }
	inline Vector<4, T> xy11() const { return Vector<4, T>(x, y, 1, 1); }
};

// 特化三维矢量
template <typename T>
struct Vector<3, T>
{
	union {
		struct
		{
			T x, y, z;
		}; // 元素别名
		struct
		{
			T r, g, b;
		};		// 元素别名
		T m[3]; // 元素数组
	};
	inline Vector() : x(T()), y(T()), z(T()) {}
	inline Vector(T X, T Y, T Z) : x(X), y(Y), z(Z) {}
	inline Vector(const Vector<3, T> &u) : x(u.x), y(u.y), z(u.z) {}
	inline Vector(const T *ptr) : x(ptr[0]), y(ptr[1]), z(ptr[2]) {}
	inline const T &operator[](size_t i) const
	{
		assert(i < 3);
		return m[i];
	}
	inline T &operator[](size_t i)
	{
		assert(i < 3);
		return m[i];
	}
	inline void load(const T *ptr)
	{
		for (size_t i = 0; i < 3; i++)
			m[i] = ptr[i];
	}
	inline void save(T *ptr)
	{
		for (size_t i = 0; i < 3; i++)
			ptr[i] = m[i];
	}
	inline Vector<2, T> xy() const { return Vector<2, T>(x, y); }
	inline Vector<3, T> xyz() const { return *this; }
	inline Vector<4, T> xyz1() const { return Vector<4, T>(x, y, z, 1); }
};

// 特化四维矢量
template <typename T>
struct Vector<4, T>
{
	union {
		struct
		{
			T x, y, z, w;
		}; // 元素别名
		struct
		{
			T r, g, b, a;
		};		// 元素别名
		T m[4]; // 元素数组
	};
	inline Vector() : x(T()), y(T()), z(T()), w(T()) {}
	inline Vector(T X, T Y, T Z, T W) : x(X), y(Y), z(Z), w(W) {}
	inline Vector(const Vector<4, T> &u) : x(u.x), y(u.y), z(u.z), w(u.w) {}
	inline Vector(const T *ptr) : x(ptr[0]), y(ptr[1]), z(ptr[2]), w(ptr[3]) {}
	inline const T &operator[](size_t i) const
	{
		assert(i < 4);
		return m[i];
	}
	inline T &operator[](size_t i)
	{
		assert(i < 4);
		return m[i];
	}
	inline void load(const T *ptr)
	{
		for (size_t i = 0; i < 4; i++)
			m[i] = ptr[i];
	}
	inline void save(T *ptr)
	{
		for (size_t i = 0; i < 4; i++)
			ptr[i] = m[i];
	}
	inline Vector<2, T> xy() const { return Vector<2, T>(x, y); }
	inline Vector<3, T> xyz() const { return Vector<3, T>(x, y, z); }
	inline Vector<4, T> xyzw() const { return *this; }
};

//---------------------------------------------------------------------
// 数学库：矢量运算
//---------------------------------------------------------------------

// = (+a)
template <size_t N, typename T>
inline Vector<N, T> operator+(const Vector<N, T> &a)
{
	return a;
}

// = (-a)
template <size_t N, typename T>
inline Vector<N, T> operator-(const Vector<N, T> &a)
{
	Vector<N, T> b;
	for (size_t i = 0; i < N; i++)
		b[i] = -a[i];
	return b;
}

// = (a == b) ? true : false
template <size_t N, typename T>
inline bool operator==(const Vector<N, T> &a, const Vector<N, T> &b)
{
	for (size_t i = 0; i < N; i++)
		if (a[i] != b[i])
			return false;
	return true;
}

// = (a != b)? true : false
template <size_t N, typename T>
inline bool operator!=(const Vector<N, T> &a, const Vector<N, T> &b)
{
	return !(a == b);
}

// = a + b
template <size_t N, typename T>
inline Vector<N, T> operator+(const Vector<N, T> &a, const Vector<N, T> &b)
{
	Vector<N, T> c;
	for (size_t i = 0; i < N; i++)
		c[i] = a[i] + b[i];
	return c;
}

// = a - b
template <size_t N, typename T>
inline Vector<N, T> operator-(const Vector<N, T> &a, const Vector<N, T> &b)
{
	Vector<N, T> c;
	for (size_t i = 0; i < N; i++)
		c[i] = a[i] - b[i];
	return c;
}

// = a * b，不是点乘也不是叉乘，而是各个元素分别相乘，色彩计算时有用
template <size_t N, typename T>
inline Vector<N, T> operator*(const Vector<N, T> &a, const Vector<N, T> &b)
{
	Vector<N, T> c;
	for (size_t i = 0; i < N; i++)
		c[i] = a[i] * b[i];
	return c;
}

// = a / b，各个元素相除
template <size_t N, typename T>
inline Vector<N, T> operator/(const Vector<N, T> &a, const Vector<N, T> &b)
{
	Vector<N, T> c;
	for (size_t i = 0; i < N; i++)
		c[i] = a[i] / b[i];
	return c;
}

// = a * x
template <size_t N, typename T>
inline Vector<N, T> operator*(const Vector<N, T> &a, T x)
{
	Vector<N, T> b;
	for (size_t i = 0; i < N; i++)
		b[i] = a[i] * x;
	return b;
}

// = x * a
template <size_t N, typename T>
inline Vector<N, T> operator*(T x, const Vector<N, T> &a)
{
	Vector<N, T> b;
	for (size_t i = 0; i < N; i++)
		b[i] = a[i] * x;
	return b;
}

// = a / x
template <size_t N, typename T>
inline Vector<N, T> operator/(const Vector<N, T> &a, T x)
{
	Vector<N, T> b;
	for (size_t i = 0; i < N; i++)
		b[i] = a[i] / x;
	return b;
}

// = x / a
template <size_t N, typename T>
inline Vector<N, T> operator/(T x, const Vector<N, T> &a)
{
	Vector<N, T> b;
	for (size_t i = 0; i < N; i++)
		b[i] = x / a[i];
	return b;
}

// a += b
template <size_t N, typename T>
inline Vector<N, T> &operator+=(Vector<N, T> &a, const Vector<N, T> &b)
{
	for (size_t i = 0; i < N; i++)
		a[i] += b[i];
	return a;
}

// a -= b
template <size_t N, typename T>
inline Vector<N, T> &operator-=(Vector<N, T> &a, const Vector<N, T> &b)
{
	for (size_t i = 0; i < N; i++)
		a[i] -= b[i];
	return a;
}

// a *= b
template <size_t N, typename T>
inline Vector<N, T> &operator*=(Vector<N, T> &a, const Vector<N, T> &b)
{
	for (size_t i = 0; i < N; i++)
		a[i] *= b[i];
	return a;
}

// a /= b
template <size_t N, typename T>
inline Vector<N, T> &operator/=(Vector<N, T> &a, const Vector<N, T> &b)
{
	for (size_t i = 0; i < N; i++)
		a[i] /= b[i];
	return a;
}

// a *= x
template <size_t N, typename T>
inline Vector<N, T> &operator*=(Vector<N, T> &a, T x)
{
	for (size_t i = 0; i < N; i++)
		a[i] *= x;
	return a;
}

// a /= x
template <size_t N, typename T>
inline Vector<N, T> &operator/=(Vector<N, T> &a, T x)
{
	for (size_t i = 0; i < N; i++)
		a[i] /= x;
	return a;
}

//---------------------------------------------------------------------
// 数学库：矢量函数
//---------------------------------------------------------------------

// 不同维度的矢量转换
template <size_t N1, size_t N2, typename T>
inline Vector<N1, T> vector_convert(const Vector<N2, T> &a, T fill = 1)
{
	Vector<N1, T> b;
	for (size_t i = 0; i < N1; i++)
		b[i] = (i < N2) ? a[i] : fill;
	return b;
}

// = |a| ^ 2
template <size_t N, typename T>
inline T vector_length_square(const Vector<N, T> &a)
{
	T sum = 0;
	for (size_t i = 0; i < N; i++)
		sum += a[i] * a[i];
	return sum;
}

// = |a|
template <size_t N, typename T>
inline T vector_length(const Vector<N, T> &a)
{
	return sqrt(vector_length_square(a));
}

// = |a| , 特化 float 类型，使用 sqrtf
template <size_t N>
inline float vector_length(const Vector<N, float> &a)
{
	return sqrtf(vector_length_square(a));
}

// = a / |a|
template <size_t N, typename T>
inline Vector<N, T> vector_normalize(const Vector<N, T> &a)
{
	return a / vector_length(a);
}

// 矢量点乘
template <size_t N, typename T>
inline T vector_dot(const Vector<N, T> &a, const Vector<N, T> &b)
{
	T sum = 0;
	for (size_t i = 0; i < N; i++)
		sum += a[i] * b[i];
	return sum;
}

// 二维矢量叉乘，得到标量
template <typename T>
inline T vector_cross(const Vector<2, T> &a, const Vector<2, T> &b)
{
	return a.x * b.y - a.y * b.x;
}

// 三维矢量叉乘，得到新矢量
template <typename T>
inline Vector<3, T> vector_cross(const Vector<3, T> &a, const Vector<3, T> &b)
{
	return Vector<3, T>(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

// 四维矢量叉乘：前三维叉乘，后一位保留
template <typename T>
inline Vector<4, T> vector_cross(const Vector<4, T> &a, const Vector<4, T> &b)
{
	return Vector<4, T>(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x, a.w);
}

// = a + (b - a) * t
template <size_t N, typename T>
inline Vector<N, T> vector_lerp(const Vector<N, T> &a, const Vector<N, T> &b, float t)
{
	return a + (b - a) * t;
}

// 各个元素取最大值
template <size_t N, typename T>
inline Vector<N, T> vector_max(const Vector<N, T> &a, const Vector<N, T> &b)
{
	Vector<N, T> c;
	for (size_t i = 0; i < N; i++)
		c[i] = (a[i] > b[i]) ? a[i] : b[i];
	return c;
}

// 各个元素取最小值
template <size_t N, typename T>
inline Vector<N, T> vector_min(const Vector<N, T> &a, const Vector<N, T> &b)
{
	Vector<N, T> c;
	for (size_t i = 0; i < N; i++)
		c[i] = (a[i] < b[i]) ? a[i] : b[i];
	return c;
}

// 将矢量的值控制在 minx/maxx 范围内
template <size_t N, typename T>
inline Vector<N, T> vector_between(const Vector<N, T> &minx, const Vector<N, T> &maxx, const Vector<N, T> &x)
{
	return vector_min(vector_max(minx, x), maxx);
}

// 判断矢量是否接近
template <size_t N, typename T>
inline bool vector_near(const Vector<N, T> &a, const Vector<N, T> &b, T dist)
{
	return (vector_length_square(a - b) <= dist);
}

// 判断两个单精度矢量是否近似
template <size_t N>
inline bool vector_near_equal(const Vector<N, float> &a, const Vector<N, float> &b, float e = 0.0001)
{
	return vector_near(a, b, e);
}

// 判断两个双精度矢量是否近似
template <size_t N>
inline bool vector_near_equal(const Vector<N, double> &a, const Vector<N, double> &b, double e = 0.0000001)
{
	return vector_near(a, b, e);
}

// 矢量值元素范围裁剪
template <size_t N, typename T>
inline Vector<N, T> vector_clamp(const Vector<N, T> &a, T minx = 0, T maxx = 1)
{
	Vector<N, T> b;
	for (size_t i = 0; i < N; i++)
	{
		T x = (a[i] < minx) ? minx : a[i];
		b[i] = (x > maxx) ? maxx : x;
	}
	return b;
}

// 输出到文本流
template <size_t N, typename T>
inline std::ostream &operator<<(std::ostream &os, const Vector<N, T> &a)
{
	os << "[";
	for (size_t i = 0; i < N; i++)
	{
		os << a[i];
		if (i < N - 1)
			os << ", ";
	}
	os << "]";
	return os;
}

// 输出成字符串
template <size_t N, typename T>
inline std::string vector_repr(const Vector<N, T> &a)
{
	std::stringstream ss;
	ss << a;
	return ss.str();
}

//---------------------------------------------------------------------
// 数学库：矩阵
//---------------------------------------------------------------------
template <size_t ROW, size_t COL, typename T>
struct Matrix
{
	T m[ROW][COL];

	inline Matrix() {}

	inline Matrix(const Matrix<ROW, COL, T> &src)
	{
		for (size_t r = 0; r < ROW; r++)
		{
			for (size_t c = 0; c < COL; c++)
				m[r][c] = src.m[r][c];
		}
	}

	inline Matrix(const initializer_list<Vector<COL, T>> &u)
	{
		auto it = u.begin();
		for (size_t i = 0; i < ROW; i++)
			SetRow(i, *it++);
	}

	inline const T *operator[](size_t row) const
	{
		assert(row < ROW);
		return m[row];
	}
	inline T *operator[](size_t row)
	{
		assert(row < ROW);
		return m[row];
	}

	// 取一行
	inline Vector<COL, T> Row(size_t row) const
	{
		assert(row < ROW);
		Vector<COL, T> a;
		for (size_t i = 0; i < COL; i++)
			a[i] = m[row][i];
		return a;
	}

	// 取一列
	inline Vector<ROW, T> Col(size_t col) const
	{
		assert(col < COL);
		Vector<ROW, T> a;
		for (size_t i = 0; i < ROW; i++)
			a[i] = m[i][col];
		return a;
	}

	// 设置一行
	inline void SetRow(size_t row, const Vector<COL, T> &a)
	{
		assert(row < ROW);
		for (size_t i = 0; i < COL; i++)
			m[row][i] = a[i];
	}

	// 设置一列
	inline void SetCol(size_t col, const Vector<ROW, T> &a)
	{
		assert(col < COL);
		for (size_t i = 0; i < ROW; i++)
			m[i][col] = a[i];
	}

	// 取得删除某行和某列的子矩阵：子式
	inline Matrix<ROW - 1, COL - 1, T> GetMinor(size_t row, size_t col) const
	{
		Matrix<ROW - 1, COL - 1, T> ret;
		for (size_t r = 0; r < ROW - 1; r++)
		{
			for (size_t c = 0; c < COL - 1; c++)
			{
				ret.m[r][c] = m[r < row ? r : r + 1][c < col ? c : c + 1];
			}
		}
		return ret;
	}

	// 取得转置矩阵
	inline Matrix<COL, ROW, T> Transpose() const
	{
		Matrix<COL, ROW, T> ret;
		for (size_t r = 0; r < ROW; r++)
		{
			for (size_t c = 0; c < COL; c++)
				ret.m[c][r] = m[r][c];
		}
		return ret;
	}

	// 取得 0 矩阵
	inline static Matrix<ROW, COL, T> GetZero()
	{
		Matrix<ROW, COL, T> ret;
		for (size_t r = 0; r < ROW; r++)
		{
			for (size_t c = 0; c < COL; c++)
				ret.m[r][c] = 0;
		}
		return ret;
	}

	// 取得单位矩阵
	inline static Matrix<ROW, COL, T> GetIdentity()
	{
		Matrix<ROW, COL, T> ret;
		for (size_t r = 0; r < ROW; r++)
		{
			for (size_t c = 0; c < COL; c++)
				ret.m[r][c] = (r == c) ? 1 : 0;
		}
		return ret;
	}
};

//---------------------------------------------------------------------
// 数学库：矩阵运算
//---------------------------------------------------------------------
template <size_t ROW, size_t COL, typename T>
inline bool operator==(const Matrix<ROW, COL, T> &a, const Matrix<ROW, COL, T> &b)
{
	for (size_t r = 0; r < ROW; r++)
	{
		for (size_t c = 0; c < COL; c++)
		{
			if (a.m[r][c] != b.m[r][c])
				return false;
		}
	}
	return true;
}

template <size_t ROW, size_t COL, typename T>
inline bool operator!=(const Matrix<ROW, COL, T> &a, const Matrix<ROW, COL, T> &b)
{
	return !(a == b);
}

template <size_t ROW, size_t COL, typename T>
inline Matrix<ROW, COL, T> operator+(const Matrix<ROW, COL, T> &src)
{
	return src;
}

template <size_t ROW, size_t COL, typename T>
inline Matrix<ROW, COL, T> operator-(const Matrix<ROW, COL, T> &src)
{
	Matrix<ROW, COL, T> out;
	for (size_t j = 0; j < ROW; j++)
	{
		for (size_t i = 0; i < COL; i++)
			out.m[j][i] = -src.m[j][i];
	}
	return out;
}

template <size_t ROW, size_t COL, typename T>
inline Matrix<ROW, COL, T> operator+(const Matrix<ROW, COL, T> &a, const Matrix<ROW, COL, T> &b)
{
	Matrix<ROW, COL, T> out;
	for (size_t j = 0; j < ROW; j++)
	{
		for (size_t i = 0; i < COL; i++)
			out.m[j][i] = a.m[j][i] + b.m[j][i];
	}
	return out;
}

template <size_t ROW, size_t COL, typename T>
inline Matrix<ROW, COL, T> operator-(const Matrix<ROW, COL, T> &a, const Matrix<ROW, COL, T> &b)
{
	Matrix<ROW, COL, T> out;
	for (size_t j = 0; j < ROW; j++)
	{
		for (size_t i = 0; i < COL; i++)
			out.m[j][i] = a.m[j][i] - b.m[j][i];
	}
	return out;
}

template <size_t ROW, size_t COL, size_t NEWCOL, typename T>
inline Matrix<ROW, NEWCOL, T> operator*(const Matrix<ROW, COL, T> &a, const Matrix<COL, NEWCOL, T> &b)
{
	Matrix<ROW, NEWCOL, T> out;
	for (size_t j = 0; j < ROW; j++)
	{
		for (size_t i = 0; i < NEWCOL; i++)
		{
			out.m[j][i] = vector_dot(a.Row(j), b.Col(i));
		}
	}
	return out;
}

template <size_t ROW, size_t COL, typename T>
inline Matrix<ROW, COL, T> operator*(const Matrix<ROW, COL, T> &a, T x)
{
	Matrix<ROW, COL, T> out;
	for (size_t j = 0; j < ROW; j++)
	{
		for (size_t i = 0; i < COL; i++)
		{
			out.m[j][i] = a.m[j][i] * x;
		}
	}
	return out;
}

template <size_t ROW, size_t COL, typename T>
inline Matrix<ROW, COL, T> operator/(const Matrix<ROW, COL, T> &a, T x)
{
	Matrix<ROW, COL, T> out;
	for (size_t j = 0; j < ROW; j++)
	{
		for (size_t i = 0; i < COL; i++)
		{
			out.m[j][i] = a.m[j][i] / x;
		}
	}
	return out;
}

template <size_t ROW, size_t COL, typename T>
inline Matrix<ROW, COL, T> operator*(T x, const Matrix<ROW, COL, T> &a)
{
	return (a * x);
}

template <size_t ROW, size_t COL, typename T>
inline Matrix<ROW, COL, T> operator/(T x, const Matrix<ROW, COL, T> &a)
{
	Matrix<ROW, COL, T> out;
	for (size_t j = 0; j < ROW; j++)
	{
		for (size_t i = 0; i < COL; i++)
		{
			out.m[j][i] = x / a.m[j][i];
		}
	}
	return out;
}

template <size_t ROW, size_t COL, typename T>
inline Vector<COL, T> operator*(const Vector<ROW, T> &a, const Matrix<ROW, COL, T> &m)
{
	Vector<COL, T> b;
	for (size_t i = 0; i < COL; i++)
		b[i] = vector_dot(a, m.Col(i));
	return b;
}

template <size_t ROW, size_t COL, typename T>
inline Vector<ROW, T> operator*(const Matrix<ROW, COL, T> &m, const Vector<COL, T> &a)
{
	Vector<ROW, T> b;
	for (size_t i = 0; i < ROW; i++)
		b[i] = vector_dot(a, m.Row(i));
	return b;
}

// 类型别名
typedef Vector<2, float> Vec2f;
typedef Vector<2, double> Vec2d;
typedef Vector<2, int> Vec2i;
typedef Vector<3, float> Vec3f;
typedef Vector<3, double> Vec3d;
typedef Vector<3, int> Vec3i;
typedef Vector<4, float> Vec4f;
typedef Vector<4, double> Vec4d;
typedef Vector<4, int> Vec4i;

typedef Matrix<4, 4, float> Mat4x4f;
typedef Matrix<3, 3, float> Mat3x3f;
typedef Matrix<4, 3, float> Mat4x3f;
typedef Matrix<3, 4, float> Mat3x4f;

inline static Vec4f Lerp(const Vec4f &v1, const Vec4f &v2, float factor)
{
	return (1.0f - factor) * v1 + factor * v2;
}
inline static Vec3f Lerp(const Vec3f &v1, const Vec3f &v2, float factor)
{
	return (1.0f - factor) * v1 + factor * v2;
}
inline static Vec2f Lerp(const Vec2f &v1, const Vec2f &v2, float factor)
{
	return (1.0f - factor) * v1 + factor * v2;
}

inline static Mat4x4f matrix_set_zero()
{
	Mat4x4f m;
	m.m[0][0] = m.m[0][1] = m.m[0][2] = m.m[0][3] = 0.0f;
	m.m[1][0] = m.m[1][1] = m.m[1][2] = m.m[1][3] = 0.0f;
	m.m[2][0] = m.m[2][1] = m.m[2][2] = m.m[2][3] = 0.0f;
	m.m[3][0] = m.m[3][1] = m.m[3][2] = m.m[3][3] = 0.0f;
	return m;
}

// set to identity
inline static Mat4x4f matrix_set_identity()
{
	Mat4x4f m;
	m.m[0][0] = m.m[1][1] = m.m[2][2] = m.m[3][3] = 1.0f;
	m.m[0][1] = m.m[0][2] = m.m[0][3] = 0.0f;
	m.m[1][0] = m.m[1][2] = m.m[1][3] = 0.0f;
	m.m[2][0] = m.m[2][1] = m.m[2][3] = 0.0f;
	m.m[3][0] = m.m[3][1] = m.m[3][2] = 0.0f;
	return m;
}

// 平移变换
inline static Mat4x4f matrix_set_translate(float x, float y, float z)
{
	Mat4x4f m = matrix_set_identity();
	m.m[3][0] = x;
	m.m[3][1] = y;
	m.m[3][2] = z;
	return m;
}

// 缩放变换
inline static Mat4x4f matrix_set_scale(float x, float y, float z)
{
	Mat4x4f m = matrix_set_identity();
	m.m[0][0] = x;
	m.m[1][1] = y;
	m.m[2][2] = z;
	return m;
}

// 旋转编号，围绕 (x, y, z) 矢量旋转 theta 角度
inline static Mat4x4f matrix_set_rotate(float x, float y, float z, float theta)
{
	float qsin = (float)sin(theta * 0.5f);
	float qcos = (float)cos(theta * 0.5f);
	float w = qcos;
	Vec3f vec = vector_normalize(Vec3f(x, y, z));
	x = vec.x * qsin;
	y = vec.y * qsin;
	z = vec.z * qsin;
	Mat4x4f m;
	m.m[0][0] = 1 - 2 * y * y - 2 * z * z;
	m.m[1][0] = 2 * x * y - 2 * w * z;
	m.m[2][0] = 2 * x * z + 2 * w * y;
	m.m[0][1] = 2 * x * y + 2 * w * z;
	m.m[1][1] = 1 - 2 * x * x - 2 * z * z;
	m.m[2][1] = 2 * y * z - 2 * w * x;
	m.m[0][2] = 2 * x * z - 2 * w * y;
	m.m[1][2] = 2 * y * z + 2 * w * x;
	m.m[2][2] = 1 - 2 * x * x - 2 * y * y;
	m.m[0][3] = m.m[1][3] = m.m[2][3] = 0.0f;
	m.m[3][0] = m.m[3][1] = m.m[3][2] = 0.0f;
	m.m[3][3] = 1.0f;
	return m;
}

// 摄影机变换矩阵：eye/视点位置，at/看向哪里，up/指向上方的矢量
/*
	相机变换很简单，首先由位置，看的方向，以及上面方向来确定一个标准的相机属性
	***看向方向的向量  是由看向方向与相机位置求得   DX +z(左手)  opengl -z(右手)
	默认相机始终看向-z方向 dx看向+z
*/
inline static Mat4x4f matrix_set_lookat(const Vec3f &eye, const Vec3f &at, const Vec3f &up)
{
	Vec3f zaxis = vector_normalize(at - eye);
	Vec3f xaxis = vector_normalize(vector_cross(up, zaxis));
	Vec3f yaxis = vector_cross(zaxis, xaxis);
	Mat4x4f m;
	m.SetCol(0, Vec4f(xaxis.x, xaxis.y, xaxis.z, -vector_dot(eye, xaxis)));
	m.SetCol(1, Vec4f(yaxis.x, yaxis.y, yaxis.z, -vector_dot(eye, yaxis)));
	m.SetCol(2, Vec4f(zaxis.x, zaxis.y, zaxis.z, -vector_dot(eye, zaxis)));
	m.SetCol(3, Vec4f(0.0f, 0.0f, 0.0f, 1.0f));
	return m;
}

// D3DXMatrixPerspectiveFovLH
inline static Mat4x4f matrix_set_perspective(float fovy, float aspect, float zn, float zf)
{
	float fax = 1.0f / (float)tan(fovy * 0.5f);
	Mat4x4f m = matrix_set_zero();
	m.m[0][0] = (float)(fax / aspect);
	m.m[1][1] = (float)(fax);
	m.m[2][2] = zf / (zf - zn);
	m.m[3][2] = -zn * zf / (zf - zn);
	m.m[2][3] = 1;
	return m;
}

//opengl透视投影
inline static Mat4x4f matrix_set_perspective_opengl(float fovy, float aspect, float zn, float zf)
{
	// Mat4x4f orth = matrix_set_identity();
	// Mat4x4f pertoorth = matrix_set_identity();
	// float halfEyeAngleRadian = fovy * 0.5f  / 180.f * 3.1415926f;
	// float n = -1 * zn;
	// float f = -1 * zf;
	// float t = zn * tan(halfEyeAngleRadian);
	// float r = t * aspect;
	// float l = -1.0f * r;
	// float b = -1.0f * t;
	// orth[0][0] = 2 / (r - l);
	// orth[1][1] = 2 / (t - b);
	// orth[2][2] = 2 / (zn - zf);
	// orth[3][3] = 1;

	// pertoorth[0][0] = n;
	// pertoorth[1][1] = n;
	// pertoorth[2][2] = n + f;
	// pertoorth[2][3] = -1.0f * n * f;
	// pertoorth[3][3] = 0;
	// pertoorth[3][2] = 1;

	// cout << "===========orth" << endl;
	// cout << "第1行" << orth.Row(0).x << "," << orth.Row(0).y << "," << orth.Row(0).z << "," << orth.Row(0).w << endl;
	// cout << "第2行" << orth.Row(1).x << "," << orth.Row(1).y << "," << orth.Row(1).z << "," << orth.Row(1).w << endl;
	// cout << "第3行" << orth.Row(2).x << "," << orth.Row(2).y << "," << orth.Row(2).z << "," << orth.Row(2).w << endl;
	// cout << "第4行" << orth.Row(3).x << "," << orth.Row(3).y << "," << orth.Row(3).z << "," << orth.Row(3).w << endl;

	// cout << "===========pertoorth" << endl;
	// cout << "第1行" << pertoorth.Row(0).x << "," << pertoorth.Row(0).y << "," << pertoorth.Row(0).z << "," << pertoorth.Row(0).w << endl;
	// cout << "第2行" << pertoorth.Row(1).x << "," << pertoorth.Row(1).y << "," << pertoorth.Row(1).z << "," << pertoorth.Row(1).w << endl;
	// cout << "第3行" << pertoorth.Row(2).x << "," << pertoorth.Row(2).y << "," << pertoorth.Row(2).z << "," << pertoorth.Row(2).w << endl;
	// cout << "第4行" << pertoorth.Row(3).x << "," << pertoorth.Row(3).y << "," << pertoorth.Row(3).z << "," << pertoorth.Row(3).w << endl;

	// cout << "===========" << endl;
	// return orth * pertoorth;

	float fax = 1.0f / (float)tan(fovy * 0.5f / 180.f * 3.1415926f);
	Mat4x4f m = matrix_set_zero();
	m.m[0][0] = (float)(fax / aspect);
	m.m[1][1] = (float)(fax);
	m.m[2][2] = -(zf + zn) / (zf - zn);
	m.m[2][3] = -2 * zn * zf / (zf - zn);
	m.m[3][2] = 1;

//	cout << "===========投影矩阵" << endl;
//	cout << "第1行" << m.Row(0).x << "," << m.Row(0).y << "," << m.Row(0).z << "," << m.Row(0).w << endl;
//	cout << "第2行" << m.Row(1).x << "," << m.Row(1).y << "," << m.Row(1).z << "," << m.Row(1).w << endl;
//	cout << "第3行" << m.Row(2).x << "," << m.Row(2).y << "," << m.Row(2).z << "," << m.Row(2).w << endl;
//	cout << "第4行" << m.Row(3).x << "," << m.Row(3).y << "," << m.Row(3).z << "," << m.Row(3).w << endl;
	return m;
}

inline static Mat4x4f matrix_set_lookat_opengl(const Vec3f &eye, const Vec3f &center, const Vec3f &up)
{
	//指向了摄像机观察方向的反方向  右手坐标系 看向z的负方向 z轴
	Vec3f f = vector_normalize(center - eye);
	//通过-z与up方向叉乘得到x
	Vec3f s = vector_normalize(vector_cross(f, up));
	//通过x与z得到当前y
	Vec3f u = vector_cross(s, f);
//	cout << "zaxis：" << f.x << "," << f.y << "," << f.z << endl;
//	cout << "xaxis：" << s.x << "," << s.y << "," << s.z << endl;
//	cout << "yaxis：" << u.x << "," << u.y << "," << u.z << endl;
	Mat4x4f view;

	view[0][0] = s.x;
	view[0][1] = u.x;
	view[0][2] = -f.x;
	view[0][3] = -vector_dot(s, eye);

	view[1][0] = s.y;
	view[1][1] = u.y;
	view[1][2] = -f.y;
	view[1][3] = -vector_dot(u, eye);

	view[2][0] = s.z;
	view[2][1] = u.z;
	view[2][2] = -f.z;
	view[2][3] = vector_dot(f, eye);

	view[3][0] = 0;
	view[3][1] = 0;
	view[3][2] = 0;
	view[3][3] = 1;

	cout << "===========view" << endl;
	cout << "第1行" << view.Row(0).x << "," << view.Row(0).y << "," << view.Row(0).z << "," << view.Row(0).w << endl;
	cout << "第2行" << view.Row(1).x << "," << view.Row(1).y << "," << view.Row(1).z << "," << view.Row(1).w << endl;
	cout << "第3行" << view.Row(2).x << "," << view.Row(2).y << "," << view.Row(2).z << "," << view.Row(2).w << endl;
	cout << "第4行" << view.Row(3).x << "," << view.Row(3).y << "," << view.Row(3).z << "," << view.Row(3).w << endl;

	cout << "===========" << endl;
	return view;
}

inline static Mat4x4f matrix_set_lookat_opengl(const Vec3f &eyepos)
{
	Mat4x4f view = matrix_set_identity();
	view[0][3] = -eyepos[0];
	view[1][3] = -eyepos[1];
	view[2][3] = -eyepos[2];

	cout << "===========view" << endl;
	cout << "第1行" << view.Row(0).x << "," << view.Row(0).y << "," << view.Row(0).z << "," << view.Row(0).w << endl;
	cout << "第2行" << view.Row(1).x << "," << view.Row(1).y << "," << view.Row(1).z << "," << view.Row(1).w << endl;
	cout << "第3行" << view.Row(2).x << "," << view.Row(2).y << "," << view.Row(2).z << "," << view.Row(2).w << endl;
	cout << "第4行" << view.Row(3).x << "," << view.Row(3).y << "," << view.Row(3).z << "," << view.Row(3).w << endl;

	cout << "===========" << endl;

	return view;
}

//绝对值
template <typename T>
inline T Abs(T x) { return (x < 0) ? (-x) : x; }
//最大值
template <typename T>
inline T Max(T x, T y) { return (x < y) ? y : x; }
//最小值
template <typename T>
inline T Min(T x, T y) { return (x > y) ? y : x; }

//限制x的范围
template <typename T>
inline T Clamp(T xmin, T xmax, T x)
{
    return Min(Max(xmin, x), xmax);
}

// 矢量转整数颜色
inline static uint32_t vector_to_color(const Vec4f &color)
{
    uint32_t r = (uint32_t)Clamp(0, 255, (int)(color.r * 255.0f));
    uint32_t g = (uint32_t)Clamp(0, 255, (int)(color.g * 255.0f));
    uint32_t b = (uint32_t)Clamp(0, 255, (int)(color.b * 255.0f));
    uint32_t a = (uint32_t)Clamp(0, 255, (int)(color.a * 255.0f));
    return (r << 16) | (g << 8) | b | (a << 24);
}

// 整数颜色到矢量
inline static Vec4f vector_from_color(uint32_t rgba)
{
    Vec4f out;
    out.r = ((rgba >> 16) & 0xff) / 255.0f;
    out.g = ((rgba >> 8) & 0xff) / 255.0f;
    out.b = ((rgba >> 0) & 0xff) / 255.0f;
    out.a = ((rgba >> 24) & 0xff) / 255.0f;
    return out;
}

// 双线性插值计算：给出四个点的颜色，以及坐标偏移，计算结果
inline static uint32_t BilinearInterp(uint32_t tl, uint32_t tr,
                                      uint32_t bl, uint32_t br, int32_t distx, int32_t disty)
{
    uint32_t f, r;
    int32_t distxy = distx * disty;
    int32_t distxiy = (distx << 8) - distxy; /* distx * (256 - disty) */
    int32_t distixy = (disty << 8) - distxy; /* disty * (256 - distx) */
    int32_t distixiy = 256 * 256 - (disty << 8) - (distx << 8) + distxy;
    r = (tl & 0x000000ff) * distixiy + (tr & 0x000000ff) * distxiy + (bl & 0x000000ff) * distixy + (br & 0x000000ff) * distxy;
    f = (tl & 0x0000ff00) * distixiy + (tr & 0x0000ff00) * distxiy + (bl & 0x0000ff00) * distixy + (br & 0x0000ff00) * distxy;
    r |= f & 0xff000000;
    tl >>= 16;
    tr >>= 16;
    bl >>= 16;
    br >>= 16;
    r >>= 16;
    f = (tl & 0x000000ff) * distixiy + (tr & 0x000000ff) * distxiy + (bl & 0x000000ff) * distixy + (br & 0x000000ff) * distxy;
    r |= f & 0x00ff0000;
    f = (tl & 0x0000ff00) * distixiy + (tr & 0x0000ff00) * distxiy + (bl & 0x0000ff00) * distixy + (br & 0x0000ff00) * distxy;
    r |= f & 0xff000000;
    return r;
}

//二维向量叉乘, 叉乘的结果其实是向量，方向垂直于两个向量组成的平面，这里我们只需要其大小和方向
inline static float CrossProduct(Vec2f a,Vec2f b)
 {
     return a.x * b.y - a.y * b.x;
 }

 //二维向量点积
inline static float DotProduct(Vec2f a,Vec2f b)
 {
     return a.x * b.x + a.y * b.y;
 }

 //二维向量减法
inline static Vec2f Minus(Vec2f a,Vec2f b)
 {
     return Vec2f(a.x - b.x, a.y - b.y);
 }
  
 //判断点M,N是否在直线AB的同一侧
inline static  bool IsPointAtSameSideOfLine( Vec2f &pointM,  Vec2f &pointN,
  Vec2f &pointA,  Vec2f &pointB)
 {
    Vec2f AB = Minus(pointB, pointA);
    Vec2f AM = Minus(pointM,pointA);
    Vec2f AN = Minus(pointN,pointA);
  
    //等于0时表示某个点在直线上
    return  CrossProduct(AB,AM) * CrossProduct(AB,AN) >= 0;
 }

//计算三角形面积
inline static  double ComputeTriangleArea(Vec2f pointA,Vec2f pointB,Vec2f pointC)
 {
     //依据两个向量的叉乘来计算
     Vec2f AB = Minus(pointB,pointA);
     Vec2f BC = Minus(pointC,pointB);
     return fabs(CrossProduct(AB,BC) / 2.0);
 }


// 通过判断面积是否相等
inline static bool IsPointInTriangle1(Vec2f pointA,Vec2f pointB,Vec2f pointC, Vec2f pointP)
 {
    double area_ABC = ComputeTriangleArea(pointA,pointB,pointC);
    double area_PAB = ComputeTriangleArea(pointP,pointA,pointB);
    double area_PAC = ComputeTriangleArea(pointP,pointA,pointC);
    double area_PBC = ComputeTriangleArea(pointP,pointB,pointC);
  
    if(fabs(area_PAB + area_PBC + area_PAC - area_ABC) < 0.000001)
        return true;
    else return false;
 }

//通过判断点在直线同侧
inline static bool IsPointInTriangle2(Vec2f pointA,Vec2f pointB,Vec2f pointC, Vec2f pointP)
 {
 return IsPointAtSameSideOfLine(pointP, pointA, pointB, pointC) &&
        IsPointAtSameSideOfLine(pointP, pointB, pointA, pointC) &&
        IsPointAtSameSideOfLine(pointP, pointC, pointA, pointB);
 }

//根据向量基本定理和点在三角形内部充要条件判断
inline static bool IsPointInTriangle3(Vec2f pointA,Vec2f pointB,Vec2f pointC, Vec2f pointP)
 {
 Vec2f AB = Minus(pointB,pointA);
 Vec2f AC = Minus(pointC,pointA);
 Vec2f AP = Minus(pointP,pointA);
 double dot_ac_ac = DotProduct(AC,AC);
 double dot_ac_ab = DotProduct(AC,AB);
 double dot_ac_ap = DotProduct(AC,AP);
 double dot_ab_ab = DotProduct(AB,AB);
 double dot_ab_ap = DotProduct(AB,AP);
  
 double tmp = 1.0 / (dot_ac_ac * dot_ab_ab - dot_ac_ab * dot_ac_ab);
  
 double u = (dot_ab_ab * dot_ac_ap - dot_ac_ab * dot_ab_ap) * tmp;
 if(u < 0 || u > 1)
 return false;
 double v = (dot_ac_ac * dot_ab_ap - dot_ac_ab * dot_ac_ap) * tmp;
 if(v < 0 || v > 1)
 return false;
  
 return u + v <= 1;
 }

// t1 = PA^PB, t2 = PB^PC,  t3 = PC^PA, t1,t2,t3 同号则 P在三角形内部
bool IsPointInTriangle4(Vec2f pointA,Vec2f pointB,Vec2f pointC, Vec2f pointP)
{
    Vec2f PA = Minus(pointA_,pointP);
    Vec2f PB = Minus(pointB_,pointP);
    Vec2f PC = Minus(pointC_,pointP);
double t1 = CrossProduct(PA,PB);
double t2 = PB.CrossProduct(PC);
double t3 = PC.CrossProduct(PA);
return t1*t2 >= 0 && t1*t3 >= 0 &&t2*t3>=0;
}
};

#endif
