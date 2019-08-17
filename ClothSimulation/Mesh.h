#ifndef MESH_H
#define MESH_H

#include <tiny_obj_loader.h>
#include <glad/glad.h>
#include "EigenHeader.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "DrawObj.h"

class MyMesh : public DrawObj
{
public:
	MyMesh();
	~MyMesh();

	bool LoadMesh(std::string inputfiles);
	void InitIndexList();
	void InitEdgesList();
	void InitMass();
	void SetMassMat();
	void InitPosVel();
	void InitTexture();
	void InitNormal();
	void loadTextures(std::string filename);
	void UpdateNormal();

	void InitBuffer();
	void Draw(glm::mat4 projection, glm::mat4 view, glm::vec3 lPos, glm::vec3 vPos, glm::vec3 lCor);
	void Combine32To64(uint64_t & keys, uint32_t & key1, uint32_t & key2);

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn;
	std::string err;

	unsigned int num_of_faces;
	unsigned int num_of_vertices;
	unsigned int num_of_edges;
	unsigned int vertices_dim;
	unsigned int edges_dim;

	std::vector<unsigned int> indexlist;
	std::vector<unsigned int> edgelist;
	std::vector<std::vector<unsigned int>> bendlist;
	std::vector<float> mass;
	float density;   //  kg/m^2

	// mass matrix for linear equation
	SpMat mt_M;	// 3m*3m


	// Velocity an Position
	VectorXf mesh_position; //3m*1
	VectorXf mesh_velocity; //3m*1
	VectorXf mesh_normal;   //3m*1
	VectorXf mesh_texture;  //2m*1

private:
	unsigned int vao, vbo, ibo, vbo_tex, vbo_norm;
	unsigned int texid;
	glm::mat4 model;
	glm::mat4 tr_inv_model;
};

#endif // ! MESH_H

