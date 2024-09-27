#include <Engine/MeshEdit/ASAP.h>

#include <Engine/MeshEdit/ARAP.h>

#include <Engine/Primitive/TriMesh.h>

#include <Eigen/Sparse>

#include <Engine/MeshEdit/Paramaterize.h>

using namespace Ubpa;
using namespace std;
using namespace Eigen;

ARAP::ARAP(Ptr<TriMesh> triMesh)
	: heMesh(make_shared<HEMesh<V>>())
{
	Init(triMesh);
}

void ARAP::Clear() {
	heMesh->Clear();
	triMesh = nullptr;
}

bool ARAP::Init(Ptr<TriMesh> triMesh) {
	Clear();

	if (triMesh == nullptr)
		return true;

	if (triMesh->GetType() == TriMesh::INVALID) {
		printf("ERROR::ARAP::Init:\n"
			"\t""trimesh is invalid\n");
		return false;
	}

	/*auto paramaterize = Paramaterize::New(triMesh);
	paramaterize->setShape(1);
	paramaterize->setWeight(0);
	paramaterize->Run();*/
	auto asap = ASAP::New(triMesh);
	asap->Run();

	// init half-edge structure
	size_t nV = triMesh->GetPositions().size();
	vector<vector<size_t>> triangles;
	triangles.reserve(triMesh->GetTriangles().size());
	for (auto triangle : triMesh->GetTriangles())
		triangles.push_back({ triangle->idx[0], triangle->idx[1], triangle->idx[2] });
	heMesh->Reserve(nV);
	heMesh->Init(triangles);

	if (!heMesh->IsTriMesh() || !heMesh->HaveBoundary()) {
		printf("ERROR::ARAP::Init:\n"
			"\t""trimesh is not a triangle mesh or hasn't a boundaries\n");
		heMesh->Clear();
		return false;
	}

	// triangle mesh's texcoords and positions ->  half-edge structure's texcoords and positions
	for (int i = 0; i < nV; i++) {
		auto v = heMesh->Vertices().at(i);
		v->coord = triMesh->GetTexcoords()[i].cast_to<vecf2>();
		v->pos = triMesh->GetPositions()[i].cast_to<vecf3>();
	}

	// flatten the triangles to the plane
	flatten();

	// set two fixed vertices
	if (nV < 2) {
		printf("ERROR::ARAP::Init:\n"
			"\t""need more vertices\n");
		heMesh->Clear();
		return false;
	}
	size_t a = heMesh->Boundaries()[0].size() / 2;
	auto v1 = heMesh->Boundaries()[0][0]->Origin();
	auto v2 = heMesh->Boundaries()[0][a]->End();
	fixed_vertices_.push_back(heMesh->Index(v1));
	fixed_coords_.push_back(vecf2(0, 0));

	this->triMesh = triMesh;
	return true;
}

bool ARAP::Run() {
	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::MinSurf::Run\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}
	for (int i = 0; i < times; i++)
	{
		kernelARAP();
	}

	// half-edge structure -> triangle mesh
	size_t nV = heMesh->NumVertices();
	size_t nF = heMesh->NumPolygons();

	vector<pointf2> texcoords;
	for (auto v : heMesh->Vertices())
		texcoords.push_back(v->coord.cast_to<pointf2>());
	triMesh->Update(texcoords);

	cout << "ARAP done" << endl;
	return true;
}

bool ARAP::Show()
{
	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::ARAP::Show\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}

	kernelARAP();

	// half-edge structure -> triangle mesh
	size_t nV = heMesh->NumVertices();
	size_t nF = heMesh->NumPolygons();
	vector<pointf3> positions;
	vector<unsigned> indice;
	positions.reserve(nV);
	indice.reserve(3 * nF);

	// Set positions
	for (auto v : heMesh->Vertices()) {
		positions.push_back(v->coord.cast_to<pointf3>());
	}

	for (auto f : heMesh->Polygons()) { // f is triangle
		for (auto v : f->BoundaryVertice()) { // vertices of the triangle
			indice.push_back(static_cast<unsigned>(heMesh->Index(v)));
		}
	}

	triMesh->Init(indice, positions);

	// Set texcoords
	vector<pointf2> texcoords;
	for (auto v : heMesh->Vertices())
		texcoords.push_back(v->coord.cast_to<pointf2>());
	triMesh->Update(texcoords);

	cout << "ARAP done" << endl;
	return true;
}

void ARAP::kernelARAP()
{
	size_t nV = heMesh->NumVertices();
	SparseMatrix<double> A(nV, nV);
	GlobalMatrixA(A);
	//cout << A << endl;

	SimplicialLLT<SparseMatrix<double>> solver;
	solver.compute(A);

	if (times < 0 || times > 100)
		times = 10;
	cout << "time = " << times << endl;

	for (int count = 0; count < times; count++)
	{
		LocalSetL();
		GlobalSolveU(solver);
	}
}

void ARAP::flatten()
{
	size_t nT = heMesh->NumPolygons();
	auto triangles = heMesh->Polygons();

	for (int i = 0; i < nT; i++) {
		auto x_tmp = triangles[i]->BoundaryVertice();

		if (Paramaterize::EqualVector(x_tmp[0]->pos, x_tmp[1]->pos) || Paramaterize::EqualVector(x_tmp[1]->pos, x_tmp[2]->pos) || Paramaterize::EqualVector(x_tmp[2]->pos, x_tmp[0]->pos))
		{
			triangles[i]->cot = { 1, 1, 1 };
			triangles[i]->x_flatten = { vecf2(1, 1), vecf2(1, 1), vecf2(1, 1) };
			continue;
		}

		triangles[i]->x_flatten.push_back(vecf2(0, 0));
		triangles[i]->x_flatten.push_back(vecf2((x_tmp[1]->pos - x_tmp[0]->pos).norm(), 0));
		float x = (x_tmp[2]->pos - x_tmp[0]->pos).dot(x_tmp[1]->pos - x_tmp[0]->pos) / (x_tmp[1]->pos - x_tmp[0]->pos).norm();
		triangles[i]->x_flatten.push_back(vecf2(x, pow(pow((x_tmp[2]->pos - x_tmp[0]->pos).norm(), 2) - pow(x, 2), 0.5)));

		//cout << triangles[i]->x_flatten[0] << triangles[i]->x_flatten[1] << triangles[i]->x_flatten[2] << endl;


		for (int j = 0; j < 3; j++) {
			double cos_theta = (triangles[i]->x_flatten[j] - triangles[i]->x_flatten[j == 0 ? 2 : j - 1])
				.cos_theta(triangles[i]->x_flatten[j == 2 ? 0 : j + 1] - triangles[i]->x_flatten[j == 0 ? 2 : j - 1]);
			double abs_cot = pow(cos_theta * cos_theta / (1 - cos_theta * cos_theta), 0.5);

			triangles[i]->cot.push_back(cos_theta > 0 ? abs_cot : -abs_cot);
			//cout << triangles[i]->cot[j] << endl;
		}
	}
}

void ARAP::LocalSetL()
{
	for (auto triangle : heMesh->Polygons())
	{
		auto vertices = triangle->BoundaryVertice();
		Matrix2d X, U;
		X << (triangle->x_flatten[0] - triangle->x_flatten[1])[0], (triangle->x_flatten[1] - triangle->x_flatten[2])[0],
			(triangle->x_flatten[0] - triangle->x_flatten[1])[1], (triangle->x_flatten[1] - triangle->x_flatten[2])[1];
		U << (vertices[0]->coord - vertices[1]->coord)[0], (vertices[1]->coord - vertices[2]->coord)[0],
			(vertices[0]->coord - vertices[1]->coord)[1], (vertices[1]->coord - vertices[2]->coord)[1];
		Matrix2d J = U * X.inverse();

		JacobiSVD<Matrix2d> svd(J, ComputeFullU | ComputeFullV);
		if (J.determinant() > 0)
			triangle->L = svd.matrixU() * svd.matrixV().transpose();
		else
		{
			Matrix2d D;
			D(0, 0) = 1; D(0, 1) = 0; D(1, 0) = 0; D(1, 1) = -1;
			triangle->L = svd.matrixU() * D * svd.matrixV().transpose();
		}

		//cout << "L:" << triangle->L << endl;
	}
}

void ARAP::GlobalSolveU(SimplicialLLT<SparseMatrix<double>>& solver)
{
	size_t nV = heMesh->NumVertices();
	VectorXd b_x = VectorXd::Zero(nV);
	VectorXd b_y = VectorXd::Zero(nV);

	auto vertice_list_ = heMesh->Vertices();

	for (int i = 0; i < nV; i++)
	{
		if (i == fixed_vertices_[0])
		{
			b_x(i) += fixed_coords_[0][0];
			b_y(i) += fixed_coords_[0][1];
			continue;
		}
		/*if (i == fixed_vertices_[1])
		{
			b_x(i) += fixed_coords_[1][0];
			b_y(i) += fixed_coords_[1][1];
			continue;
		}*/
		Vector2d sum = Vector2d::Zero();
		for (auto he : vertice_list_[i]->OutHEs())
		{
			// Left Triangle
			auto triangle = he->Polygon();
			size_t t = 0;
			int index = 0;
			double coe_ij = 0;
			if (triangle != nullptr)
			{
				for (index = 0; heMesh->Index(triangle->BoundaryVertice()[index]) != i; index++);
				auto diffx = triangle->x_flatten[index] - triangle->x_flatten[index == 2 ? 0 : index + 1];
				coe_ij += triangle->cot[index];
				sum += triangle->cot[index] * triangle->L * Vector2d(diffx[0], diffx[1]);
			}
			// Right Triangle
			triangle = he->Pair()->Polygon();
			if (triangle != nullptr)
			{
				for (index = 0; heMesh->Index(triangle->BoundaryVertice()[index]) != i; index++);
				auto diffx = triangle->x_flatten[index] - triangle->x_flatten[index == 0 ? 2 : index - 1];
				coe_ij += triangle->cot[index == 0 ? 2 : index - 1];
				sum += triangle->cot[index == 0 ? 2 : index - 1] * triangle->L * Vector2d(diffx[0], diffx[1]);
			}
			// Set
			int j = static_cast<int>(heMesh->Index(he->End()));
			if (j == fixed_vertices_[0])
			{
				b_x(i) -= (-coe_ij) * fixed_coords_[0][0];
				b_y(i) -= (-coe_ij) * fixed_coords_[0][1];
			}
			/*else if (j == fixed_vertices_[1])
			{
				b_x(i) -= (-coe_ij) * fixed_coords_[1][0];
				b_y(i) -= (-coe_ij) * fixed_coords_[1][1];
			}*/

		}

		b_x(i) += sum(0);
		b_y(i) += sum(1);
	}
	//cout << b_x << endl << b_y << endl;
	VectorXd u_x = solver.solve(b_x);
	VectorXd u_y = solver.solve(b_y);
	//cout << u_x << endl << u_y << endl;

	for (int i = 0; i < nV; i++)
	{
		vertice_list_[i]->coord = vecf2(u_x(i), u_y(i));
	}
}

void ARAP::GlobalMatrixA(SparseMatrix<double>& A)
{
	vector<Triplet<double>> A_Triplet;
	size_t nV = heMesh->NumVertices();
	auto vertice_list_ = heMesh->Vertices();

	for (int i = 0; i < nV; i++)
	{
		//if (i == fixed_vertices_[0] || i == fixed_vertices_[1])
		if (i == fixed_vertices_[0])
		{
			A_Triplet.push_back(Triplet<double>(i, i, 1));
			continue;
		}
		double sum = 0;
		for (auto he : vertice_list_[i]->OutHEs())
		{
			double coe_ij = 0;
			// Left Triangle
			auto triangle = he->Polygon();
			size_t t = 0;
			int index = 0;
			if (triangle != nullptr)
			{
				for (index = 0; heMesh->Index(triangle->BoundaryVertice()[index]) != i; index++);
				coe_ij += triangle->cot[index];
			}
			// Right Triangle
			triangle = he->Pair()->Polygon();
			if (triangle != nullptr)
			{
				for (index = 0; heMesh->Index(triangle->BoundaryVertice()[index]) != i; index++);
				coe_ij += triangle->cot[index == 0 ? 2 : index - 1];
			}
			// Set
			sum += coe_ij;
			int j = static_cast<int>(heMesh->Index(he->End()));
			//if (j != fixed_vertices_[0] && j != fixed_vertices_[1])
			if (j != fixed_vertices_[0])
				A_Triplet.push_back(Triplet<double>(i, j, -coe_ij));
		}

		A_Triplet.push_back(Triplet<double>(i, i, sum));
	}

	A.setFromTriplets(A_Triplet.begin(), A_Triplet.end());
}

void ARAP::setTimes(int time)
{
	times = time;
}
