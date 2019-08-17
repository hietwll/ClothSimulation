#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc

#include "Mesh.h"
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <iostream>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif // !STB_IMAGE_IMPLEMENTATION



MyMesh::MyMesh(){
	obj_name = "triggle_mesh";

	// shader init
	shader = new Shader("Resources/shaders/cloth.vs", "Resources/shaders/cloth.fs");
	bool ret = LoadMesh("Resources/obj/simple_plate.obj");

	model = glm::mat4(1.0f);
	tr_inv_model = glm::transpose(glm::inverse(model));
}

MyMesh::~MyMesh()
{

}

bool MyMesh::LoadMesh(std::string inputfile)
{
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str());

	if (ret)
	{
		density = 0.01;
		num_of_faces = shapes[0].mesh.indices.size()/3; // assume all triangle meshes
		num_of_vertices = attrib.vertices.size()/3;
		vertices_dim = num_of_vertices * 3;

		indexlist.resize(num_of_faces*3);
		mass.resize(num_of_vertices);
		mt_M.resize(vertices_dim, vertices_dim);
		mesh_texture.resize(2 * num_of_vertices);

		InitIndexList();
		InitEdgesList();
		InitMass();
		SetMassMat();
		InitPosVel();
		InitTexture();
		InitNormal();
	}

	return ret;
}

void MyMesh::InitNormal()
{
	mesh_normal.resize(vertices_dim);
	mesh_normal.setZero();

	for (unsigned int n = 0; n < num_of_vertices; n++)
	{
		mesh_normal[3 * n + 0] = 0.0;
		mesh_normal[3 * n + 1] = 1.0;
		mesh_normal[3 * n + 2] = 0.0;
	}
}


void MyMesh::UpdateNormal()
{
	unsigned int id[3];

	glm::vec3 pt[3];

	mesh_normal.setZero();

	for (unsigned int n = 0; n < num_of_faces; n++)
	{
		for (unsigned int i = 0; i < 3; i++)
		{
			id[i] = indexlist[3 * n + i];

			for (unsigned int j = 0; j < 3; j++)
			{
				pt[i][j] = mesh_position[3 * id[i] + j];
			}
		}

		glm::vec3 glm_normal = glm::normalize(glm::cross(pt[1] - pt[0], pt[2] - pt[1]));

		for (unsigned int i = 0; i < 3; i++)
		{
			for (unsigned int j = 0; j < 3; j++)
			{
				mesh_normal[3 * id[i] + j] += glm_normal[j];
			}
		}
	}

	// renormalize all verticies
	for (unsigned int n = 0; n < num_of_vertices; n++)
	{
		glm::vec3 norm = glm::normalize(glm::vec3(mesh_normal[3 * n + 0], 
			mesh_normal[3 * n + 1], mesh_normal[3 * n + 2]));
		
		mesh_normal[3 * n + 0] = -norm[0];
		mesh_normal[3 * n + 1] = -norm[1];
		mesh_normal[3 * n + 2] = -norm[2];

	}
}



void MyMesh::InitTexture()
{
	mesh_texture.setZero();

	for (unsigned int n = 0; n < num_of_vertices; n++)
	{
		mesh_texture[2 * n + 0] = (mesh_position[3 * n + 0] + 1.0) / 2.0;
		mesh_texture[2 * n + 1] = (mesh_position[3 * n + 2] + 1.0) / 2.0;
	}
}


void MyMesh::InitPosVel()
{
	mesh_position.resize(vertices_dim);
	mesh_velocity.resize(vertices_dim);

	mesh_position.setZero();
	mesh_velocity.setZero();

	for (unsigned int n = 0; n < vertices_dim; n++)
	{
		mesh_position[n] = attrib.vertices[n];
	 }

	for (unsigned int n = 0; n < num_of_vertices; n++)
	{
		mesh_position[3 * n + 2] = mesh_position[3 * n + 1];
		mesh_position[3 * n + 1] = 0.0;
	}
}

void MyMesh::InitIndexList()
{
	for (unsigned int n = 0; n < num_of_faces*3; n++)
	{
		indexlist[n] = shapes[0].mesh.indices[n].vertex_index;
	}
}

void MyMesh::InitBuffer()
{
	///////////////////////////////////////// vao for cloth/////////////////////////////////////////////////
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &vbo_norm);
	glGenBuffers(1, &vbo_tex);
	glGenBuffers(1, &ibo);


	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, num_of_vertices * 3 * sizeof(float), mesh_position.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_norm);
	glBufferData(GL_ARRAY_BUFFER, num_of_vertices * 3 * sizeof(float), mesh_normal.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_tex);
	glBufferData(GL_ARRAY_BUFFER, num_of_vertices * 2 * sizeof(float), mesh_texture.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_of_faces * 3 * sizeof(unsigned int), &(indexlist[0]), GL_STATIC_DRAW);

	// activate attribs
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	// set attrib arrays using glVertexAttribPointer()
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	int stride = 3 * sizeof(float);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_norm);
	stride = 3 * sizeof(float);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, stride, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_tex);
	stride = 2 * sizeof(float);
	glVertexAttribPointer(2, 2, GL_FLOAT, false, stride, 0);


	loadTextures("Resources/images/textures/cloth2.jpg");

	// use texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texid);

	shader->use();
	shader->setInt("texture1", 0);
}


void MyMesh::loadTextures(std::string filename) {

	int width, height, nrChannels;
	
	// Create texture
	glGenTextures(1, &texid);
	glBindTexture(GL_TEXTURE_2D, texid);

	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Cubemap texture failed to load at path: " << filename << std::endl;
		stbi_image_free(data);
	}
}


void MyMesh::Draw(glm::mat4 projection, glm::mat4 view, glm::vec3 lPos, glm::vec3 vPos, glm::vec3 lCor)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	shader->use();

	shader->setMat4("projection", projection);

	shader->setMat4("view", view);

	shader->setMat4("model", model);

	shader->setMat4("tr_inv_model", tr_inv_model);

	shader->setVec3("lightPos", lPos);
	shader->setVec3("viewPos", vPos);
	shader->setVec3("lightColor", lCor);
	shader->setVec3("objectColor", glm::vec3(1.0, 0.0, 0.0));

	// dynamic update
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, num_of_vertices * 3 * sizeof(float), mesh_position.data());

	// dynamic update
	glBindBuffer(GL_ARRAY_BUFFER, vbo_norm);
	glBufferSubData(GL_ARRAY_BUFFER, 0, num_of_vertices * 3 * sizeof(float), mesh_normal.data());

	glBindVertexArray(vao);

	// draw mesh
	glDrawElements(GL_TRIANGLES,  // primitive type
		num_of_faces * 3, // # of indices
		GL_UNSIGNED_INT,          // data type
		(void*)0);
}


void MyMesh::InitEdgesList()
{
	std::unordered_map<uint64_t, unsigned int> maps;
	uint64_t keys;
	uint32_t key1, key2;
	uint32_t idx1, idx2;

	for (unsigned int n = 0; n < num_of_faces; n++)
	{
		for (int j = 0; j < 3; j++) {
			// loop for three egdes of one face
			if (j == 0)
			{
				idx1 = 0;
				idx2 = 1;
			}
			else if (j == 1)
			{
				idx1 = 0;
				idx2 = 2;
			}
			else
			{
				idx1 = 1;
				idx2 = 2;
			}

			// begin judge repeat edges
			if (indexlist[3*n + idx1] > indexlist[3*n + idx2])
			{
				key1 = indexlist[3*n + idx1];
				key2 = indexlist[3*n + idx2];
			}
			else if (indexlist[3*n + idx1] < indexlist[3*n + idx2])
			{
				key1 = indexlist[3*n + idx2];
				key2 = indexlist[3*n + idx1];
			}
			else if(indexlist[3*n + idx1] == indexlist[3*n + idx2])
			{
				std::cout << "A edge can not have the same point at two side !!!" << std::endl;
				exit;
			}
	

			Combine32To64(keys, key1, key2);

			if (maps.find(keys) == maps.end())
			{
				maps.insert(std::make_pair(keys,n));
				edgelist.push_back(key1);
				edgelist.push_back(key2);
			}
			else
			{
				unsigned int m = maps[keys];  // index of the second face
				std::vector<unsigned int> vec;
				vec.resize(4);

				vec[0] = key1;
				vec[1] = key2;

				vec[2] = indexlist[3 * n + 0] + indexlist[3 * n + 1] + indexlist[3 * n + 2] - vec[0] - vec[1];
				vec[3] = indexlist[3 * m + 0] + indexlist[3 * m + 1] + indexlist[3 * m + 2] - vec[0] - vec[1];

				bendlist.push_back(vec);
			}
		}
	}

	//for (auto eid : edgelist)
	//{
	//	cout
	//}

	num_of_edges = edgelist.size() / 2;
	edges_dim = 3 * num_of_edges;

	printf("Num of surfaces: %i \n", num_of_faces);
	printf("Num of vertices: %i \n", num_of_vertices);
	printf("Num of edges: %i \n", num_of_edges);
	printf("Num of bending edges: %i \n", bendlist.size());
}


void MyMesh::InitMass()
{
	// three verticies and two edges
	std::vector<glm::vec3> vt(3);
	float area;

	// init zero for sum
	for (auto & m : mass) m = 0.1;

	// loop over faces
	for (unsigned int n = 0; n < num_of_faces; n++)
	{
		for (unsigned int j = 0; j < 3; j++)
		{
			vt[j][0] = attrib.vertices[3 * indexlist[3*n+j] + 0];
			vt[j][1] = attrib.vertices[3 * indexlist[3*n+j] + 1];
			vt[j][2] = attrib.vertices[3 * indexlist[3*n+j] + 2];
		}

		area = 0.5*density*glm::length(glm::cross(vt[1] - vt[0], vt[2] - vt[0]));
		area = 1.0 / 3.0*area;

		mass[indexlist[3*n]] += area;
		mass[indexlist[3*n+1]] += area;
		mass[indexlist[3*n+2]] += area;
	}
}

void MyMesh::SetMassMat()
{
	//mt_M.setZero();
	//mt_M.reserve(VectorXi::Constant(vertices_dim, 1)); // estimated number of non-zero enties per column

	for (unsigned int n = 0; n < num_of_vertices; n++)
	{
		for (unsigned int j = 0; j < 3; j++)
		{
			mt_M.coeffRef(3*n + j, 3*n + j) = mass[n];
		}
	}

	mt_M.makeCompressed();

	//bool s2 = Eigen::saveMarket(mt_M, "mass.txt");
}

void MyMesh::Combine32To64(uint64_t & keys, uint32_t & key1, uint32_t & key2)
{
	keys = key1;
	keys = (keys << 32);
	keys |= key2;
}





