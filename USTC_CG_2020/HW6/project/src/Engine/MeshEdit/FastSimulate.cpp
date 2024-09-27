#include <Engine/MeshEdit/FastSimulate.h>
#include <Eigen/Sparse>
#define DEBUG 0

using namespace Ubpa;
using namespace Eigen;
using namespace std;

FastSimulate::FastSimulate(const std::vector<pointf3>& plist, const std::vector<unsigned>& elist)
	:Simulate(plist, elist)
{
	this->n_notfix = 0;
	//Init();
}
void FastSimulate::Clear()
{
}

bool FastSimulate::Run()
{
	FastSimulateOnce(3); // set iteration time be 3

	return true;
}

void FastSimulate::InitConstant()
{
	// set Gravity matrix and mass Matrix M 
	Gravity = MatrixXd(nV, 3);
	Gravity.setZero();
	M = SparseMatrix<double>(nV, nV);
	std::vector<Eigen::Triplet<double> > M_coeff;

	for (int i = 0; i < nV; i++)
	{
		M_coeff.push_back(Eigen::Triplet<double>(i, i, this->mass[i]));
		if (isFixed[i] == false)
		{
			Gravity(i, 1) = -9.8 * mass[i];
			//Gravity(i, 2) = 9.8 * mass[i]; // add another direction force 
		}
		else
		{
			Gravity(i, 1) = 0.0; // if fixed, set its gravity be zero 
		}
	}
	M.setFromTriplets(M_coeff.begin(), M_coeff.end());

	if (DEBUG) cout << "M:" << endl << MatrixXd(M) << endl << endl;
	if (DEBUG) cout << "Gravity:" << endl << Gravity << endl << endl;

	// set stiffness matrix 
	mat_stiff = SparseMatrix<double>(nE, nE);
	std::vector<Eigen::Triplet<double> > mat_stiff_coeff;
	for (int i = 0; i < nE; i++)
	{
		mat_stiff_coeff.push_back(Eigen::Triplet<double>(i, i, this->stringStiff[i]));
	}
	mat_stiff.setFromTriplets(mat_stiff_coeff.begin(), mat_stiff_coeff.end());
	if (DEBUG) cout << "stiffness:" << endl << MatrixXd(mat_stiff) << endl << endl;
}

void FastSimulate::InitXf()
{
	this->n_notfix = nV - this->fixed_id.size();

	// set initial Xn, Xn_, K, b
	Xn = MatrixXd(nV, 3);
	Xn_ = MatrixXd(nV, 3);

	Xn_.setZero();

	K = SparseMatrix<double>(n_notfix, nV);
	std::vector<Eigen::Triplet<double> > K_coeff;

	int j = 0;
	for (int i = 0; i < nV; i++)
	{
		// set Xn initially be origin X 
		Xn(i, 0) = positions[i][0];
		Xn(i, 1) = positions[i][1];
		Xn(i, 2) = positions[i][2];

		if (this->isFixed[i] == false)
		{
			K_coeff.push_back(Eigen::Triplet<double>(j, i, 1));
			j++;
		}
	}
	K.setFromTriplets(K_coeff.begin(), K_coeff.end());
	if (DEBUG) cout << "K:" << endl << MatrixXd(K) << endl << endl;

	Xf = K * Xn;
	if (DEBUG) cout << "Xf:" << endl << Xf << endl << endl;

	b = MatrixXd(nV, 3);
	b = Xn - K.transpose() * K * Xn;
	if (DEBUG) cout << "b:" << endl << b << endl << endl;

	newX = MatrixXd(n_notfix, 3);

	if (DEBUG) cout << "X0:" << endl << MatrixXd(Xn) << endl << endl;

}

void FastSimulate::Init()
{
	Clear();
	D = MatrixXd(nE, 3); // D is (nE x 3)

	InitConstant();

	InitXf();

	// get A 
	A = SparseMatrix<double>(nV, nE); // A is nV x nE
	getA();
	if (DEBUG) cout << "A:" << endl << MatrixXd(A) << endl << endl;

	// get L, J 
	L = A * mat_stiff * A.transpose();
	J = A * mat_stiff; // J: (nV x nE)

	// pre-factorize 
	SparseMatrix<double> mat_coeff = K * (M + h * h * L) * K.transpose();
	if (DEBUG) cout << "mat_coeff:" << endl << MatrixXd(mat_coeff) << endl << endl;
	mat_coeff.makeCompressed();
	solver.compute(mat_coeff);

}


void FastSimulate::getD()
{
	D.setZero();

	for (int i = 0; i < this->nE; i++)
	{
		auto v1_idx = edgelist[2 * i];
		auto v2_idx = edgelist[2 * i + 1];
		pointf3 v1, v2;
		v1 = pointf3(newX(v1_idx, 0), newX(v1_idx, 1), newX(v1_idx, 2));
		v2 = pointf3(newX(v2_idx, 0), newX(v2_idx, 1), newX(v2_idx, 2));

		double currentLen = pointf3::distance(v1, v2);

		//if (DEBUG) cout << "v1: (" << v1 << ") v2:(" << v2 << ") v1-v2: ("<< v1-v2 << ") originLen: "<< stringOriginLen[i] << " len:" << currentLen << endl << endl;

		vector<double> di;
		di.clear();
		for (int j = 0; j < 3; j++)
		{
			di.push_back((stringOriginLen[i] / currentLen) * (v1 - v2)[j]);
		}
		D.row(i) = Eigen::Map<Eigen::VectorXd>(di.data(), 3);
	}
	//if (DEBUG) cout << "D:" << endl << MatrixXd(D) << endl << endl;
}

void FastSimulate::getA()
{
	std::vector<Eigen::Triplet<double> > A_coeff;
	A.setZero();
	for (int i = 0; i < nE; i++)
	{
		auto v1_idx = edgelist[2 * i];
		auto v2_idx = edgelist[2 * i + 1];
		A_coeff.push_back(Eigen::Triplet<double>(v1_idx, i, +1)); // v1: + 1
		A_coeff.push_back(Eigen::Triplet<double>(v2_idx, i, -1)); // v2: -1
	}
	A.setFromTriplets(A_coeff.begin(), A_coeff.end());
}


void FastSimulate::LocalUpdate()
{
	getD();
}

void FastSimulate::GlobalUpdate()
{
	MatrixXd mat_b = K * (h * h * (J * D + Gravity) + M * y - (M + h * h * L) * b);
	this->Xf = solver.solve(mat_b);
	//if (DEBUG) cout << "solved Xf :" << endl << Xf << endl << endl;
	assert(Xf.cols() == 3);
	assert(Xf.rows() == n_notfix);
}

void FastSimulate::FastSimulateOnce(int iter)
{
	y = 2 * Xn - Xn_;
	newX = y;  // at beginning, set newX = y; 
	for (int i = 0; i < iter; i++)
	{
		if (DEBUG) cout << "newX:" << endl << MatrixXd(newX) << endl << endl;
		LocalUpdate();
		GlobalUpdate();
		newX = K.transpose() * Xf + b;
	}
	// update movement
	for (int i = 0; i < nV; i++)
	{
		if (isFixed[i] == false)
		{
			this->positions[i][0] = newX(i, 0);
			this->positions[i][1] = newX(i, 1);
			this->positions[i][2] = newX(i, 2);
		}
	}

	// update Xn, Xn_
	Xn_ = Xn;
	Xn = newX;
	if (DEBUG) cout << "Xn-1:" << endl << MatrixXd(Xn_) << endl << endl;
	if (DEBUG) cout << "Xn:" << endl << MatrixXd(Xn) << endl << endl;
	if (DEBUG) cout << "==========================================================" << endl;

}


