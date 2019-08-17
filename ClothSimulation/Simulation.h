#ifndef SIMULATION_H
#define SIMULATION_H

#include "Mesh.h"
#include "Scene.h"
#include "Constraint.h"
#include <unordered_set>


class Simulation
{
public:
	Simulation();
	Simulation(MyMesh* meshes, Scene* scene, float velocity=0.001);
	~Simulation();

	void InitSimualtion();
	void updateLocation(float time);
	void UpdateMass(unsigned int n, float mm);
	void CholeskyDecomp();

	MyMesh* mesh;
	Scene* scene;

	float speed;
	float h;             // s
	float h2;            // s2
	float spring_stiff;  // N/m
	float bending_stiff; // N/m
	float gravity;       // m/s2
	float cur_time;
	std::unordered_set<unsigned int> fixset;

private:
	void SetConstrain();
	void SetLeft();
	void SetRight();
	void SetForceExt();
	void CalculateInertia();
	void UpdateAux(VectorXf & pos);


private:
	// matrix related to strain constriants
	SpMat mat_strain_left;        //3m*3m, m is number of verticies
	SpMat mat_strain_right;       //3m*3s, s is number of constraints
	VectorXf mat_spring_aux;      //3s*1, s is number of constraints

	// matrix related to bending constriants
	SpMat mat_bending_left;        //3m*3m, m is number of verticies
	SpMat mat_bending_right;       //3m*3s, s is number of constraints
	VectorXf mat_bending_aux;      //3s*1, s is number of constraints

	// other common matrix
	VectorXf force_ext;    //3m*1
	VectorXf y_inertia;    //3m*1
	VectorXf new_postion;  //3m*1

	// Constraint vector
	std::vector<Constraint*> constraints;

	// Sparse Linear System Solver
	Eigen::SimplicialLDLT<SpMat> solver;
};


#endif // !SIMULATION_H

