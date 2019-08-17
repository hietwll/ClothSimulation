#ifndef SKYBOX_H
#define SKYBOX_H
#include <iostream>
#include <vector>
#include <string>
#include "Shader.h"
#include "DrawObj.h"

class SkyBox: public DrawObj {

public:
	SkyBox();
	~SkyBox();

	void InitBuffer();
	void Draw(glm::mat4 projection, glm::mat4 view, glm::vec3 lPos, glm::vec3 vPos, glm::vec3 lCor);

private:
	void loadCubemap();

private:
	std::vector<std::string> faces;
	unsigned int textureID;
	unsigned int vao, vbo, ibo;
	glm::mat4 model;
};

#endif // !SKYBOX_H

