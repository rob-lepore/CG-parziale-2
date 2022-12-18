#ifndef __FIGURES__
#define __FIGURES__
#include "Lib.h"
#include "Structs.h"
#include "Materials.h"
#include "Light.h"
extern bool showAnchors;

class Mesh {
public:
	vector<vec3> vertici;
	vector<vec4> colori;
	vector <GLuint>  indici;
	vector<vec3> normali;
	vector<vec2> texCoords;
	vec3 position;
	vec3 scl;
	float thetax;
	float thetay;
	float thetaz;

	mat4 Model;
	int sceltaVS;
	int sceltaFS;
	GLuint VAO;
	GLuint VBO_G;
	GLuint VBO_C;
	GLuint VBO_normali;
	GLuint EBO_indici;
	GLuint VBO_coord_texture;
	//ShadingType shading;
	Material* material;
	string name;
	vec4 ancora_obj;
	vec4 ancora_world;
	unsigned int texture;

	Mesh(string name, Material* material) {
		this->name = name;
		this->position = vec3(0);
		this->scl = vec3(1);
		this->thetax = 0;
		this->thetay = 0;
		this->thetaz = 0;

		this->material = material;
	};

public:

	virtual void build() {};
	void creaVAO();
	virtual void show(unsigned int lsceltaVS, unsigned int lsceltaFS, LightShaderUniform light_unif, unsigned int loc_texture, unsigned int MatModel) = 0;
	void translate(float x, float y, float z) {
		this->position = { x,y,z };
	}
	void scale(float x, float y, float z) {
		this->scl = { x,y,z };
	}
	void rotateX(float theta) {
		this->thetax = theta;
	}
	void rotateY(float theta) {
		this->thetay = theta;
	}
	void rotateZ(float theta) {
		this->thetaz = theta;
	}

	void setShading(ShaderOption shader) {
		switch (shader) {
		case NONE:
			sceltaVS = 0;
			sceltaFS = 0;
			break;
		case GOURAUD:
			sceltaVS = 1;
			sceltaFS = 1;
			break;
		case BLINNPHONG:
			sceltaVS = 2;
			sceltaFS = 3;
			break;
		case PHONG:
			sceltaVS = 3;
			sceltaFS = 2;
			break;
		case CARTOON:
			sceltaVS = 3;
			sceltaFS = 6;
			break;
		}
	}
};

class Primitive : public Mesh{
public:
	Primitive(string name, Material* mat = new Material()) : Mesh(name, mat) {};
	void show(unsigned int lsceltaVS, unsigned int lsceltaFS, LightShaderUniform light_unif, unsigned int loc_texture, unsigned int MatModel);
};

class Cube : public Primitive {
public:
	Cube(string name, Material* material = new Material()) : Primitive(name, material) {
		this->build();
		this->creaVAO();
	};
	void build();
};

class Sphere : public Primitive {
public:
	Sphere(string name, Material* material = new Material()) : Primitive(name, material) {
		this->build();
		this->creaVAO();
	};

	void build();
};

class Cone : public Primitive {
public:
	Cone(string name, Material* material = new Material()) : Primitive(name, material) {
		this->build();
		this->creaVAO();
	};

	void build();
};

class Pyramid : public Primitive {
public:
	Pyramid(string name, Material* material = new Material()) : Primitive(name, material) {
		this->build();
		this->creaVAO();
	};

	void build();
};

class Plane : public Primitive {
public:
	Plane(string name, Material* material = new Material()) : Primitive(name, material) {
		this->build();
		this->creaVAO();
	};

	void build();
};

class PlaneSubdivided : public Primitive {
public:
	int nSubdivisions;
	vec2 textureRepetition;

	PlaneSubdivided(string name, int N, vec2 rep, Material* material = new Material()) : Primitive(name, material) {
		nSubdivisions = N;
		textureRepetition = vec2(1/(N/rep.x), 1 / (N / rep.y));
		this->build();
		this->creaVAO();
	};

	void build();
};

class Cylinder : public Primitive {
public:
	Cylinder(string name, Material* material = new Material()) : Primitive(name, material) {
		this->build();
		this->creaVAO();
	};

	void build();
};

class Torus : public Primitive {
public:
	Torus(string name, Material* material = new Material()) : Primitive(name, material) {
		this->build();
		this->creaVAO();
	};

	void build();
};

class ObjMesh : public Primitive {
public:
	ObjMesh() : Primitive("Obj", new Material()) {
		this->sceltaVS = 3;
		this->sceltaFS = 4;
	};
};

class ObjMeshVector : public Mesh {
public:
	vector<ObjMesh> model3D;
	string filename;
	
	ObjMeshVector(string name, string path) : Mesh(name, new Material()){
		this->filename = path;
	}

	void build();

	void show(unsigned int lsceltaVS, unsigned int lsceltaFS, LightShaderUniform light_unif, unsigned int loc_texture, unsigned int MatModel);
};

class Composite : public Mesh {
public:
	vector<Mesh*> parts;

	Composite(string name, Material* mat = new Material()) : Mesh(name, mat) {};

	void add(Mesh* m) {
		parts.push_back(m);
	}

	void show(unsigned int lsceltaVS, unsigned int lsceltaFS, LightShaderUniform light_unif, unsigned int loc_texture, unsigned int MatModel);

	virtual void build(){};
};

class CubeComposite : public Composite {
public:
	CubeComposite(string name, Material* mat = new Material()) : Composite(name, mat) {
		this->build();
	};
	void build();
};

#endif