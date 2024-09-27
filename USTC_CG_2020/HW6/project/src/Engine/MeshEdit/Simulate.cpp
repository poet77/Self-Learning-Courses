#include <Engine/MeshEdit/Simulate.h>

#include <ctime>

using namespace Ubpa;

using namespace std;
using namespace Eigen;


void Simulate::Clear() {
	this->positions.clear();
	this->velocity.clear();
}

bool Simulate::ReInit() {
	//Clear();

	//初始化速度
	this->velocity.resize(positions.size());
	for (int i = 0; i < positions.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			this->velocity[i][j] = 0.f;
		}
	}

	size_t nV = positions.size();
	size_t nE = edgelist.size() / 2;


	//重设固定点
	delete[] isFixed;
	isFixed = new bool[nV];
	memset(isFixed, 0, sizeof(bool) * nV);
	for (int idx : fixed_id)
	{
		isFixed[idx] = true;
	}
	//准备加速算法需要的矩阵和向量
	InitAcc();
	return true;




}

bool Ubpa::Simulate::Init()
{
	this->velocity.resize(positions.size());
	for (int i = 0; i < positions.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			this->velocity[i][j] = 0.f;
		}
	}
	//初始化弹簧原长和邻接表
	linkedlist.clear();
	originlen.clear();
	size_t nV = positions.size();
	linkedlist.resize(nV);
	size_t nE = edgelist.size() / 2;
	originlen.resize(nE);
	float tempf;
	for (int i = 0; i < nE; i++)
	{
		int vi = edgelist[2 * i], vj = edgelist[2 * i + 1];
		linkedlist[vi].push_back(vj);
		linkedlist[vj].push_back(vi);
		for (int j = 0; j < 3; j++)
		{
			tempf = positions[vi][j] - positions[vj][j];
			originlen[i] += tempf * tempf;
		}
		originlen[i] = sqrt(originlen[i]);
	}
	//初始化重力
	gravity[0] = 0.1f;
	gravity[1] = 0.f;
	gravity[2] = 0.f;
	//
	stiff = 200.f;
	//设置固定点
	fixed_id.push_back(0);
	fixed_id.push_back(10);
	delete[] isFixed;
	isFixed = new bool[nV];
	memset(isFixed, 0, sizeof(bool) * nV);
	for (int idx : fixed_id)
	{
		isFixed[idx] = true;
	}
	InitAcc();
	return true;
}

bool Simulate::Run() {

	clock_t starttime = clock();
	SimulateOnce();
	cout << "time" << clock() - starttime << "ms/frame" << std::endl;
	return true;
}

void Ubpa::Simulate::SetLeftFix()
{
	//固定网格x坐标最小点
	fixed_id.clear();
	double x = 100000;
	for (int i = 0; i < positions.size(); i++)
	{
		if (positions[i][0] < x)
		{
			x = positions[i][0];
		}
	}

	for (int i = 0; i < positions.size(); i++)
	{
		if (abs(positions[i][0] - x) < 1e-5)
		{
			fixed_id.push_back(i);
		}
	}

	Init();
}

void Simulate::SimulateOnce() {
	// TODO
	//SimImplictEuler();
	SimAccelarated();
}

bool Ubpa::Simulate::SimImplictEuler()
{
	//cout << "SIM" << std::endl;
	size_t nE = edgelist.size() / 2;
	size_t nV = positions.size();

	Eigen::Matrix3f I;
	I.fill(0.f);
	I(0, 0) = I(1, 1) = I(2, 2) = 1.f;

	std::vector<Eigen::Triplet<float>> coeflist;

	Eigen::Matrix<float, -1, 3> X[2];
	X[0].resize(nV, 3); X[1].resize(nV, 3);
	Eigen::Matrix<float, -1, 3> Y;
	Y.resize(nV, 3);
	for (int i = 0; i < nV; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			Y(i, j) = positions[i][j] + h * velocity[i][j] + h * h * gravity[j] / mass;
		}
	}

	//牛顿法迭代求解
	X[0] = Y;
	Eigen::SparseMatrix<float> mat;
	mat.resize(3 * nV, 3 * nV);
	Eigen::SimplicialLLT<Eigen::SparseMatrix<float>> solver;

	int k = 0;
	while (true)
	{
		//生成矩阵
		coeflist.clear();
		coeflist.reserve(nE * 3 * 9);
		for (int i = 0; i < nV; i++)
		{
			Eigen::Matrix3f iitemp = mass * I;
			for (int j : linkedlist[i])
			{
				Eigen::Matrix<float, 1, 3> r_v = X[k % 2].row(i) - X[k % 2].row(j);
				float r_len = r_v.norm() + 0.0001f;
				Eigen::Matrix3f temp = (-h * h) * ((stiff * (originlen[i] / r_len - 1.f)) * I - (stiff * originlen[i] / r_len / r_len / r_len) * (r_v.transpose() * r_v));
				iitemp += temp;
				PushBackToCoef(coeflist, i, j, -temp);
			}
			PushBackToCoef(coeflist, i, i, iitemp);
		}
		mat.setFromTriplets(coeflist.begin(), coeflist.end());

		Eigen::Matrix<float, -1, 3> g;
		gFunc(X[k % 2], Y, g);

		solver.compute(mat);
		MatrixX3f deltaX = -TovVector(solver.solve(TofVector(g)));
		X[(k + 1) % 2] = X[k % 2] + deltaX;

		k++;
		//cout << deltaX.norm() << std::endl;
		float delta = deltaX.norm();
		if (isinf(delta) || isnan(delta))
			return false;
		if (delta / (float)nV < 0.0001f || k>10)
			break;
	}
	//cout << "Iter times:" << k << std::endl;
	//转移模拟结果
	for (int i = 0; i < nV; i++)
	{
		for (int j = 0; j < 3; j++)
			velocity[i][j] = (X[k % 2](i, j) - positions[i][j]) / h;
		if (!isFixed[i])
			for (int j = 0; j < 3; j++)
				positions[i][j] = X[k % 2](i, j);
	}
	for (int idx : fixed_id)
	{
		velocity[idx] = pointf3(0.f, 0.f, 0.f);
	}
	return true;
}

bool Ubpa::Simulate::SimAccelarated()
{
	int iterk = 0;
	size_t nV = positions.size();
	Y.resize(nV, 3);
	for (int i = 0; i < nV; i++)
		for (int j = 0; j < 3; j++)
			Y(i, j) = positions[i][j] + h * velocity[i][j];
	X = Y;
	Phi.resize(nV, 3);
	for (int i = 0; i < nV; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			Phi(i, j) = -h * h * gravity[j] / mass;
		}
	}
	while (true)
	{
		LocalPhase();
		GlobalPhase();
		float delta = deltaX.norm() / nV;
		//cout << delta << std::endl;
		if (iterk > 5)
			break;
		iterk++;
	}
	for (int i = 0; i < nV; i++)
	{
		for (int j = 0; j < 3; j++)
			velocity[i][j] = (X(i, j) - positions[i][j]) / h;
		if (!isFixed[i])
			for (int j = 0; j < 3; j++)
				positions[i][j] = X(i, j);
	}
	for (int idx : fixed_id)
	{
		velocity[idx] = pointf3(0.f, 0.f, 0.f);
	}
	return true;
}

Eigen::Matrix<float, -1, 1> Ubpa::Simulate::TofVector(Eigen::Matrix<float, -1, 3> a)
{
	Eigen::Matrix<float, -1, 1> v;
	v.resize(a.rows() * 3, 1);
	for (int i = 0; i < a.rows(); i++)
	{
		v(3 * i, 0) = a(i, 0);
		v(3 * i + 1, 0) = a(i, 1);
		v(3 * i + 2, 0) = a(i, 2);
	}
	return v;
}

Eigen::Matrix<float, -1, 3> Ubpa::Simulate::TovVector(Eigen::Matrix<float, -1, 1> a)
{
	Eigen::Matrix<float, -1, 3> v;
	v.resize(a.rows() / 3, 3);
	for (int i = 0; i < v.rows(); i++)
	{
		v(i, 0) = a(3 * i, 0);
		v(i, 1) = a(3 * i + 1, 0);
		v(i, 2) = a(3 * i + 2, 0);
	}
	return v;
}

void Ubpa::Simulate::PushBackToCoef(std::vector<Eigen::Triplet<float>>& coeflist, const int arow, const int acol, const Matrix3f& mat)
{
	//cout << "Push back" << arow << ", " << acol << std::endl << mat << std::endl;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			coeflist.push_back(Eigen::Triplet<float>(arow * 3 + i, acol * 3 + j, mat(i, j)));
		}
	}
}

void Ubpa::Simulate::gFunc(const Eigen::Matrix<float, -1, 3>& X, const Eigen::Matrix<float, -1, 3>& Y, Eigen::Matrix<float, -1, 3>& ans)
{

	size_t nE = edgelist.size() / 2;
	size_t nV = positions.size();
	Eigen::Matrix<float, -1, 3> f;
	f.resize(nV, 3);
	f.fill(0.f);
	for (int i = 0; i < nE; i++)
	{
		int vi = edgelist[2 * i], vj = edgelist[2 * i + 1];
		Matrix<float, 1, 3> r_vec = X.row(vj) - X.row(vi);
		float r_len = r_vec.norm() + 0.0001f;
		Matrix<float, 1, 3> force = (stiff * (r_len - originlen[i]) / r_len) * r_vec;
		f.row(vi) += force;
		f.row(vj) -= force;
	}
	Eigen::Matrix3f I;
	I.fill(0.f);
	I(0, 0) = I(1, 1) = I(2, 2) = 1.f;
	ans.resize(nV, 3);

	ans = mass * (X - Y) - (h * h) * f;

}

void Ubpa::Simulate::InitAcc()
{
	size_t nV = positions.size();
	size_t nE = edgelist.size() / 2;
	GenMh2L();
	GenJ();
	AcSolver.compute(Mh2L);
	Phi.resize(nV, 3);
	for (int i = 0; i < nV; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			Phi(i, j) = -h * h * gravity[j] * mass;
		}
	}
	d.resize(nE, 3);
}

void Ubpa::Simulate::GenMh2L()
{
	std::vector<Eigen::Triplet<float>> coeflist;
	size_t nV = positions.size();
	Mh2L.resize(nV, nV);
	for (int i = 0; i < nV; i++)
	{
		if (isFixed[i])
		{
			coeflist.push_back(Eigen::Triplet<float>(i, i, 1.f));
			continue;
		}
		float iicoef = mass;
		for (int j : linkedlist[i])
		{
			float Lij = stiff * h * h;
			iicoef += Lij;
			coeflist.push_back(Eigen::Triplet(i, j, -Lij));
		}
		coeflist.push_back(Eigen::Triplet<float>(i, i, iicoef));
	}
	Mh2L.setFromTriplets(coeflist.begin(), coeflist.end());
}

void Ubpa::Simulate::GenJ()
{
	size_t nV = positions.size();
	size_t nE = edgelist.size() / 2;
	std::vector<Eigen::Triplet<float>> coeflist;
	J.resize(nV, nE);
	for (int i = 0; i < nE; i++)
	{
		coeflist.push_back(Eigen::Triplet<float>(edgelist[2 * i], i, stiff));
		coeflist.push_back(Eigen::Triplet<float>(edgelist[2 * i + 1], i, -stiff));
	}
	J.setFromTriplets(coeflist.begin(), coeflist.end());
}

void Ubpa::Simulate::GenRight()
{
	vecB = (h * h) * J * d + mass * Y - Phi;
	for (auto i : fixed_id)
	{
		for (int j = 0; j < 3; j++)
			vecB(i, j) = positions[i][j];
	}
}

void Ubpa::Simulate::LocalPhase()
{
	size_t nE = edgelist.size() / 2;
	for (int i = 0; i < nE; i++)
	{
		size_t vi = edgelist[2 * i], vj = edgelist[2 * i + 1];
		Matrix<float, 1, 3> r_vec = X.row(vi) - X.row(vj);
		d.row(i) = originlen[i] / r_vec.norm() * r_vec;
	}
}

void Ubpa::Simulate::GlobalPhase()
{
	GenRight();
	//X = AcSolver.solve(vecB);
	deltaX = AcSolver.solve(vecB) - X;
	X = X + deltaX;
}
