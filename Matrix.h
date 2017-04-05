#pragma once

#include "Vector.h"

namespace softlit
{
	template<typename T, int N>
	struct Matrix
	{
		Matrix<T, N>() {}
		~Matrix<T, N>() {}

		const T* operator[](size_t index) const
		{
			return data[index];
		}

		T* operator[](size_t index)
		{
			return data[index];
		}

		T data[N][N];
	};

	template<typename T>
	struct Matrix<T, 2>
	{
		Matrix<T, 2>()
			: Matrix<T, 2>(1, 0, 0, 1)
		{
		}

		Matrix<T, 2>(T a1, T a2, T b1, T b2)
		{
			data[0][0] = a1;
			data[0][1] = a2;
			data[1][0] = b1;
			data[1][1] = b2;
		}

		const T* operator[](size_t index) const
		{
			return data[index];
		}

		T* operator[](size_t index)
		{
			return data[index];
		}

		T determinant() const
		{
			const T a1 = data[0][0];
			const T a2 = data[0][1];

			const T b1 = data[1][0];
			const T b2 = data[1][1];

			return (a1 * b2) - (b1 * a2);
		}

		void invert()
		{
			const T det = determinant();
			if (det == 0) return; // Singular

			const T a1 = data[0][0];
			const T a2 = data[0][1];

			const T b1 = data[1][0];
			const T b2 = data[1][1];

			Matrix<T, 2> C = { b2, -a2, b1, a1 };

			*this = C * (1 / det);
		}

		T data[2][2];
	};

	template<typename T>
	struct Matrix<T, 3>
	{
		Matrix<T, 3>()
			: Matrix<T, 3>(1, 0, 0, 0, 1, 0, 0, 0, 1)
		{
		}

		Matrix<T, 3>(T a1, T a2, T a3,
				     T b1, T b2, T b3,
					 T c1, T c2, T c3)
		{
			data[0][0] = a1;
			data[0][1] = a2;
			data[0][2] = a3;

			data[1][0] = b1;
			data[1][1] = b2;
			data[1][2] = b3;

			data[2][0] = c1;
			data[2][1] = c2;
			data[2][2] = c3;
		}

		const T* operator[](size_t index) const
		{
			return data[index];
		}

		T* operator[](size_t index)
		{
			return data[index];
		}

		T determinant() const
		{
			const T a0 = data[0][0];
			const T a1 = data[0][1];
			const T a2 = data[0][2];

			const T b0 = data[1][0];
			const T b1 = data[1][1];
			const T b2 = data[1][2];

			const T c0 = data[2][0];
			const T c1 = data[2][1];
			const T c2 = data[2][2];

			const T m0 = (b1 * c2) - (c1 * b2);
			const T m1 = (b0 * c2) - (c0 * b2);
			const T m2 = (b0 * c1) - (c0 * b1);

			return (a0 * m0) - (a1 * m1) + (a2 * m2);
		}

		void invert()
		{
			const T det = determinant();
			if (det == 0.0) return; // Singular matrix

			const T a0 = data[0][0];
			const T a1 = data[0][1];
			const T a2 = data[0][2];

			const T b0 = data[1][0];
			const T b1 = data[1][1];
			const T b2 = data[1][2];

			const T c0 = data[2][0];
			const T c1 = data[2][1];
			const T c2 = data[2][2];

			// Cofactors
			const T c00 = (b1 * c2) - (c1 * b2);
			const T c01 = (c0 * b2) - (b0 * c2);
			const T c02 = (b0 * c1) - (c0 * b1);

			const T c10 = (c1 * a2) - (c2 * a1);
			const T c11 = (a0 * c2) - (c0 * a2);
			const T c12 = (c0 * a1) - (a0 * c1);

			const T c20 = (b2 * a1) - (b1 * a2);
			const T c21 = (b0 * a2) - (a0 * b2);
			const T c22 = (a0 * b1) - (b0 * a1);

			// Matrix of cofactors, already transposed
			Matrix<T, 3> C = { c00, c10, c20, c01, c11, c21, c02, c12, c22 };

			*this = C * (1 / det);
		}

		T data[3][3];
	};

	template<typename T>
	struct Matrix<T, 4>
	{
		Matrix<T, 4>()
			: Matrix<T, 4>(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)
		{
		}

		Matrix<T, 4>(T a1, T a2, T a3, T a4,
			T b1, T b2, T b3, T b4,
			T c1, T c2, T c3, T c4,
			T d1, T d2, T d3, T d4)
		{
			data[0][0] = a1;
			data[0][1] = a2;
			data[0][2] = a3;
			data[0][3] = a4;

			data[1][0] = b1;
			data[1][1] = b2;
			data[1][2] = b3;
			data[1][3] = b4;

			data[2][0] = c1;
			data[2][1] = c2;
			data[2][2] = c3;
			data[2][3] = c4;

			data[3][0] = d1;
			data[3][1] = d2;
			data[3][2] = d3;
			data[3][3] = d4;
		}

		const T* operator[](size_t index) const
		{
			return data[index];
		}

		T* operator[](size_t index)
		{
			return data[index];
		}

		T determinant() const
		{
			const T a1 = data[0][0];
			const T a2 = data[0][1];
			const T a3 = data[0][2];
			const T a4 = data[0][3];

			const T b1 = data[1][0];
			const T b2 = data[1][1];
			const T b3 = data[1][2];
			const T b4 = data[1][3];

			const T c1 = data[2][0];
			const T c2 = data[2][1];
			const T c3 = data[2][2];
			const T c4 = data[2][3];

			const T d1 = data[3][0];
			const T d2 = data[3][1];
			const T d3 = data[3][2];
			const T d4 = data[3][3];

			const T m1 = Matrix<T, 3>(b2, b3, b4, c2, c3, c4, d2, d3, d4).determinant();
			const T m2 = Matrix<T, 3>(b1, b3, b4, c1, c3, c4, d1, d3, d4).determinant();
			const T m3 = Matrix<T, 3>(b1, b2, b4, c1, c2, c4, d1, d2, d4).determinant();
			const T m4 = Matrix<T, 3>(b1, b2, b3, c1, c2, c3, d1, d2, d3).determinant();

			return (a1 * m1) - (a2 * m2) + (a3 * m3) - (a4 * m4);
		}

		void invert()
		{
			const T det = determinant();
			if (det == 0) return; // Singular

			const T a1 = data[0][0];
			const T a2 = data[0][1];
			const T a3 = data[0][2];
			const T a4 = data[0][3];

			const T b1 = data[1][0];
			const T b2 = data[1][1];
			const T b3 = data[1][2];
			const T b4 = data[1][3];

			const T c1 = data[2][0];
			const T c2 = data[2][1];
			const T c3 = data[2][2];
			const T c4 = data[2][3];

			const T d1 = data[3][0];
			const T d2 = data[3][1];
			const T d3 = data[3][2];
			const T d4 = data[3][3];

			const T m11 = Matrix<T, 3>(b2, b3, b4, c2, c3, c4, d2, d3, d4).determinant();
			const T m12 = Matrix<T, 3>(b1, b3, b4, c1, c3, c4, d1, d3, d4).determinant();
			const T m13 = Matrix<T, 3>(b1, b2, b4, c1, c2, c4, d1, d2, d4).determinant();
			const T m14 = Matrix<T, 3>(b1, b2, b3, c1, c2, c3, d1, d2, d3).determinant();

			const T m21 = Matrix<T, 3>(a2, a3, a4, c2, c3, c4, d2, d3, d4).determinant();
			const T m22 = Matrix<T, 3>(a1, a3, a4, c1, c3, c4, d1, d3, d4).determinant();
			const T m23 = Matrix<T, 3>(a1, a2, a4, c1, c2, c4, d1, d2, d4).determinant();
			const T m24 = Matrix<T, 3>(a1, a2, a3, c1, c2, c3, d1, d2, d3).determinant();

			const T m31 = Matrix<T, 3>(a2, a3, a4, b2, b3, b4, d2, d3, d4).determinant();
			const T m32 = Matrix<T, 3>(a1, a3, a4, b1, b3, b4, d1, d3, d4).determinant();
			const T m33 = Matrix<T, 3>(a1, a2, a4, b1, b2, b4, d1, d2, d4).determinant();
			const T m34 = Matrix<T, 3>(a1, a2, a3, b1, b2, b3, d1, d2, d3).determinant();

			const T m41 = Matrix<T, 3>(a2, a3, a4, b2, b3, b4, c2, c3, c4).determinant();
			const T m42 = Matrix<T, 3>(a1, a3, a4, b1, b3, b4, c1, c3, c4).determinant();
			const T m43 = Matrix<T, 3>(a1, a2, a4, b1, b2, b4, c1, c2, c4).determinant();
			const T m44 = Matrix<T, 3>(a1, a2, a3, b1, b2, b3, c1, c2, c3).determinant();

			// Matrix of cofactors, pre-transposed
			Matrix<T, 4> C = { m11, m21, m31, m41, m12, m22, m32, m42, m13, m23, m33, m43, m14, m24, m34, m44 };

			*this = C * (1 / det);
		}

		T data[4][4];
	};

	template<typename T, int N>
	Vector<T, N> operator*(const Matrix<T, N>& m, const Vector<T, N>& v)
	{
		Vector<T, N> res;

		for (int i = 0; i < N; i++)
		{
			const Vector<T, N>& r = m[i];
			res[i] = dot(r, v);
		}

		return res;
	}

	template<typename T, int N>
	Matrix<T, N> operator*(const Matrix<T, N>& m, T scalar)
	{
		Matrix<T, N> ret = m;
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				ret[i][j] *= scalar;
			}
		}

		return ret;
	}

	template<typename T, int N>
	void operator*=(Matrix<T, N>& m, T scalar)
	{
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				m[i][j] *= scalar;
			}
		}
	}

	template<typename T, int N>
	Matrix<T, N>& transpose(Matrix<T, N>& m)
	{
		Matrix<T, N> matrix = m;
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				m[i][j] = matrix[j][i];
			}
		}

		return m;
	}

	template<typename T, int N>
	T determinant(const Matrix<T, N>& m)
	{
		return m.determinant();
	}

	template<typename T, int N>
	void invert(Matrix<T, N>& m)
	{
		m.invert();
	}

	template<typename T, int N>
	Matrix<T, N> inverse(const Matrix<T, N>& m)
	{
		const T det = m.determinant();
		if (det == 0) return Matrix<T, N>(); // Singular

		Matrix<T, N> res = m;
		res.invert();

		return res;
	}

	typedef Matrix<float, 2> mat2;
	typedef Matrix<float, 3> mat3;
	typedef Matrix<float, 4> mat4;

	typedef Matrix<double, 2> mat2d;
	typedef Matrix<double, 3> mat3d;
	typedef Matrix<double, 4> mat4d;
}