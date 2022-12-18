#ifndef __LIGHT__
#define __LIGHT__

#include "Lib.h"

class Light { 
public:
	glm::vec4 vector; //se w = 0, direzionale, altrimenti puntiforme
	glm::vec3 color;
	GLfloat power;

	Light() {
		vector = { 0,0,0,0 };
		color = { 1,1,1 };
		power = 1;
	}

	Light(vec4 vector, vec3 color, GLfloat power) {
		this->vector = vector;
		this->color = color;
		this->power = power;
	}
};

typedef struct {
	GLuint light_color_pointer;
	GLuint light_power_pointer;
	GLuint light_vector_pointer;
	GLuint material_diffuse;
	GLuint material_ambient;
	GLuint material_specular;
	GLuint material_shininess;
} LightShaderUniform;

#endif