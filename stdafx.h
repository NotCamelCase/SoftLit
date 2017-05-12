#pragma once

#include <iostream>
#include <vector>
#include <chrono>
#include <assert.h>
#include <stdint.h>

// Force right-handed coordinate system
#define GLM_FORCE_INLINE 
#if GLM_COORDINATE_SYSTEM != GLM_RIGHT_HANDED
#undef GLM_COORDINATE_SYSTEM
#define GLM_COORDINATE_SYSTEM GLM_RIGHT_HANDED
#endif
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <SDL.h>

#include "Utils.h"
#include "RasterizerConfig.h"