#include "Constraint.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>


Constraint::Constraint()
{
}

Constraint::~Constraint()
{
}

void Constraint::SetLeft(std::vector<Trip> & triplist)
{

}

void Constraint::SetRight(std::vector<Trip> & triplist)
{
}

void Constraint::CalculateAux(VectorXf & pos, VectorXf & aux)
{

}


StrainConstraint::StrainConstraint()
{

}

StrainConstraint::StrainConstraint(unsigned int iths, unsigned int idx1, unsigned int idx2, float stiff, float rest_len) : ith(iths),
indx1(idx1),indx2(idx2), stiffness(stiff), rest_length(rest_len)
{
	ctype = STRAIN;
}

StrainConstraint::~StrainConstraint()
{

}

void StrainConstraint::SetLeft(std::vector<Trip> & triplist)
{
	// push back 4 locations for each spring constrain: (indx1,indx1,k),(indx2,indx2,k),(indx1,indx2,-k),(indx1,indx2,-k)
	// for each location, the trip is copied three times due to Kronecker product

	triplist.push_back(Trip(3 * indx1+0, 3 * indx1+0, stiffness));
	triplist.push_back(Trip(3 * indx1+1, 3 * indx1+1, stiffness));
	triplist.push_back(Trip(3 * indx1+2, 3 * indx1+2, stiffness));

	triplist.push_back(Trip(3 * indx2 + 0, 3 * indx2 + 0, stiffness));
	triplist.push_back(Trip(3 * indx2 + 1, 3 * indx2 + 1, stiffness));
	triplist.push_back(Trip(3 * indx2 + 2, 3 * indx2 + 2, stiffness));

	triplist.push_back(Trip(3 * indx1 + 0, 3 * indx2 + 0, -stiffness));
	triplist.push_back(Trip(3 * indx1 + 1, 3 * indx2 + 1, -stiffness));
	triplist.push_back(Trip(3 * indx1 + 2, 3 * indx2 + 2, -stiffness));

	triplist.push_back(Trip(3 * indx2 + 0, 3 * indx1 + 0, -stiffness));
	triplist.push_back(Trip(3 * indx2 + 1, 3 * indx1 + 1, -stiffness));
	triplist.push_back(Trip(3 * indx2 + 2, 3 * indx1 + 2, -stiffness));
}

void StrainConstraint::SetRight(std::vector<Trip> & triplist)
{
	// push back 2 locations for each spring constrain: (indx1,ith,-k),(indx2,ith,k)
	// for each location, the trip is copied three times due to Kronecker product

	triplist.push_back(Trip(3 * indx1 + 0, 3 * ith + 0, -stiffness));
	triplist.push_back(Trip(3 * indx1 + 1, 3 * ith + 1, -stiffness));
	triplist.push_back(Trip(3 * indx1 + 2, 3 * ith + 2, -stiffness));

	triplist.push_back(Trip(3 * indx2 + 0, 3 * ith + 0, stiffness));
	triplist.push_back(Trip(3 * indx2 + 1, 3 * ith + 1, stiffness));
	triplist.push_back(Trip(3 * indx2 + 2, 3 * ith + 2, stiffness));

}

void StrainConstraint::CalculateAux(VectorXf & pos, VectorXf & aux)
{
	glm::vec3 p1, p2;

	p1 = glm::vec3(pos[3 * indx1 + 0], pos[3 * indx1 + 1], pos[3 * indx1 + 2]);
	p2 = glm::vec3(pos[3 * indx2 + 0], pos[3 * indx2 + 1], pos[3 * indx2 + 2]);

	p2 = glm::normalize(p2 - p1);
	
	aux[3 * ith + 0] = rest_length*p2[0];
	aux[3 * ith + 1] = rest_length*p2[1];
	aux[3 * ith + 2] = rest_length*p2[2];

}


BendingConstraint::BendingConstraint()
{

}

BendingConstraint::BendingConstraint(unsigned int idx0, unsigned int idx1, 
	unsigned int idx2, unsigned int idx3, float k0, float k1, float k2, float k3, float stiff)
{
	indx[0] = idx0; indx[1] = idx1; indx[2] = idx2; indx[3] = idx3;
	ck[0] = k0; ck[1] = k1; ck[2] = k2; ck[3] = k3;

	stiffness = stiff;
	ctype = BENDING;
}

BendingConstraint::~BendingConstraint()
{

}

void BendingConstraint::SetLeft(std::vector<Trip> & triplist)
{
	// push back 16 locations for each bending constrain:
	//(indx0,indx0,ck0*ck0) (indx0,indx0,ck0*ck0) (indx0,indx0,ck0*ck0) (indx0,indx0,ck0*ck0)
	// ...
	// for each location, the trip is copied three times due to Kronecker product

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			triplist.push_back(Trip(3 * indx[i] + 0, 3 * indx[j] + 0, ck[i] * ck[j] * stiffness));
			triplist.push_back(Trip(3 * indx[i] + 1, 3 * indx[j] + 1, ck[i] * ck[j] * stiffness));
			triplist.push_back(Trip(3 * indx[i] + 2, 3 * indx[j] + 2, ck[i] * ck[j] * stiffness));
		}
}

void BendingConstraint::SetRight(std::vector<Trip> & triplist)
{


}

void BendingConstraint::CalculateAux(VectorXf & pos, VectorXf & aux)
{

}