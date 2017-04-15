#pragma once

#include "Vector.h"
#include "Matrix.h"

// General-purpose global math utility functions
namespace softlit
{
	// Constructs a view matrix where camera is originated at EYE pos and looking down AT while UP defines the 3D-cartesian up dir
	static mat4 lookAt(const vec3& eye, const vec3& at, const vec3& upDir)
	{
		// Form a 3D cartesian coordinate system
		const vec3 zAxis = normalize(eye - at);
		const vec3 xAxis = cross(normalize(upDir), zAxis);
		const vec3 yAxis = cross(xAxis, zAxis);

		mat4 view;

		view[0][0] = xAxis.x;
		view[0][1] = xAxis.y;
		view[0][2] = xAxis.z;

		view[1][0] = yAxis.x;
		view[1][1] = yAxis.y;
		view[1][2] = yAxis.z;

		view[2][0] = zAxis.x;
		view[2][1] = zAxis.y;
		view[2][2] = zAxis.z;

		view[3][0] = -dot(eye, xAxis);
		view[3][1] = -dot(eye, yAxis);
		view[3][2] = -dot(eye, zAxis);

		return view;
	}

	// Constructs a perspective projection matrix based on FOV, viewport aspect ratio, near and far planes
	static mat4 perspective(const float fov, const float aspect, const float n, const float f)
	{
		// Calculate (l, r) & (b, t) frustum points
		const float t = n * tanf(fov * 0.5f * M_PI / 180.f);
		const float b = -t;

		const float l = aspect * b;
		const float r = -l;

		mat4 proj;

		proj[0][0] = (2 * n) / (r - l);
		proj[0][2] = (r + l) / (r - l);

		proj[1][1] = (2 * n) / (t - b);
		proj[1][2] = (t + b) / (t - b);

		proj[2][2] = -(f + n) / (f - n);
		proj[2][3] = -(2 * n * f) / (f - n);

		proj[3][2] = -1;
		proj[3][3] = 0;

		return proj;
	}
}