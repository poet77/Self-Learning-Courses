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

	cout << "Minimize done" << endl;
	return true;
}

void MinSurf::Minimize() {
	// Construct the equations
	std::vector<Triplet<double>> A_Triplet;
	size_t nV = heMesh->NumVertices();
	VectorXd b_x = VectorXd::Zero(nV);
	VectorXd b_y = VectorXd::Zero(nV);
	VectorXd b_z = VectorXd::Zero(nV);

	for (auto v : heMesh->Vertices()) {
		int i = static_cast<int>(heMesh->Index(v));
		if (v->IsBoundary()) {
			A_Triplet.push_back(Triplet<double>(i, i, 1));
			b_x(i) += (v->pos[0]);
			b_y(i) += (v->pos[1]);
			b_z(i) += (v->pos[2]);
			continue;
		}

		A_Triplet.push_back(Triplet<double>(i, i, double(v->Degree())));

		for (auto neighbour : v->AdjVertices()) {
			if (neighbour->IsBoundary()) {
				b_x(i) += (neighbour->pos[0]);
				b_y(i) += (neighbour->pos[1]);
				b_z(i) += (neighbour->pos[2]);
			}
			else {
				int i_neighbour = static_cast<int>(heMesh->Index(neighbour));
				A_Triplet.push_back(Triplet<double>(i, i_neighbour, -1));
			}
		}
	}

	// solve the equations
	SparseMatrix<double> A(nV, nV);
	SimplicialLLT<SparseMatrix<double>> solver;
	A.setFromTriplets(A_Triplet.begin(), A_Triplet.end());
	solver.compute(A);

	VectorXd Vx = solver.solve(b_x);
	VectorXd Vy = solver.solve(b_y);
	VectorXd Vz = solver.solve(b_z);

	// Set the solutions
	for (auto v : heMesh->Vertices()) {
		size_t i = heMesh->Index(v);
		pointf3 tmp = { Vx(i), Vy(i), Vz(i) };
		v->pos = tmp.cast_to<vecf3>();
	}
}
