#include "Lib.h"

class Material {

public:
	
	string name;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	GLfloat shininess;

	Material() {
		ambient = glm::vec3(1, 1, 1);
		diffuse = glm::vec3(0, 0, 0);
		specular = glm::vec3(0, 0, 0);
		shininess = 1.f;
	};

	Material(vec3 amb, vec3 diff, vec3 spec, float shine) {
		ambient = amb;
		diffuse = diff;
		specular = spec;
		shininess = shine;
	};

};

class Emerald : public Material {
public:
	Emerald() {
		name = "Emerald";
		ambient = { 0.0215, 0.04745, 0.0215 };
		diffuse = { 0.07568, 0.61424, 0.07568 };
		specular = { 0.633, 0.727811, 0.633 };
		shininess = 78.8f;
	}
};

class Brass : public Material {
public:
	Brass() {
		ambient = { 0.5, 0.06, 0.015 };
		diffuse = { 0.78, 0.57, 0.11 };
		specular = { 0.99, 0.91, 0.81 };
		shininess = 27.8f;
	}
};

class RedPlastic : public Material {
public:
	RedPlastic() {
		ambient = { 0.1, 0.0, 0.0 };
		diffuse = { 0.6, 0.1, 0.1 };
		specular = { 0.7, 0.6, 0.6 };
		shininess = 150.0f;
	}
};

class SnowWhite : public Material{
public:
	SnowWhite() {
		ambient = { 0.2, 0.2, 0.2 };
		diffuse = { 0.95, 0.96, 0.98 };
		specular = { 0.8, 0.8, 0.8 };
		shininess = 1.78125f;
	}
};

class Pink : public Material {
public:
	Pink() {
		ambient = { 0.05f,0.0f,0.0f };
		diffuse = { 0.5f,0.4f,0.4f };
		specular = { 0.7f,0.04f,0.04f };
		shininess = 1.78125f;
	}
};

class DarkWood : public Material {
public:
	DarkWood() {
		ambient = { 0.05f,0.0f,0.0f };
		diffuse = { 0.2f,0.1f,0.05f };
		specular = { 0.9f,0.9f,0.8f };
		shininess = 30.f;
	}
};

class RedBrick : public Material {
public:
	RedBrick() {
		ambient = { 0.05f,0.0f,0.0f };
		diffuse = { 0.2f,0.05f,0.05f };
		specular = { 0.1f,0.1f,0.1f };
		shininess = 2.f;
	}
};

class Parquet : public Material {
public:
	Parquet() {
		ambient = { 0.05f,0.0f,0.0f };
		diffuse = { 0.6f,0.4f,0.1f };
		specular = { 0.9f,0.9f,0.8f };
		shininess = 30.f;
	}
};

class Stucco : public Material {
public:
	Stucco() {
		ambient = { 0.05f,0.0f,0.0f };
		diffuse = { 0.6f,0.6f,0.6f };
		specular = { 0.2f,0.2f,0.2f };
		shininess = 30.f;
	}
};

class Fabric : public Material {
public:
	Fabric() {
		ambient = { 0.02, 0.02, 0.02 };
		diffuse = { 0.6, 0.6, 0.6 };
		specular = { 0.1, 0.1, 0.1 };
		shininess = 50.f;
	}
};


typedef enum {
	EMERALD,
	BRASS,
	REDPLASTIC,
	SNOWWHITE,
	PINK,
	DARKWOOD,
	REDBRICK,
	STUCCO,
	PARQUET,
	FABRIC,

} MaterialOption;