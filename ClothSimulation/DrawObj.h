#ifndef DRAWOBJ_H
#define DRAWOBJ_H

#include <glm/glm.hpp>
#include <string>
#include "Shader.h"
#include "EigenHeader.h"

class DrawObj
{
public:
	DrawObj() {};
	virtual ~DrawObj() { if(shader != nullptr)delete shader; };
	virtual void Draw(glm::mat4 projection, glm::mat4 view, glm::vec3 lPos, glm::vec3 vPos, glm::vec3 lCor) = 0 {};
	virtual void InitBuffer() = 0 {};
	virtual void HandleCollid(VectorXf & mesh_pos) {};

	std::string obj_name;
	Shader* shader;
};


#endif // !DRAWOBJ_H

