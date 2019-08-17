#ifndef EIGENHEADER_H
#define EIGENHEADER_H

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/SparseCholesky>
#include <unsupported/Eigen/SparseExtra>

typedef Eigen::Matrix<unsigned int, Eigen::Dynamic, 1> VectorXi; // column vector of unint 
typedef Eigen::Matrix<float, Eigen::Dynamic, 1> VectorXf; // column vector of float
typedef Eigen::SparseMatrix<float> SpMat;
typedef Eigen::Triplet<float,unsigned int> Trip;

#endif // !EIGENHEADER_H

