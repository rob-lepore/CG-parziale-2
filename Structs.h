#pragma once
#include "Lib.h"

#include <string>

typedef struct {
	float fovY, aspect, near_plane, far_plane;
} Perspective;


typedef enum {
	NONE,
	BLINNPHONG,
	PHONG,
	CARTOON,
	GOURAUD
} ShaderOption;