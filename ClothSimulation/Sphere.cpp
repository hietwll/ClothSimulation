#include "Sphere.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

SphereDraw::SphereDraw()
{

}

SphereDraw::~SphereDraw()
{

}

SphereDraw::SphereDraw(glm::vec3 pos, glm::vec3 col, float r):cpos(pos),color(col),radius(r)
{
	obj_name = "sphere";

	model = glm::mat4(1.0f);
	model = glm::translate(model,cpos);
	model = glm::scale(model, glm::vec3(radius, radius, radius));

	tr_inv_model = glm::transpose(glm::inverse(model));

	shader = new Shader("Resources/shaders/sphere.vs", "Resources/shaders/sphere.fs");
}

void SphereDraw::InitBuffer()
{
	Sphere* sphere = new Sphere(1.0, 72, 18);

	/////////////////////////////////////// vao for planet/////////////////////////////////////////////////
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sphere->getInterleavedVertexSize(), sphere->getInterleavedVertices(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere->getIndexSize(), sphere->getIndices(), GL_STATIC_DRAW);

	// activate attribs
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	// set attrib arrays using glVertexAttribPointer()
	int stride = sphere->getInterleavedStride();
	glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, 0); // pos
	glVertexAttribPointer(1, 3, GL_FLOAT, false, stride, (void*)(3 * sizeof(float))); //nor
	glVertexAttribPointer(2, 2, GL_FLOAT, false, stride, (void*)(6 * sizeof(float))); //tex

	index_cnt = sphere->getIndexCount();

	delete sphere;
}


void SphereDraw::HandleCollid(VectorXf & mesh_pos)
{
	float rads = radius+0.05;

	for (int i = 0; i < mesh_pos.size()/3; i++)
	{
		float x = mesh_pos[3 * i + 0];
		float y = mesh_pos[3 * i + 1];
		float z = mesh_pos[3 * i + 2];

		if ((x-cpos[0])*(x - cpos[0]) + (y-cpos[1])*(y-cpos[1]) + (z-cpos[2])*(z - cpos[2]) < rads*rads)
		{
			glm::vec3 dir(x-cpos[0], y - cpos[1], z - cpos[2]);

			dir = glm::normalize(dir);

			mesh_pos[3 * i + 0] = (rads)*dir[0] + cpos[0];
			mesh_pos[3 * i + 1] = (rads)*dir[1] + cpos[1];
			mesh_pos[3 * i + 2] = (rads)*dir[2] + cpos[2];
		}
	}
}


void SphereDraw::Draw(glm::mat4 projection, glm::mat4 view, glm::vec3 lPos, glm::vec3 vPos, glm::vec3 lCor)
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

	glBindVertexArray(vao);

	// draw sphere
	glDrawElements(GL_TRIANGLES,  // primitive type
		index_cnt * 3,            // # of indices
		GL_UNSIGNED_INT,          // data type
		(void*)0);
}