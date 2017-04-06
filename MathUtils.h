#pragma once

#include "Vector.h"
#include "Matrix.h"

// General-purpose global math utility functions
namespace softlit
{
	// Constructs a view matrix where camera is originated at EYE pos and looking down AT while UP defines the 3D-cartesian up dir
	static mat4 lookAt(const vec3& eye, const vec3& at, const vec3& up)
	{
		const vec3 forward = normalize(at - eye);
		const vec3 right = cross(forward, up);

		mat4 view =
		{
			right.x, right.y, right.z, 0,
			up.x, up.y, up.z, 0,
			forward.x, forward.y, forward.z, 0,
			eye.x, eye.y, eye.z, 1
		};

		return view;
	}

	// Constructs a perspective projection matrix based on FOV, viewport aspect ratio, near and far planes
	static mat4 perspective(const float fov, const float aspect, const float n, const float f)
	{
		mat4 proj;

		//TODO

		return proj;
	}
}