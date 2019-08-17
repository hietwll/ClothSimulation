#include "Ray.h"


Ray::Ray()
	:tMax_(std::numeric_limits<float>::infinity())
{
}

Ray::Ray(const glm::vec3 & _o, const glm::vec3 & _d, float _tMax)
	: o_(_o), d_(glm::normalize(_d)), tMax_(_tMax)
{

}


Ray::~Ray()
{
}

void Ray::initRay()
{
	computePermute();
	computeSheer();
	for (int i = 0; i < 3; i++)
	{
		inv_d_[i] = 1.0f / d_[i];
	}
}

void Ray::computePermute()
{
	glm::mat3 p(0.0f);
	const auto ab = glm::abs(d_);
	kz = 0;
	if (ab.x > ab.y)
	{
		if (ab.z > ab.x)
			kz = 2;
		else
			kz = 0;
	}
	else
	{
		if (ab.z > ab.y)
			kz = 2;
		else
			kz = 1;
	}
	kx = (kz + 1) % 3;
	ky = (kx + 1) % 3;
	p[0][kx] = p[1][ky] = p[2][kz] = 1.0f;

	permute2z_ = p;

}

void Ray::computeSheer()
{
	glm::vec3 rd = Permute2Z(d_);
	sx = -rd.x / rd.z;
	sy = -rd.y / rd.z;
	sz = 1.f / rd.z;
}