#include "Figures.h"
#include "Light.h"
#include "iostream"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

void Mesh::creaVAO() {
	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);
	//Genero , rendo attivo, riempio il VBO della geometria dei vertici
	glGenBuffers(1, &this->VBO_G);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO_G);
	glBufferData(GL_ARRAY_BUFFER, this->vertici.size() * sizeof(vec3), this->vertici.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	//Genero , rendo attivo, riempio il VBO dei colori
	glGenBuffers(1, &this->VBO_C);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO_C);
	glBufferData(GL_ARRAY_BUFFER, this->colori.size() * sizeof(vec4), this->colori.data(), GL_STATIC_DRAW);
	//Adesso carico il VBO dei colori nel layer 2
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	//Genero , rendo attivo, riempio il VBO delle normali
	glGenBuffers(1, &this->VBO_normali);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO_normali);
	glBufferData(GL_ARRAY_BUFFER, this->normali.size() * sizeof(vec3), this->normali.data(), GL_STATIC_DRAW);
	//Adesso carico il VBO delle NORMALI nel layer 2
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(2);


	glGenBuffers(1, &this->VBO_coord_texture);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO_coord_texture);
	glBufferData(GL_ARRAY_BUFFER, this->texCoords.size() * sizeof(vec2), this->texCoords.data(), GL_STATIC_DRAW);
	//Adesso carico il VBO delle TEXTURE nel layer 3
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(3);

	//EBO di tipo indici
	glGenBuffers(1, &this->EBO_indici);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO_indici);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indici.size() * sizeof(GLuint), this->indici.data(), GL_STATIC_DRAW);

}

void Primitive::show(unsigned int lsceltaVS, unsigned int lsceltaFS, LightShaderUniform light_unif, unsigned int loc_texture, unsigned int MatModel) {
	this->Model = mat4(1.0);
	this->Model = glm::translate(this->Model, this->position);
	this->Model = glm::scale(this->Model, this->scl);
	this->Model = glm::rotate(this->Model, radians(this->thetax), vec3(1, 0, 0));
	this->Model = glm::rotate(this->Model, radians(this->thetay), vec3(0, 1, 0));
	this->Model = glm::rotate(this->Model, radians(this->thetaz), vec3(0, 0, 1));

	this->ancora_world = this->Model * this->ancora_obj;

	//Passo al Vertex Shader il puntatore alla matrice Model dell'oggetto k-esimo della Scena, che sar� associata alla variabile Uniform mat4 Projection
	//all'interno del Vertex shader. Uso l'identificatio MatModel

	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(this->Model));
	glUniform1i(lsceltaVS, this->sceltaVS);
	glUniform1i(lsceltaFS, this->sceltaFS);
	//Passo allo shader il puntatore ai materiali
	glUniform3fv(light_unif.material_ambient, 1, glm::value_ptr(this->material->ambient));
	glUniform3fv(light_unif.material_diffuse, 1, glm::value_ptr(this->material->diffuse));
	glUniform3fv(light_unif.material_specular, 1, glm::value_ptr(this->material->specular));
	glUniform1f(light_unif.material_shininess, this->material->shininess);

	glBindVertexArray(this->VAO);

	if (showAnchors) {
		int ind = indici.size() - 1;
		glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, BUFFER_OFFSET(ind * sizeof(GLuint)));
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glUniform1i(loc_texture, 0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawElements(GL_TRIANGLES, (this->indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Cube::build() {
	
	vec4 color = { 1,1,1,1 };
	this->vertici.push_back(vec3(-1.0, -1.0, 1.0));
	this->colori.push_back(color);
	this->vertici.push_back(vec3(1.0, -1.0, 1.0));
	this->colori.push_back(color);
	this->vertici.push_back(vec3(1.0, 1.0, 1.0));
	this->colori.push_back(color);
	this->vertici.push_back(vec3(-1.0, 1.0, 1.0));
	this->colori.push_back(color);
	// back
	this->vertici.push_back(vec3(-1.0, -1.0, -1.0));
	this->colori.push_back(color);
	this->vertici.push_back(vec3(1.0, -1.0, -1.0));
	this->colori.push_back(color);
	this->vertici.push_back(vec3(1.0, 1.0, -1.0));
	this->colori.push_back(color);
	this->vertici.push_back(vec3(-1.0, 1.0, -1.0));
	this->colori.push_back(color);



	this->vertici.push_back(vec3(0.0, 0.0, 0.0));
	this->colori.push_back(color);
	this->ancora_obj = (vec4(0.0, 0.0, 0.0, 1.0));

	this->indici.push_back(0); this->indici.push_back(1); this->indici.push_back(2);
	this->indici.push_back(2); this->indici.push_back(3); this->indici.push_back(0);
	this->indici.push_back(1); this->indici.push_back(5); this->indici.push_back(6);
	this->indici.push_back(6); this->indici.push_back(2); this->indici.push_back(1);
	this->indici.push_back(7); this->indici.push_back(6); this->indici.push_back(5);
	this->indici.push_back(5); this->indici.push_back(4); this->indici.push_back(7);
	this->indici.push_back(4); this->indici.push_back(0); this->indici.push_back(3);
	this->indici.push_back(3); this->indici.push_back(7); this->indici.push_back(4);
	this->indici.push_back(4); this->indici.push_back(5); this->indici.push_back(1);
	this->indici.push_back(1); this->indici.push_back(0); this->indici.push_back(4);
	this->indici.push_back(3); this->indici.push_back(2); this->indici.push_back(6);
	this->indici.push_back(6); this->indici.push_back(7); this->indici.push_back(3);

	this->texCoords.push_back(vec2(0, 0));
	this->texCoords.push_back(vec2(1, 0));
	this->texCoords.push_back(vec2(1, 1));
	this->texCoords.push_back(vec2(1, 1));
	this->texCoords.push_back(vec2(0, 1));
	this->texCoords.push_back(vec2(0, 0));

	

	this->normali.push_back(normalize(vec3(-0.5, -0.5, 0)));
	this->normali.push_back(normalize(vec3(0.5, -0.5, 0)));
	this->normali.push_back(normalize(vec3(0.5, 0.5, 0)));
	this->normali.push_back(normalize(vec3(-0.5, 0.5, 0)));
	//back
	this->normali.push_back(normalize(vec3(-0.5, -0.5, -1)));
	this->normali.push_back(normalize(vec3(0.5, -0.5, -1)));
	this->normali.push_back(normalize(vec3(0.5, 0.5, -1)));
	this->normali.push_back(normalize(vec3(-0.5, 0.5, -1)));

	int nv = this->vertici.size();

	this->indici.push_back(nv - 1);
}

void Sphere::build() {
	vec3 centro = vec3(0.0, 0.0, 0.0);
	vec3 raggio = vec3(1.0, 1.0, 1.0);
	float s, t;
	int Stacks = 100;  //numero di suddivisioni sull'asse y
	int Slices = 100;  // numero di suddivisioni sull'asse x

	//Calc The Vertices

	//Stacks= suddivisioni lungo l'asse y
	for (int i = 0; i <= Stacks; ++i) {

		float V = i / (float)Stacks;
		float phi = V * pi <float>();

		// Loop Through Slices suddivisioni lungo l'asse x
		for (int j = 0; j <= Slices; ++j) {

			float U = j / (float)Slices;
			float theta = U * (pi <float>() * 2);

			// Calc The Vertex Positions
			float x = centro.x + raggio.x * (cosf(theta) * sinf(phi));
			float y = centro.y + raggio.y * cosf(phi);
			float z = centro.z + raggio.z * sinf(theta) * sinf(phi);

			this->vertici.push_back(vec3(x, y, z)),
				this->colori.push_back(vec4(1,1,1,1));

			//Normale nel vertice
			this->normali.push_back(vec3(x, y, z));
			//coordinata di Texture
			s = U;
			t = V;
			this->texCoords.push_back(vec2(s, t));

		}
	}

	// Calc The Index Positions
	for (int i = 0; i < Slices * Stacks + Slices; ++i) {

		this->indici.push_back(i);
		this->indici.push_back(i + Slices + 1);
		this->indici.push_back(i + Slices);


		this->indici.push_back(i + Slices + 1);
		this->indici.push_back(i);
		this->indici.push_back(i + 1);
	}

	this->vertici.push_back(vec3(0.0, 0.0, 0.0));
	this->colori.push_back(vec4(0.0, 1.0, 0.0, 1.0));

	this->ancora_obj = (vec4(0.0, 1.0, 0.0, 1.0));


	int nv = this->vertici.size();
	this->indici.push_back(nv - 1);

}

void Cone::build() {
	int Stacks = 100;  //numero di suddivisioni sull'asse x
	int Slices = 100;  // numero di suddivisioni sull'asse y
	float s, t;


	//Calc The Vertices
	for (int i = 0; i <= Stacks; ++i) {

		float V = i / (float)Stacks;
		float h = V;

		// Loop Through Slices
		for (int j = 0; j <= Slices; ++j) {

			float U = j / (float)Slices;
			float theta = U * (glm::pi <float>() * 2);

			// Calc The Vertex Positions
			float x = h * cosf(theta);
			float y = h;
			float z = h * sinf(theta);


			// Push Back Vertex Data
			this->vertici.push_back(vec3(x, y, z));
			this->colori.push_back(vec4(1,1,1,1));
			//Normale nel vertice
			this->normali.push_back(normalize(vec3(cos(theta) / sqrt(2.0f), -1 / sqrt(2.0f), sin(theta) / sqrt(2.0f))));

			//coordinata di texture
			s = U;
			t = V;
			this->texCoords.push_back(vec2(s, t));

		}
	}

	// Calc The Index Positions
	for (int i = 0; i < Slices * Stacks + Slices; ++i) {

		this->indici.push_back(i);
		this->indici.push_back(i + Slices + 1);
		this->indici.push_back(i + Slices);


		this->indici.push_back(i + Slices + 1);
		this->indici.push_back(i);
		this->indici.push_back(i + 1);
	}

	this->ancora_obj = (vec4(0.0, 0.0, 0.0, 1.0));

}

void Pyramid::build() {
	vec4 color = vec4(1, 1, 1, 1);
	this->vertici.push_back(vec3(-1.0, 0.0, 1.0));
	this->colori.push_back(color);
	this->texCoords.push_back(vec2(0,0));
	this->normali.push_back(vec3(-1,0,1));

	this->vertici.push_back(vec3(1.0, 0.0, 1.0));
	this->colori.push_back(color);
	this->texCoords.push_back(vec2(1, 0));
	this->normali.push_back(vec3(1, 0, 1));

	this->vertici.push_back(vec3(1.0, 0.0, -1.0));
	this->colori.push_back(color);
	this->texCoords.push_back(vec2(1, 1));
	this->normali.push_back(vec3(1, 0, -1));

	this->vertici.push_back(vec3(-1.0, 0.0, -1.0));
	this->colori.push_back(color);
	this->texCoords.push_back(vec2(0, 1));
	this->normali.push_back(vec3(-1, 0, -1));

	// Apice piramide
	this->vertici.push_back(vec3(0.0, 1.0, 0.0));
	this->colori.push_back(color);
	this->texCoords.push_back(vec2(0.5, 0.5));
	this->normali.push_back(vec3(0, 1, 0));

	for (int i = 0; i < this->vertici.size(); i++)
		this->normali.push_back(vec3(0.0));

	this->indici.push_back(0); this->indici.push_back(1); this->indici.push_back(2);
	this->indici.push_back(0); this->indici.push_back(2); this->indici.push_back(3);

	this->indici.push_back(0); this->indici.push_back(4); this->indici.push_back(3);
	this->indici.push_back(0); this->indici.push_back(1); this->indici.push_back(4);

	this->indici.push_back(3); this->indici.push_back(2); this->indici.push_back(4);
	this->indici.push_back(1); this->indici.push_back(2); this->indici.push_back(4);

	this->vertici.push_back(vec3(0.0, 0.3, 0.0));
	this->colori.push_back(color);
	this->ancora_obj = (vec4(0.0, 0.3, 0.0, 1.0));
	int nv = this->vertici.size();
	this->indici.push_back(nv - 1);
}

void Plane::build() {
	vec4 color = { 1,1,1,1 };
	this->vertici.push_back(vec3(-0.5, 0.0, 0.5));
	this->colori.push_back(color);
	this->vertici.push_back(vec3(0.5, 0.0, 0.5));
	this->colori.push_back(color);
	this->vertici.push_back(vec3(0.5, 0.0, -0.5));
	this->colori.push_back(color);
	this->vertici.push_back(vec3(-0.5, 0.0, -0.5));
	this->colori.push_back(color);

	this->indici.push_back(0); this->indici.push_back(1); this->indici.push_back(2);
	this->indici.push_back(0); this->indici.push_back(2); this->indici.push_back(3);

	this->normali.push_back(vec3(0.0, 1.0, 0.0));
	this->normali.push_back(vec3(0.0, 1.0, 0.0));
	this->normali.push_back(vec3(0.0, 1.0, 0.0));
	this->normali.push_back(vec3(0.0, 1.0, 0.0));

	this->texCoords.push_back(vec2(0.0, 1.0));
	this->texCoords.push_back(vec2(1.0, 1.0));
	this->texCoords.push_back(vec2(1.0, 0.0));
	this->texCoords.push_back(vec2(0.0, 0.0));


	this->vertici.push_back(vec3(0.0, 0.0, 0.0));
	this->colori.push_back(vec4(1.0, 0.0, 0.0, 1.0));
	this->ancora_obj = (vec4(0.0, 0.0, 0.0, 1.0));
	int nv = this->vertici.size();
	this->indici.push_back(nv - 1);
}

void PlaneSubdivided::build() {
	vec4 color = { 1,1,1,1 };
	int N = nSubdivisions;

	int i, j;

	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			this->vertici.push_back(vec3(-0.5 + (float)i / N, 0.0, -0.5 + (float)j / N));
			this->colori.push_back(color);
			this->normali.push_back(vec3(0.0, 1.0, 0.0));
			this->texCoords.push_back(vec2(i*textureRepetition.x, j*textureRepetition.y));

		}
	}
	int cont = -1;

	for (i = 0; i <= pow(N, 2) - (N + 1); i++) {

		j = i % (N);

		if (j != N - 1)
		{
			this->indici.push_back(i);
			this->indici.push_back(i + 1);
			this->indici.push_back(i + N);

			this->indici.push_back(i + N + 1);
			this->indici.push_back(i + 1);
			this->indici.push_back(i + N);
		}
	}

	this->vertici.push_back(vec3(0.0, 0.0, 0.0));
	this->ancora_obj = (vec4(0.0, 1.0, 0.0, 1.0));
	this->colori.push_back(vec4(0.0, 1.0, 0.0, 1.0));
	int nv = this->vertici.size();
	this->indici.push_back(nv - 1);


}

void Cylinder::build() {
	int Stacks = 50;  //numero di suddivisioni sull'asse x
	int Slices = 50;  // numero di suddivisioni sull'asse y

	float s, t;
	//Calc The Vertices
	for (int i = 0; i <= Stacks; ++i) {

		float V = i / (float)Stacks;
		float h = V;

		// Loop Through Slices
		for (int j = 0; j <= Slices; ++j) {

			float U = j / (float)Slices;
			float theta = U * (glm::pi <float>() * 2);

			// Calc The Vertex Positions
			float x = cosf(theta);
			float y = h;
			float z = sinf(theta);


			// Push Back Vertex Data
			this->vertici.push_back(vec3(x, y, z));
			this->colori.push_back(vec4(1,1,1,1));
			//Normale nel vertice
			this->normali.push_back(vec3(normalize(vec3(cos(theta), 0, sin(theta)))));

			//Coordinata di texture
			s = U;
			t = V;
			this->texCoords.push_back(vec2(s, t));
		}
	}

	// Calc The Index Positions
	for (int i = 0; i < Slices * Stacks + Slices; ++i) {

		this->indici.push_back(i);
		this->indici.push_back(i + Slices + 1);
		this->indici.push_back(i + Slices);


		this->indici.push_back(i + Slices + 1);
		this->indici.push_back(i);
		this->indici.push_back(i + 1);
	}

	this->ancora_obj = (vec4(0.0, 0.0, 0.0, 1.0));

}

void Torus::build() {
	int Stacks = 100;  //numero di suddivisioni sull'asse x
	int Slices = 100;  // numero di suddivisioni sull'asse y
	float R = 1, r = 0.5;
	float s, t;


	//Calc The Vertices
	for (int i = 0; i <= Stacks; ++i) {

		float V = i / (float)Stacks;
		float phi = V * glm::pi <float>() * 2;

		// Loop Through Slices
		for (int j = 0; j <= Slices; ++j) {

			float U = j / (float)Slices;
			float theta = U * (glm::pi <float>() * 2);

			// Calc The Vertex Positions
			float x = (R + r * cosf(phi)) * cosf(theta);
			float y = r * sinf(phi);
			float z = (R + r * cosf(phi)) * sinf(theta);


			// Push Back Vertex Data
			this->vertici.push_back(vec3(x, y, z));
			this->colori.push_back(vec4(1,1,1,1));
			//Normale nel vertice
			//this->normali.push_back(vec3(normalize(vec3(sin(phi) * cos(theta), cos(phi), sin(theta) * sin(phi)))));
			//phi main
			//theta tube
			this->normali.push_back(vec3(normalize(vec3(cos(phi) * cos(theta), sin(theta) * cos(phi), sin(phi) ))));

			//Coordinata di Texture
			s = U;
			t = V;
			this->texCoords.push_back(vec2(s, t));
		}
	}

	// Calc The Index Positions
	for (int i = 0; i < Slices * Stacks + Slices; ++i) {

		this->indici.push_back(i);
		this->indici.push_back(i + Slices + 1);
		this->indici.push_back(i + Slices);


		this->indici.push_back(i + Slices + 1);
		this->indici.push_back(i);
		this->indici.push_back(i + 1);
	}

	this->ancora_obj = (vec4(0.0, 1.0, 0.0, 1.0));

}

void ObjMeshVector::build() {
	int nmeshes = model3D.size();

	for (int i = 0; i < nmeshes; i++)
	{
		model3D[i].creaVAO();
		model3D[i].Model = mat4(1.0);
		model3D[i].Model = glm::translate(model3D[i].Model, this->position);
		model3D[i].Model = glm::scale(model3D[i].Model, this->scl);
		model3D[i].Model = glm::rotate(model3D[i].Model, radians(this->thetax), vec3(1, 0, 0));
		model3D[i].Model = glm::rotate(model3D[i].Model, radians(this->thetay), vec3(0, 1, 0));
		model3D[i].Model = glm::rotate(model3D[i].Model, radians(this->thetaz), vec3(0, 0, 1));

	}

	this->ancora_obj = (vec4(0.0, 0.0, 0.0, 1.0));

}

void ObjMeshVector::show(unsigned int lsceltaVS, unsigned int lsceltaFS, LightShaderUniform light_unif, unsigned int loc_texture, unsigned int MatModel) {
	for (int k = 0; k < model3D.size(); k++)
	{
		
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(this->model3D[k].Model));
		glUniform1i(lsceltaVS, this->model3D[k].sceltaVS);
		glUniform1i(lsceltaFS, this->model3D[k].sceltaFS);
		//Passo allo shader il puntatore ai materiali
		glUniform3fv(light_unif.material_ambient, 1, value_ptr(this->model3D[k].material->ambient));
		glUniform3fv(light_unif.material_diffuse, 1, value_ptr(this->model3D[k].material->diffuse));
		glUniform3fv(light_unif.material_specular, 1, value_ptr(this->model3D[k].material->specular));
		glUniform1f(light_unif.material_shininess, this->model3D[k].material->shininess);

		glBindVertexArray(this->model3D[k].VAO);


		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_TRIANGLES, (this->model3D[k].indici.size()) * sizeof(GLuint), GL_UNSIGNED_INT, 0);


		glBindVertexArray(0);
		
		//model3D[k].show(lsceltaVS, lsceltaFS, light_unif, loc_texture, MatModel);
	}
}

void Composite::show(unsigned int lsceltaVS, unsigned int lsceltaFS, LightShaderUniform light_unif, unsigned int loc_texture, unsigned int MatModel) {
	this->Model = mat4(1.0);
	this->Model = glm::translate(this->Model, this->position);
	this->Model = glm::scale(this->Model, this->scl);
	this->Model = glm::rotate(this->Model, radians(this->thetax), vec3(1, 0, 0));
	this->Model = glm::rotate(this->Model, radians(this->thetay), vec3(0, 1, 0));
	this->Model = glm::rotate(this->Model, radians(this->thetaz), vec3(0, 0, 1));

	for (int i = 0; i < this->parts.size(); i++) {
		parts[i]->Model = this->Model;
		parts[i]->Model = glm::translate(parts[i]->Model, parts[i]->position);
		parts[i]->Model = glm::scale(parts[i]->Model, parts[i]->scl);
		parts[i]->Model = glm::rotate(parts[i]->Model, radians(parts[i]->thetax), vec3(1, 0, 0));
		parts[i]->Model = glm::rotate(parts[i]->Model, radians(parts[i]->thetay), vec3(0, 1, 0));
		parts[i]->Model = glm::rotate(parts[i]->Model, radians(parts[i]->thetaz), vec3(0, 0, 1));


		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(parts[i]->Model));
		glUniform1i(lsceltaVS, this->sceltaVS);
		glUniform1i(lsceltaFS, this->sceltaFS);
		//Passo allo shader il puntatore ai materiali
		glUniform3fv(light_unif.material_ambient, 1, glm::value_ptr(this->material->ambient));
		glUniform3fv(light_unif.material_diffuse, 1, glm::value_ptr(this->material->diffuse));
		glUniform3fv(light_unif.material_specular, 1, glm::value_ptr(this->material->specular));
		glUniform1f(light_unif.material_shininess, this->material->shininess);

		glBindVertexArray(parts[i]->VAO);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glUniform1i(loc_texture, 0);
		glBindTexture(GL_TEXTURE_2D, this->texture);
		glDrawElements(GL_TRIANGLES, (parts[i]->indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}

void CubeComposite::build() {
	Plane* l1 = new Plane("cube face", this->material);
	l1->texture = this->texture;
	l1->translate(0, 0, 0.5);
	l1->rotateX(90);
	Plane* l2 = new Plane("cube face", this->material);
	l2->texture = this->texture;
	l2->translate(0, 0, -0.5);
	l2->rotateX(90);
	l2->rotateZ(180);

	Plane* l3 = new Plane("cube face", this->material);
	l3->texture = this->texture;
	l3->translate(-0.5, 0, 0);
	l3->rotateZ(90);
	l3->rotateX(90);
	Plane* l4 = new Plane("cube face", this->material);
	l4->texture = this->texture;
	l4->translate(0.5, 0, 0);
	l4->rotateZ(90);
	l4->rotateX(90);
	l4->rotateY(180);

	Plane* l5 = new Plane("cube face", this->material);
	l5->texture = this->texture;
	l5->translate(0, -0.5, 0);
	l5->rotateY(90);
	l5->rotateX(180);
	Plane* l6 = new Plane("cube face", this->material);
	l6->texture = this->texture;
	l6->translate(0, 0.5, 0);
	l6->rotateY(90);

	this->add(l1);
	this->add(l2);
	this->add(l3);
	this->add(l4);
	this->add(l5);
	this->add(l6);

	this->ancora_obj = (vec4(0.0, 0.0, 0.0, 1.0));

}