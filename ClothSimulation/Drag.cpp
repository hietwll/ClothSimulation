#include "Ray.h"
#include "Mesh.h"
#include <vector>
#include <chrono>

bool Intersect(const Ray & r, const glm::vec3(&p)[3], glm::vec3 & out_barycentric, float & out_thit)
{
	// move Ray to (0,0,0)
	glm::vec3 p0t = p[0] - r.o_;
	glm::vec3 p1t = p[1] - r.o_;
	glm::vec3 p2t = p[2] - r.o_;

	//permute
	glm::vec3 rd = r.Permute2Z(r.d_);
	p0t = r.Permute2Z(p0t);
	p1t = r.Permute2Z(p1t);
	p2t = r.Permute2Z(p2t);

	// shear to Z direction
	// 
	p0t.x += r.sx * p0t.z;
	p0t.y += r.sy * p0t.z;
	p1t.x += r.sx * p1t.z;
	p1t.y += r.sy * p1t.z;
	p2t.x += r.sx * p2t.z;
	p2t.y += r.sy * p2t.z;

	// find intersect on (0,0,z)
	// page 161
	// Barycentric test on (0,0)

	float b01 = p0t.x * p1t.y - p1t.x * p0t.y;
	float b12 = p1t.x * p2t.y - p2t.x * p1t.y;
	float b20 = p2t.x * p0t.y - p0t.x * p2t.y;

	// TODO: double ver
	if (b01 == 0.0f || b12 == 0.0f || b20 == 0.0f)
	{

	}
	// Barycentric test failed (not all same side)
	if ((b01 < 0.0f || b12 < 0.0f || b20 < 0.0f) && (b01 > 0.0f || b12 > 0.0f || b20 > 0.0f))
		return false;

	// all zero, on the edge for all three edges
	float bsum = b01 + b12 + b20;
	if (bsum == 0.0f)
		return false;
	// b0 + b1 + b2  = 1; 
	// all >= 0
	float b0 = b12 / bsum;
	float b1 = b20 / bsum;
	float b2 = b01 / bsum;
	// shear z
	p0t.z *= r.sz;
	p1t.z *= r.sz;
	p2t.z *= r.sz;
	// distance
	float dis = p0t.z * b0 + p1t.z * b1 + p2t.z * b2;
	if (dis < 0 || dis > r.tMax_)
		return false;
	out_barycentric = glm::vec3(b0, b1, b2);
	out_thit = dis;
	return true;
}



void getDragPoint(glm::mat4 & mvp, MyMesh * mesh, float x, float y, glm::vec3 & pick, int & phit)
{
	phit = -1;

	static std::vector<glm::vec3> vrt;
	if (mesh == nullptr)
		return;

	int vn = mesh->num_of_vertices;
	vrt.resize(vn);
	float * v = mesh->mesh_position.data();

	std::cout << std::endl;

	std::cout << " x = "<< x << " y = " << y << std::endl;


	if (vn == 0)
		return;

	for (int cv = 0; cv < vn; cv++)
	{
		glm::vec4 vp(
			v[cv * 3],
			v[cv * 3 + 1],
			v[cv * 3 + 2],
			1);
		glm::vec4  temp = mvp * vp;;
		vrt[cv] = glm::vec3(temp.x / temp.w, temp.y / temp.w, temp.z / temp.w);

	}

	auto bg = std::chrono::high_resolution_clock::now();
	int trihit = -1;

	float tHit;
	Ray r(glm::vec3(x, -y, 100.0f), glm::vec3(0.0f, 0.0f, -1.0f));

	r.initRay();
	const auto & t = mesh->indexlist;
	glm::vec3 bec;
	for (int ct = 0; ct < t.size() / 3; ct++)
	{
		int t1 = t[ct * 3];
		int t2 = t[ct * 3 + 1];
		int t3 = t[ct * 3 + 2];
		glm::vec3 p[3];
		p[0] = vrt[t1];
		p[1] = vrt[t2];
		p[2] = vrt[t3];
		if (Intersect(r, p, bec, tHit) == true)
		{
			r.tMax_ = tHit;
			trihit = ct;
		}
	}

	// get which point is selected
	if (trihit >= 0)
	{
		if (bec[0] >= bec[1] && bec[0] >= bec[2]) phit = t[trihit * 3 + 0];
		if (bec[1] >= bec[0] && bec[1] >= bec[2]) phit = t[trihit * 3 + 1];
		if (bec[2] >= bec[1] && bec[2] >= bec[0]) phit = t[trihit * 3 + 2];

		pick = vrt[phit];
	}

	//using namespace std::chrono_literals;
	//std::this_thread::sleep_for(5s);
	//auto ed = std::chrono::high_resolution_clock::now();
	//std::chrono::duration<double> d2 = (ed - bg);
	//std::cout << "scalar  " << d2.count() * 1000 << "ms" << std::endl;
	std::cout << "triggle clicked£º   " << trihit <<" point clicked:  "<<phit<< std::endl;
}

