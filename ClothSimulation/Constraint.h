#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include "EigenHeader.h"

typedef enum
{
	STRAIN,
	BENDING	
} ConstraintType;

class Constraint
{
public:
	Constraint();
	virtual ~Constraint();
	virtual void SetLeft(std::vector<Trip> & triplist);
	virtual void SetRight(std::vector<Trip> & triplist);
	virtual void CalculateAux(VectorXf & pos, VectorXf & aux);

	ConstraintType ctype;
};

class StrainConstraint : public Constraint
{
public:
	StrainConstraint();
	StrainConstraint(unsigned int ith,unsigned int idx1, unsigned int idx2, float stiff, float rest_len);
	~StrainConstraint();
	void SetLeft(std::vector<Trip> & triplist);
	void SetRight(std::vector<Trip> & triplist);
	void CalculateAux(VectorXf & pos, VectorXf & aux);

	unsigned int ith;
	unsigned int indx1;
	unsigned int indx2;
	float stiffness;
	float rest_length;
};


class BendingConstraint : public Constraint
{
public:
	BendingConstraint();
	BendingConstraint(unsigned int idx0, unsigned int idx1, unsigned int idx2, unsigned int idx3, float k0, float k1, float k2, float k3, float stiff);
	~BendingConstraint();
	void SetLeft(std::vector<Trip> & triplist);
	void SetRight(std::vector<Trip> & triplist);
	void CalculateAux(VectorXf & pos, VectorXf & aux);

	unsigned int indx[4];
	float ck[4];
	float stiffness;
};



#endif // !CONSTRAINT_H

