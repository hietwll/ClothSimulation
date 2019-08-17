#include "Simulation.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include<windows.h>
#include<iostream>


bool IsEdge(float x)
{
	float limit = 1e-6;
	return ((x - 1.0) < limit && (x - 1.0) > -limit) || ((x + 1.0) < limit && (x + 1.0) > -limit);
}

Simulation::Simulation()
{

}

Simulation::Simulation(MyMesh* meshes, Scene* scenes, float velocity)
{
	mesh = meshes;
	scene = scenes;
	speed = velocity;
	spring_stiff = 2000.0; // N/m
	bending_stiff = 0.1; // N/m
	gravity = -0.5;
	h = (1.0 / 30.0);
	h2 = h * h;
	cur_time = 0.0;
}

Simulation::~Simulation()
{

}

void Simulation::InitSimualtion()
{
	y_inertia.resize(mesh->vertices_dim);
	new_postion.resize(mesh->vertices_dim);
	mat_spring_aux.resize(mesh->edges_dim);

	y_inertia.setZero();
	new_postion.setZero();
	mat_spring_aux.setZero();

	SetConstrain();
	SetForceExt();
	SetLeft();
	SetRight();

	CholeskyDecomp();
}

void Simulation::CholeskyDecomp()
{
	solver.compute(mesh->mt_M + h2 * mat_strain_left);
}

void Simulation::SetConstrain()
{
	// Set Stain Constain
	float rest_len;
	glm::vec3 p0, p1, p2, p3;
	glm::vec3 e0, e1, e2, e3, e4;
	unsigned int idx0, idx1, idx2, idx3;
	float c01, c02, c03, c04;
	float area0, area1;

	for (unsigned int n = 0; n < mesh->num_of_edges; n++)
	{
		idx1 = mesh->edgelist[2*n];
		idx2 = mesh->edgelist[2*n+1];

		p1 = glm::vec3(mesh->mesh_position[3 * idx1], mesh->mesh_position[3 * idx1 + 1], mesh->mesh_position[3 * idx1 + 2]);
		p2 = glm::vec3(mesh->mesh_position[3 * idx2], mesh->mesh_position[3 * idx2 + 1], mesh->mesh_position[3 * idx2 + 2]);

		constraints.push_back(new StrainConstraint(n, idx1, idx2, spring_stiff, glm::length(p1 - p2)));
	}


	// Set Bending Constraint
	for (auto bend : mesh->bendlist)
	{
		idx0 = bend[0];
		idx1 = bend[1];
		idx2 = bend[2];
		idx3 = bend[3];

		p0 = glm::vec3(mesh->mesh_position[3 * idx0], mesh->mesh_position[3 * idx0 + 1], mesh->mesh_position[3 * idx0 + 2]);
		p1 = glm::vec3(mesh->mesh_position[3 * idx1], mesh->mesh_position[3 * idx1 + 1], mesh->mesh_position[3 * idx1 + 2]);
		p2 = glm::vec3(mesh->mesh_position[3 * idx2], mesh->mesh_position[3 * idx2 + 1], mesh->mesh_position[3 * idx2 + 2]);
		p3 = glm::vec3(mesh->mesh_position[3 * idx3], mesh->mesh_position[3 * idx3 + 1], mesh->mesh_position[3 * idx3 + 2]);

		e0 = p1 - p0;
		e1 = p2 - p0;
		e2 = p3 - p0;
		e3 = p2 - p1;
		e4 = p3 - p1;

		area0 = 0.5*glm::length(glm::cross(e1, e0));
		area1 = 0.5*glm::length(glm::cross(e2, e0));

		c01 = glm::dot(e1, e0) / glm::length(glm::cross(e1, e0));
		c02 = glm::dot(e2, e0) / glm::length(glm::cross(e2, e0));
		c03 = glm::dot(e3, -e0) / glm::length(glm::cross(e3, -e0));
		c04 = glm::dot(e4, -e0) / glm::length(glm::cross(e4, -e0));

		constraints.push_back(new BendingConstraint(idx0,idx1,idx2,idx3,c03+c04,c01+c02,-c01-c03,-c02-c04,3.0/(area0+area1)*bending_stiff));

	}


	// fixpoints [1,3,4,6]
	fixset.insert(4);
	fixset.insert(6);

	// Set fix ponits
	for (auto n : fixset)
		UpdateMass(n,5000.0);
}

void Simulation::UpdateMass(unsigned int n, float mm)
{
	for (unsigned int j = 0; j < 3; j++)
	{
		mesh->mt_M.coeffRef(3 * n + j, 3 * n + j) = mm;
	}
}

void Simulation::SetLeft()
{
	std::vector<Trip> triplist;
	mat_strain_left.resize(mesh->vertices_dim,mesh->vertices_dim);
	mat_strain_left.setZero();

	for (auto con : constraints)
	{
		//if (con->ctype == STRAIN)
		{
			con->SetLeft(triplist);
		}
	}

	mat_strain_left.setFromTriplets(triplist.begin(), triplist.end());

	bool s2 = Eigen::saveMarket(mat_strain_left, "mat_strain_left.txt");

}

void Simulation::SetRight()
{
	std::vector<Trip> triplist;
	mat_strain_right.resize(mesh->vertices_dim, mesh->edges_dim);  // 3m * 3s
	mat_strain_right.setZero();

	for (auto con : constraints)
	{
		if (con->ctype == STRAIN)
		{
			con->SetRight(triplist);
		}
	}

	mat_strain_right.setFromTriplets(triplist.begin(), triplist.end());

	mat_strain_right.makeCompressed();

	//mat_strain_right.setZero();

	//bool s = Eigen::saveMarket(mat_strain_right, "mat_strain_right2.txt");
}

void Simulation::SetForceExt()
{
	force_ext.resize(mesh->vertices_dim);
	force_ext.setZero();

	for (unsigned int n = 0; n < mesh->num_of_vertices; n++)
	{
		force_ext[3 * n + 1] = mesh->mass[n]*gravity;
	}

}

void Simulation::CalculateInertia()
{
	y_inertia = mesh->mesh_position + h * mesh->mesh_velocity;
}

void Simulation::UpdateAux(VectorXf & pos)
{
	mat_spring_aux.setZero();

	for (auto con : constraints)
	{
		if (con->ctype == STRAIN)
		{
			con->CalculateAux(pos, mat_spring_aux);
		}
	}

}

void Simulation::updateLocation(float time)
{
	cur_time += h; 

	printf("Curent time is : %f \n", cur_time);

	CalculateInertia();

	new_postion = mesh->mesh_position;

	for (int k = 0; k < 20; k++)
	{
		UpdateAux(new_postion);
		new_postion = solver.solve(h2*mat_strain_right*mat_spring_aux + mesh->mt_M*y_inertia + h2 * force_ext);

		// naive fixpoint
		for (auto n : fixset)
		{
			new_postion[3 * n + 0] = mesh->mesh_position[3 * n + 0];
			new_postion[3 * n + 1] = mesh->mesh_position[3 * n + 1];
			new_postion[3 * n + 2] = mesh->mesh_position[3 * n + 2];
		}

	     // naive collide
		for (auto x : scene->drawobjs)
		{
			x->HandleCollid(new_postion);
		}

	}

	mesh->mesh_velocity = 0.95*(new_postion - mesh->mesh_position) / h;

	mesh->mesh_position += (mesh->mesh_velocity)*h;

	mesh->UpdateNormal();


	//Sleep(100);
}
