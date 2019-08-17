#ifndef SCENE_H
#define SCENE_H

#include "DrawObj.h"
#include "Camera.h"
#include <vector>

class Scene
{
public:
	Scene();
	Scene(Camera* cam, std::vector<DrawObj*>& draws);
	~Scene();
	void InitBuffer();
	void Draw();
	void UpdateCamera(int width, int height);

public:
	Camera* mycamera;
	std::vector<DrawObj*> drawobjs;

	glm::mat4 projection;
	glm::mat4 view;
	glm::mat4 proj_view;
	glm::mat4 proj_view_inv;

	glm::vec3 lPos;
	glm::vec3 lCor;
};

#endif // !SCENE_H

