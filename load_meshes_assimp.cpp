// Include AssImp
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include "Figures.h"

bool loadAssImp(const char* path, vector<ObjMesh> & mymesh)
 {

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(path, aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_FlipUVs);
 	if (!scene) {
		fprintf(stderr, importer.GetErrorString());
		getchar();
		return false;
	}
	const aiMesh* mesh; 

	// Fill vertices positions
	int num_meshes= scene->mNumMeshes;  //Numero di oggetti che compongono il modello
	mymesh.resize(num_meshes);  //Faccio il resize di mymesh al numero di mesh che compongono l'oggetto

	for (unsigned int nm=0; nm<num_meshes;nm++)
	{
		//Per ogni mesh dell'oggetto 
		mesh  = scene->mMeshes[nm];
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex]; 

		aiColor3D color;
		float value;
		// Read mtl file vertex data
		
		if (aiReturn_SUCCESS == material->Get(AI_MATKEY_COLOR_AMBIENT, color)) 
		{
				mymesh[nm].material->ambient = glm::vec3(color.r, color.g, color.b);
		}
		else
		{
			printf("Errore in ambientale \n");
		}
		
		if (aiReturn_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, color))
		{
				mymesh[nm].material->diffuse = glm::vec3(color.r, color.g, color.b);
		}
		else
		{
				mymesh[nm].material->diffuse = glm::vec3(1.0,0.2,0.1);
		}
		
		if (aiReturn_SUCCESS == material->Get(AI_MATKEY_COLOR_SPECULAR, color))
		{
				mymesh[nm].material->specular = glm::vec3(color.r, color.g, color.b);
		}
		
		else
		{
			printf("Errore in specular \n");
			mymesh[nm].material->specular = glm::vec3(0.5,0.5,0.5);
		}
		if (aiReturn_SUCCESS == material->Get(AI_MATKEY_SHININESS_STRENGTH, value))
		{
				mymesh[nm].material->shininess = value;
		}
		else
		{
			//printf("Errore in shininess \n");
			mymesh[nm].material->shininess = 50.0f;
			 
		}
		
		// http://assimp.sourceforge.net/lib_html/structai_material.html

		 
		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		
			aiVector3D pos = mesh->mVertices[i];
			mymesh[nm].vertici.push_back(glm::vec3(pos.x, pos.y, pos.z));
		
		}
	 
	// Fill vertices texture coordinates
	 
		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			mymesh[nm].texCoords.push_back(glm::vec2(0.0,0.0));
			mymesh[nm].colori.push_back(vec4(1.0, 0.0, 0.0,1.0));
		 
		
		}

	// Fill vertices normals
	 
		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			aiVector3D n = mesh->mNormals[i];
			mymesh[nm].normali.push_back(glm::vec3(n.x, n.y, n.z));
		}


	// Fill face indices
	 
		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			// Assume the model has only triangles.
			mymesh[nm].indici.push_back(mesh->mFaces[i].mIndices[0]);
			mymesh[nm].indici.push_back(mesh->mFaces[i].mIndices[1]);
			mymesh[nm].indici.push_back(mesh->mFaces[i].mIndices[2]);
		}

	}
	return true;
}
