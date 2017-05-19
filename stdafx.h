#pragma once

#include <iostream>
#include <assert.h>
#include <stdint.h>
#include <vector>
#include <chrono>
#include <float.h>

#define GLM_FORCE_INLINE
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

#if !defined(_WIN32) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

#include "Utils.h"
#include "RasterizerConfig.h"