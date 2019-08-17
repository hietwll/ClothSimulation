#ifndef RAY_H
#define RAY_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Ray
{
public:
	Ray();
	Ray(const glm::vec3 &_o, const glm::vec3 & _d, float _tMax = std::numeric_limits<float>::infinity());
	~Ray();
	void initRay();

	glm::vec3 Permute2Z(const glm::vec3 &v) const { return glm::vec3(v[kx], v[ky], v[kz]); }

	glm::vec3 o_;
	glm::vec3 d_;
	glm::mat3 permute2z_;
	mutable float tMax_;

	// Sheer
	float sx, sy, sz;
	glm::vec3 inv_d_;
private:
	void computePermute();
	void computeSheer();
	// Permute
	int kx, ky, kz;

};

#endif // !RAY_H

