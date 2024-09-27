#include <Engine/MeshEdit/MinSurf.h>

#include <Engine/Primitive/TriMesh.h>

#include <Eigen/Sparse>

using namespace Ubpa;

using namespace std;
using namespace Eigen;

MinSurf::MinSurf(Ptr<TriMesh> triMesh)
	: heMesh(make_shared<HEMesh<V>>())
{
	Init(triMesh);
}

void MinSurf::Clear() {
	heMesh->Clear();
	triMesh = nullptr;
}

bool MinSurf::Init(Ptr<TriMesh> triMesh) {
	Clear();

	if (triMesh == nullptr)
		return true;

	if (triMesh->GetType() == TriMesh::INVALID) {
		printf("ERROR::MinSurf::Init:\n"
			"\t""trimesh is invalid\n");
		return false;
	}

	// init half-edge structure
	size_t nV = triMesh->GetPositions().size();
	vector<vector<size_t>> triangles;
	triangles.reserve(triMesh->GetTriangles().size());
	for (auto triangle : triMesh->GetTriangles())
		triangles.push_back({ triangle->idx[0], triangle->idx[1], triangle->idx[2] });
	heMesh->Reserve(nV);
	heMesh->Init(triangles);

	if (!heMesh->IsTriMesh() || !heMesh->HaveBoundary()) {
		printf("ERROR::MinSurf::Init:\n"
			"\t""trimesh is not a triangle mesh or hasn't a boundaries\n");
		heMesh->Clear();
		return false;
	}

	// triangle mesh's positions ->  half-edge structure's positions
	for (int i = 0; i < nV; i++) {
		auto v = heMesh->Vertices().at(i);
		v->pos = triMesh->GetPositions()[i].cast_to<vecf3>();
	}

	this->triMesh = triMesh;
	return true;
}

bool MinSurf::Run() {
	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::MinSurf::Run\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}

	BuildMatL();
	GetVectorb();
	Minimize();

	// half-edge structure -> triangle mesh
	size_t nV = heMesh->NumVertices();
	size_t nF = heMesh->NumPolygons();
	vector<pointf3> positions;
	vector<unsigned> indice;
	positions.reserve(nV);
	indice.reserve(3 * nF);
	for (auto v : heMesh->Vertices())
		positions.push_back(v->pos.cast_to<pointf3>());
	for (auto f : heMesh->Polygons()) { // f is triangle
		for (auto v : f->BoundaryVertice()) // vertices of the triangle
			indice.push_back(static_cast<unsigned>(heMesh->Index(v)));
	}

	triMesh->Init(indice, positions);

	return true;
}

void MinSurf::Minimize() {
	// TODO
	
	size_t nV = heMesh->NumVertices();

	Eigen::SparseLU<Eigen::SparseMatrix<float>> solver;
	Eigen::MatrixXf result;
	solver.compute(Mat_L);
	result = solver.solve(Vec_b);


	for (int i = 0; i < nV; i++)
	{
		heMesh->Vertices()[i]->pos = Ubpa::vecf3(result(i, 0), result(i, 1), result(i, 2));
	}
	/*
	cout << "WARNING::MinSurf::Minimize:" << endl
		<< "\t" << "not implemented" << endl;
		*/
}

//获得系数矩阵.由于非对称，后面应当用LU方法求解的
void Ubpa::MinSurf::BuildMatL()
{
	size_t nV = heMesh->NumVertices();
	Mat_L.resize(nV, nV);
	std::vector<Eigen::Triplet<float>> coef_of_L;
	for (auto v : heMesh->Vertices())
	{
		size_t v_idx = heMesh->Index(v);
		if (v->IsBoundary())
		{
			coef_of_L.push_back(Eigen::Triplet<float>(v_idx, v_idx, 1.f));
		}
		else
		{
			std::vector<V*> adjVs = v->AdjVertices();
			coef_of_L.push_back(Eigen::Triplet<float>(v_idx, v_idx, (float)(adjVs.size())));
			for (auto u : adjVs)
			{
				coef_of_L.push_back(Eigen::Triplet<float>(v_idx, heMesh->Index(u), -1.f));
			}
		}
	}
	Mat_L.setFromTriplets(coef_of_L.begin(), coef_of_L.end());
}

void Ubpa::MinSurf::GetVectorb()
{
	//获得向量
	Vec_b = Eigen::MatrixX3f::Zero(heMesh->NumVertices(), 3);

	for (auto v : heMesh->Vertices())
	{
		if (v->IsBoundary())
		{
			size_t v_idx = heMesh->Index(v);
			Vec_b(v_idx, 0) = v->pos[0];
			Vec_b(v_idx, 1) = v->pos[1];
			Vec_b(v_idx, 2) = v->pos[2];
		}
	}
}