#include "Scene.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>


Scene::Scene(Camera* cam, std::vector<DrawObj*>& draws):mycamera(cam),drawobjs(draws)
{
	lPos = glm::vec3(-7.0, 5.0, 0.0);
	lCor = glm::vec3(1.0,1.0,1.0);
}

Scene::Scene()
{

}

Scene::~Scene()
{
	delete mycamera;
	for (auto x : drawobjs)
	{
		if (x != nullptr) { delete x; }//std::cout << x->obj_name << std::endl;
	}
}


void Scene::InitBuffer()
{
	for (auto x : drawobjs)
		x->InitBuffer();
}

void Scene::Draw()
{
	for (auto x : drawobjs)
		x->Draw(projection, view,lPos,mycamera->Position,lCor);
}

void Scene::UpdateCamera(int width, int height)
{
	mycamera->setFollowPos();
	projection = glm::perspective(glm::radians(mycamera->Zoom), (float)(width) / (float)(height), 0.1f, 100.0f);
	view = mycamera->GetViewMatrix();
	proj_view = projection * view;
}