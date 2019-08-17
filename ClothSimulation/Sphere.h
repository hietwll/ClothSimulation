#ifndef SPHEREDRAW_H
#define SPHEREDRAW_H

#include "SphereGeom.h"
#include "DrawObj.h"


class SphereDraw: public DrawObj 
{

public:
	SphereDraw(glm::vec3 pos, glm::vec3 col, float r);
	SphereDraw();
	~SphereDraw();

	void InitBuffer();
	void Draw(glm::mat4 projection, glm::mat4 view, glm::vec3 lPos, glm::vec3 vPos, glm::vec3 lCor);
	void HandleCollid(VectorXf & mesh_pos);

private:
	glm::vec3 color,cpos;
	float radius;
	unsigned int vao, vbo, ibo;
	glm::mat4 model;
	glm::mat4 tr_inv_model;
	int index_cnt;
};

#endif // ! SPHEREDRAW_H
