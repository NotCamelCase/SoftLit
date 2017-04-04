#pragma once

namespace softlit {
	template<class T, int N>
	class Vector
	{
	public:
		Vector<T, N>() {}
		~Vector<T, N>() {}

		T data[N];

		T& operator[](short index)
		{
			return data[index];
		}

		const T& operator[](short index) const
		{
			return data[index];
		}
	};

	template<class T>
	class Vector<T, 2>
	{
	public:
		Vector<T, 2>() : x(T(0)), y(T(0)) {}
		Vector<T, 2>(T all) : x(all), y(all) {}
		Vector<T, 2>(T _x, T _y) : x(_x), y(_y) {}
		~Vector<T, 2>() {}

		union
		{
			struct { T x, y; };
			T data[2];
		};

		T& operator[](short index)
		{
			return data[index];
		}

		const T& operator[](short index) const
		{
			return data[index];
		}
	};

	template<class T>
	class Vector<T, 3>
	{
	public:
		Vector<T, 3>() : x(T(0)), y(T(0)), z(T(0)) {}
		Vector<T, 3>(T val) : x(val), y(val), z(val) {}
		Vector<T, 3>(T _x, T _y) : x(_x), y(_y), z(T(0)) {}
		Vector<T, 3>(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}

		Vector<T, 3>(const Vector<T, 2>& other)
		{
			x = other[0];
			y = other[1];
			z = T(0);
		}

		union
		{
			struct { T x, y, z; };
			struct { T r, g, b; };
			T data[3];
		};

		T& operator[](short index)
		{
			return data[index];
		}

		const T& operator[](short index) const
		{
			return data[index];
		}
	};

	template<class T>
	class Vector<T, 4>
	{
	public:
		Vector<T, 4>() : x(0), y(0), z(0), w(0) {}
		Vector<T, 4>(T all) : x(all), y(all), z(all), w(all) {}
		Vector<T, 4>(T _x, T _y) : x(_x), y(_y), z(T(0)), w(T(0)) {}
		Vector<T, 4>(T _x, T _y, T _z) : x(_x), y(_y), z(_z), w(T(0)) {}
		Vector<T, 4>(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {}
		~Vector<T, 4>() {}

		Vector<T, 4>(const Vector<T, 3> other)
		{
			x = other[0];
			y = other[1];
			z = other[2];
			w = T(0);
		}

		union
		{
			struct { T x, y, z, w; };
			struct { T r, g, b, a; };
			T data[4];
		};

		T& operator[](short index)
		{
			return data[index];
		}

		const T& operator[](short index) const
		{
			return data[index];
		}
	};

	template<class T, int N>
	Vector<T, N> operator+(const Vector<T, N>& a, const Vector<T, N> &b)
	{
		Vector<T, N> res;
		for (int i = 0; i < N; i++)
		{
			res[i] = a[i] + b[i];
		}

		return res;
	}

	template<class T, int N>
	Vector<T, N>& operator+=(Vector<T, N>& l, const Vector<T, N>& r)
	{
		for (int i = 0; i < N; i++)
		{
			l[i] += r[i];
		}

		return l;
	}

	template<class T, int N>
	Vector<T, N> operator-(const Vector<T, N>& self)
	{
		Vector<T, N> res;
		for (int i = 0; i < N; i++)
		{
			res[i] = -self[i];
		}

		return res;
	}

	template<class T, int N>
	Vector<T, N> operator-(const Vector<T, N>& l, const Vector<T, N> &r)
	{
		Vector<T, N> res;
		for (int i = 0; i < N; i++)
		{
			res[i] = l[i] - r[i];
		}

		return res;
	}

	template<class T, int N>
	Vector<T, N>& operator-=(Vector<T, N> &l, const Vector<T, N>& r)
	{
		for (int i = 0; i < N; i++)
		{
			l[i] -= r[i];
		}

		return l;
	}

	template<class T, int N>
	Vector<T, N> operator*(const Vector<T, N>& a, typename std::common_type<T>::type f)
	{
		Vector<T, N> res;
		for (int i = 0; i < N; i++)
		{
			res[i] = a[i] * f;
		}

		return res;
	}

	template<class T, int N>
	Vector<T, N> operator*(const Vector<T, N>& a, const Vector<T, N> &b)
	{
		Vector<T, N> res;
		for (int i = 0; i < N; i++)
		{
			res[i] = a[i] * b[i];
		}

		return res;
	}

	template<class T, int N>
	Vector<T, N>& operator*=(Vector<T, N>& l, const Vector<T, N> &r)
	{
		for (int i = 0; i < N; i++)
		{
			l[i] *= r[i];
		}

		return l;
	}

	template<typename T, int N>
	Vector<T, N>& operator*=(Vector<T, N>& l, typename std::common_type<T>::type f)
	{
		for (int i = 0; i < N; i++)
		{
			l[i] *= f;
		}

		return l;
	}

	template<class T, int N>
	Vector<T, N> operator/(const Vector<T, N>& v, typename std::common_type<T>::type f)
	{
		Vector<T, N> res;
		for (int i = 0; i < N; i++)
		{
			res[i] = v[i] / f;
		}

		return res;
	}

	template<class T, int N>
	Vector<T, N>& operator/=(Vector<T, N>& a, typename std::common_type<T>::type f)
	{
		for (int i = 0; i < N; i++)
		{
			a[i] /= f;
		}

		return a;
	}

	template<class T, int N>
	bool operator==(const Vector<T, N>& a, const Vector<T, N>& b)
	{
		for (int i = 0; i < N; i++)
		{
			if (!fabs(a[i]) - fabs(b[i]) <= FLT_EPSILON)
				return false;
		}

		return true;
	}

	template<class T, int N>
	bool operator!=(const Vector<T, N>& a, const Vector<T, N>& b)
	{
		return !(a == b);
	}

	template<class T, int N>
	T dot(const Vector<T, N>& a, const Vector<T, N>& b)
	{
		T sum(0);
		for (int i = 0; i < N; i++)
		{
			sum += a[i] * b[i];
		}

		return sum;
	}

	template<class T, int N>
	Vector<T, N> reflect(const Vector<T, N>& v, const Vector<T, N>& n)
	{
		return v - n * T(2) * dot(n, v);
	}

	template<class T>
	Vector<T, 3> cross(const Vector<T, 3>& l, const Vector<T, 3>& r)
	{
		return Vector<T, 3>(l.y * r.z - r.y * l.z, l.x * r.z - r.x * l.z, l.x * r.y - r.x * l.y);
	}

	template<class T, int N>
	Vector<T, N> clamp(const Vector<T, N>& v)
	{
		Vector<T, N> res;
		for (int i = 0; i < N; i++)
		{
			const T val = v[i];
			res[i] = val < 0 ? 0 : val > 1 ? 1 : val;
		}

		return res;
	}

	template<class T, int N>
	Vector<T, N>& normalize(Vector<T, N>& a)
	{
		T norm = length2(a);
		T invNor = 1 / sqrt(norm);
		a *= invNor;

		return a;
	}

	template<class T, int N>
	T length2(const Vector<T, N>& a)
	{
		return dot(a, a);
	}

	template<class T, int N>
	T length(const Vector<T, N>& a)
	{
		return sqrt(length2(a));
	}

	typedef Vector<float, 2> vec2;
	typedef Vector<double, 2> vec2d;
	typedef Vector<int, 2> vec2i;
	typedef Vector<float, 3> vec3;
	typedef Vector<double, 3> vec3d;
	typedef Vector<int, 3> vec3i;
	typedef Vector<float, 4> vec4;
	typedef Vector<double, 4> vec4d;
	typedef Vector<int, 4> vec4i;
}