#include <Engine/MeshEdit/ASAP.h>

#include <Engine/MeshEdit/MinSurf.h>

#include <Engine/Primitive/TriMesh.h>

#include <Eigen/Sparse>

#include <Engine/MeshEdit/Paramaterize.h>

using namespace Ubpa;
using namespace std;
using namespace Eigen;

ASAP::ASAP(Ptr<TriMesh> triMesh)
	: heMesh(make_shared<HEMesh<V>>())
{
	Init(triMesh);
}

void ASAP::Clear() {
	heMesh->Clear();
	triMesh = nullptr;
}

bool ASAP::Init(Ptr<TriMesh> triMesh) {
	Clear();

	if (triMesh == nullptr)
		return true;

	if (triMesh->GetType() == TriMesh::INVALID) {
		printf("ERROR::ASAP::Init:\n"
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
		printf("ERROR::ASAP::Init:\n"
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

	this->triMesh = triMesh;
	return true;
}

bool ASAP::Run() {
	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::MinSurf::Run\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}
	kernelASAP();

	// half-edge structure -> triangle mesh
	size_t nV = heMesh->NumVertices();
	size_t nF = heMesh->NumPolygons();

	vector<pointf2> texcoords;
	for (auto v : heMesh->Vertices())
		texcoords.push_back(v->coord.cast_to<pointf2>());
	triMesh->Update(texcoords);

	cout << "ASAP done" << endl;
	return true;
}

bool ASAP::Show()
{
	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::ASAP::Show\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}

	kernelASAP();

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

	cout << "ASAP done" << endl;
	return true;
}

void ASAP::kernelASAP()
{
	int nT = static_cast<int>(heMesh->NumPolygons());
	int nV = static_cast<int>(heMesh->NumVertices());

	if (nV < 2)
		return;
	int index1 = static_cast<int>(heMesh->Index(heMesh->Boundaries()[0][0]->Origin()));
	size_t a = heMesh->Boundaries()[0].size() / 2;
	int index2 = static_cast<int>(heMesh->Index(heMesh->Boundaries()[0][a]->End()));

	vector<double> two_points = { 0, 0, 1, 1 };

	// construct the equations
	std::vector<Triplet<double>> A_Triplet;
	VectorXd b = VectorXd::Zero(2 * (size_t)nV + 2 * (size_t)nT);
	auto triangles = heMesh->Polygons();

	for (int t = 0; t < nT; t++)
	{
		double tmp = 0;

		for (int i = 0; i < 3; i++)
		{
			int v_index = static_cast<int>(heMesh->Index(triangles[t]->BoundaryVertice()[i]));
			tmp += cot[t][i] * (diff_x[t][i] * diff_x[t][i] + diff_y[t][i] * diff_y[t][i]);

			if (v_index != index1 && v_index != index2)
			{
				A_Triplet.push_back(Triplet<double>(2 * nV + 2 * t, 2 * v_index,
					-cot[t][i] * diff_x[t][i] + cot[t][i == 0 ? 2 : i - 1] * diff_x[t][i == 0 ? 2 : i - 1]));
				A_Triplet.push_back(Triplet<double>(2 * nV + 2 * t, 2 * v_index + 1,
					-cot[t][i] * diff_y[t][i] + cot[t][i == 0 ? 2 : i - 1] * diff_y[t][i == 0 ? 2 : i - 1]));
				A_Triplet.push_back(Triplet<double>(2 * nV + 2 * t + 1, 2 * v_index,
					-cot[t][i] * diff_y[t][i] + cot[t][i == 0 ? 2 : i - 1] * diff_y[t][i == 0 ? 2 : i - 1]));
				A_Triplet.push_back(Triplet<double>(2 * nV + 2 * t + 1, 2 * v_index + 1,
					cot[t][i] * diff_x[t][i] - cot[t][i == 0 ? 2 : i - 1] * diff_x[t][i == 0 ? 2 : i - 1]));

				A_Triplet.push_back(Triplet<double>(2 * v_index, 2 * nV + 2 * t,
					-cot[t][i] * diff_x[t][i] + cot[t][i == 0 ? 2 : i - 1] * diff_x[t][i == 0 ? 2 : i - 1]));
				A_Triplet.push_back(Triplet<double>(2 * v_index + 1, 2 * nV + 2 * t,
					-cot[t][i] * diff_y[t][i] + cot[t][i == 0 ? 2 : i - 1] * diff_y[t][i == 0 ? 2 : i - 1]));
				A_Triplet.push_back(Triplet<double>(2 * v_index, 2 * nV + 2 * t + 1,
					-cot[t][i] * diff_y[t][i] + cot[t][i == 0 ? 2 : i - 1] * diff_y[t][i == 0 ? 2 : i - 1]));
				A_Triplet.push_back(Triplet<double>(2 * v_index + 1, 2 * nV + 2 * t + 1,
					cot[t][i] * diff_x[t][i] - cot[t][i == 0 ? 2 : i - 1] * diff_x[t][i == 0 ? 2 : i - 1]));
			}

			else if (v_index == index2)
			{
				b(2 * (size_t)nV + 2 * (size_t)t) -= (-cot[t][i] * diff_x[t][i] + cot[t][i == 0 ? 2 : i - 1] * diff_x[t][i == 0 ? 2 : i - 1]) * two_points[2];
				b(2 * (size_t)nV + 2 * (size_t)t) -= (-cot[t][i] * diff_y[t][i] + cot[t][i == 0 ? 2 : i - 1] * diff_y[t][i == 0 ? 2 : i - 1]) * two_points[3];
				b(2 * (size_t)nV + 2 * (size_t)t + 1) -= (-cot[t][i] * diff_y[t][i] + cot[t][i == 0 ? 2 : i - 1] * diff_y[t][i == 0 ? 2 : i - 1]) * two_points[2];
				b(2 * (size_t)nV + 2 * (size_t)t + 1) -= (cot[t][i] * diff_x[t][i] - cot[t][i == 0 ? 2 : i - 1] * diff_x[t][i == 0 ? 2 : i - 1]) * two_points[3];
			}
		}

		A_Triplet.push_back(Triplet<double>(2 * nV + 2 * t, 2 * nV + 2 * t, tmp));
		A_Triplet.push_back(Triplet<double>(2 * nV + 2 * t + 1, 2 * nV + 2 * t + 1, tmp));
	}

	for (int i = 0; i < nV; i++)
	{
		auto adjedges = heMesh->Vertices()[i]->OutHEs();
		size_t size = adjedges.size();
		double tmp = 0;

		if (i == index1 || i == index2)
		{
			A_Triplet.push_back(Triplet<double>(2 * i, 2 * i, 1));
			A_Triplet.push_back(Triplet<double>(2 * i + 1, 2 * i + 1, 1));
			b(2 * (size_t)i) = i == index1 ? two_points[0] : two_points[2];
			b(2 * (size_t)i + 1) = i == index1 ? two_points[1] : two_points[3];
			continue;
		}

		for (int j = 0; j < size; j++)
		{
			double coe_ij = 0;
			// Left Triangle
			auto triangle = adjedges[j]->Polygon();
			size_t t = 0;
			int index = 0;
			if (triangle != nullptr)
			{
				t = heMesh->Index(triangle);
				for (index = 0; heMesh->Index(triangle->BoundaryVertice()[index]) != i; index++);
				coe_ij += cot[t][index];
			}
			// Right Triangle
			triangle = adjedges[j]->Pair()->Polygon();
			if (triangle != nullptr)
			{
				t = heMesh->Index(triangle);
				for (index = 0; heMesh->Index(triangle->BoundaryVertice()[index]) != i; index++);
				coe_ij += cot[t][index == 0 ? 2 : index - 1];
			}
			// Set
			tmp += coe_ij;
			int v_index = static_cast<int>(heMesh->Index(adjedges[j]->End()));
			if (v_index != index1 && v_index != index2)
			{
				A_Triplet.push_back(Triplet<double>(i * 2, 2 * v_index, -coe_ij));
				A_Triplet.push_back(Triplet<double>(i * 2 + 1, 2 * v_index + 1, -coe_ij));
			}
			else if (v_index == index2)
			{
				b((size_t)i * 2) -= (-coe_ij) * two_points[2];
				b((size_t)i * 2 + 1) -= (-coe_ij) * two_points[3];
			}
		}

		A_Triplet.push_back(Triplet<double>(i * 2, i * 2, tmp));
		A_Triplet.push_back(Triplet<double>(i * 2 + 1, i * 2 + 1, tmp));
	}

	//cout << b << endl;

	// solve the equations
	SparseMatrix<double> A(2 * (size_t)nV + 2 * (size_t)nT, 2 * (size_t)nV + 2 * (size_t)nT);
	SimplicialLLT<SparseMatrix<double>> solver;
	A.setFromTriplets(A_Triplet.begin(), A_Triplet.end());
	//cout << A << endl;
	solver.compute(A);

	VectorXd V = solver.solve(b);
	//cout << V << endl;

	// Update the solutions
	for (auto v : heMesh->Vertices()) {
		size_t i = heMesh->Index(v);
		if (i != index1 && i != index2) {
			pointf2 tmp = { V(static_cast<Index>(2 * i)), V(static_cast<Index>(2 * i + 1)) };
			v->coord = tmp.cast_to<vecf2>();
		}
		else if (i == index1) {
			pointf2 tmp = { two_points[0], two_points[1] };
			v->coord = tmp.cast_to<vecf2>();
		}
		else {
			pointf2 tmp = { two_points[2], two_points[3] };
			v->coord = tmp.cast_to<vecf2>();
		}
	}
}

void ASAP::flatten()
{
	size_t nT = heMesh->NumPolygons();
	std::vector<std::vector<vecf2>> x_plane(nT);
	cot.resize(nT);
	auto triangles = heMesh->Polygons();

	for (int i = 0; i < nT; i++) {
		auto x_tmp = triangles[i]->BoundaryVertice();
		x_plane[i].push_back(vecf2(0, 0));
		x_plane[i].push_back(vecf2((x_tmp[1]->pos - x_tmp[0]->pos).norm(), 0));
		float x = (x_tmp[2]->pos - x_tmp[0]->pos).dot(x_tmp[1]->pos - x_tmp[0]->pos) / (x_tmp[1]->pos - x_tmp[0]->pos).norm();
		x_plane[i].push_back(vecf2(x, pow(pow((x_tmp[2]->pos - x_tmp[0]->pos).norm(), 2) - pow(x, 2), 0.5)));

		if (Paramaterize::EqualVector(x_tmp[0]->pos, x_tmp[1]->pos) || Paramaterize::EqualVector(x_tmp[1]->pos, x_tmp[2]->pos) || Paramaterize::EqualVector(x_tmp[2]->pos, x_tmp[0]->pos))
		{
			cot[i] = { 1, 1, 1 };
			diff_x.push_back({ 1, 1, 1 });
			diff_y.push_back({ 1, 1, 1 });
			continue;
		}

		diff_x.push_back({ (double)x_plane[i][0][0] - x_plane[i][1][0], (double)x_plane[i][1][0] - x_plane[i][2][0], (double)x_plane[i][2][0] - x_plane[i][0][0] });
		diff_y.push_back({ (double)x_plane[i][0][1] - x_plane[i][1][1], (double)x_plane[i][1][1] - x_plane[i][2][1], (double)x_plane[i][2][1] - x_plane[i][0][1] });

		for (int j = 0; j < 3; j++) {
			double cos_theta = (x_plane[i][j] - x_plane[i][j == 0 ? 2 : j - 1]).cos_theta(x_plane[i][j == 2 ? 0 : j + 1] - x_plane[i][j == 0 ? 2 : j - 1]);
			double abs_cot = pow(cos_theta * cos_theta / (1 - cos_theta * cos_theta), 0.5);

			cot[i].push_back(cos_theta > 0 ? abs_cot : -abs_cot);
		}
	}
}

