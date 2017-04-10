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
		const vec3 forward = normalize(eye - at);
		const vec3 right = cross(normalize(upDir), forward);
		const vec3 up = cross(forward, right);

		mat4 view;

		view[0][0] = right.x;
		view[0][1] = right.y;
		view[0][2] = right.z;

		view[1][0] = up.x;
		view[1][1] = up.y;
		view[1][2] = up.z;

		view[2][0] = forward.x;
		view[2][1] = forward.y;
		view[2][2] = forward.z;

		view[3][0] = eye.x;
		view[3][1] = eye.y;
		view[3][2] = eye.z;

		return view;
	}

	// Constructs a perspective projection matrix based on FOV, viewport aspect ratio, near and far planes
	static mat4 perspective(const float fov, const float aspect, const float n, const float f)
	{
		mat4 proj;

		// Calculate (l, r) & (b, t) frustum points
		const float t = n * tanf(fov * 0.5f * M_PI / 180.f);
		const float b = -t;

		const float r = aspect * t;
		const float l = -r;

		proj[0][0] = (2 * n) / (r - l);
		proj[0][2] = (r + l) / (r - l);

		proj[1][1] = (2 * n) / (t - b);
		proj[1][2] = (t + b) / (t - b);

		proj[2][2] = -(f + n) / (f - n);
		proj[2][3] = (-2 * n * f) / (f - n);

		proj[3][2] = -1;
		proj[3][3] = 0;

		return proj;
	}
}