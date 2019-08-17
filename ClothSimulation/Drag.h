#ifndef DRAG_H
#define DRAG_H

#include <glm/glm.hpp>

class Mymesh;

void getDragPoint(glm::mat4 & mvp, MyMesh * mesh, float x, float y, glm::vec3 & pick, int & phit);

#endif