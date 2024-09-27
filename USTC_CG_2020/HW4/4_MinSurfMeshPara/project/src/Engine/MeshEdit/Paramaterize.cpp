#include <Engine/MeshEdit/Paramaterize.h>

#include <Engine/MeshEdit/MinSurf.h>

#include <Engine/Primitive/TriMesh.h>

#include <Eigen/Sparse>



using namespace Ubpa;

using namespace std;

using namespace Eigen;

Paramaterize::Paramaterize(Ptr<TriMesh> triMesh) : heMesh(make_shared<HEMesh<V>>())
{
	Init(triMesh);
	//minSurf = MinSurf::New(triMesh);
	//this->triMesh = triMesh;
}


void Paramaterize::Clear() {
	heMesh->Clear();
	triMesh = nullptr;

	//minSurf->Clear();
	
	// TODO
	//cout << "Paramaterize::Clear:" << endl
	//	<< "\t" << "not implemented" << endl;
}

bool Paramaterize::Init(Ptr<TriMesh> triMesh) {
	// TODO
	//Clear();
	//minSurf->Init(triMesh);
	//return true;
	/*
	cout << "Paramaterize::Init:" << endl
		<< "\t" << "not implemented" << endl;
		*/
//	return false;

	/*
	Clear();
	minSurf->Init(triMesh);
	return true;
	*/



	Clear();

	if (triMesh == nullptr)
		return true;

	if (triMesh->GetType() == TriMesh::INVALID) {
		printf("ERROR::Paramaterize::Init:\n"
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
		printf("ERROR::Paramaterize::Init:\n"
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

	this->triMesh = triMesh;
	return true;

}

bool Paramaterize::Run() {	

	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::MinSurf::Run\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}

	kernelParamaterize();

	// Set half-edge structure -> triangle mesh 
	size_t nV = heMesh->NumVertices();
	size_t nF = heMesh->NumPolygons();

	vector<pointf2> texcoords;
	for (auto v : heMesh->Vertices())
		texcoords.push_back(v->coord.cast_to<pointf2>());
	triMesh->Update(texcoords);

	cout << "Parameterize done" << endl;
	return true;
}

bool Paramaterize::Show()
{
	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::MinSurf::Show\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}

	kernelParamaterize();

	// Set half-edge structure -> triangle mesh 
	size_t nV = heMesh->NumVertices();
	size_t nF = heMesh->NumPolygons();
	vector<pointf3> positions;
	vector<unsigned> indice;
	positions.reserve(nV);
	indice.reserve(3 * nF);

	// Set positions
	for (auto v : heMesh->Vertices()) {
		if (shape == 0 && v->Degree() == 2 && v->IsBoundary()) {
			// change the position of some special vertices
			if (abs(v->coord[0] - 1) < 1e-6)
				v->coord[0] += 0.01f;
			else if (abs(v->coord[0]) < 1e-6)
				v->coord[0] -= 0.01f;
			else if (abs(v->coord[1] - 1) < 1e-6)
				v->coord[1] += 0.01f;
			else if (abs(v->coord[1]) < 1e-6)
				v->coord[1] -= 0.01f;
		}

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

	cout << "Parameterize done" << endl;
	return true;
}

void Paramaterize::kernelParamaterize()
{
	setBoundary();

	// construct the equations
	std::vector<Triplet<double>> A_Triplet;
	size_t nV = heMesh->NumVertices();
	VectorXd b_x = VectorXd::Zero(nV);
	VectorXd b_y = VectorXd::Zero(nV);

	for (auto v : heMesh->Vertices()) {
		int i = static_cast<int>(heMesh->Index(v));

		if (v->IsBoundary()) {
			A_Triplet.push_back(Triplet<double>(i, i, 1));
			b_x(i) += (v->coord[0]);
			b_y(i) += (v->coord[1]);
			continue;
		}

		vector<double> weight_list_;
		double sum = 0;
		setWeightValue(v, weight_list_, sum);
		A_Triplet.push_back(Triplet<double>(i, i, sum));

		for (int j = 0; j < weight_list_.size(); j++) {
			auto neighbour = v->AdjVertices()[j];
			if (neighbour->IsBoundary()) {
				b_x(i) += weight_list_[j] * (neighbour->coord[0]);
				b_y(i) += weight_list_[j] * (neighbour->coord[1]);
			}
			else {
				int i_neighbour = static_cast<int>(heMesh->Index(neighbour));
				A_Triplet.push_back(Triplet<double>(i, i_neighbour, -weight_list_[j]));
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

	// Update the solutions
	for (auto v : heMesh->Vertices()) {
		size_t i = heMesh->Index(v);
		pointf2 tmp = { Vx(i), Vy(i) };
		v->coord = tmp.cast_to<vecf2>();
	}
}

// Map the boundary uniformly onto the plane
void Paramaterize::setBoundary()
{
	auto boundaries = heMesh->Boundaries()[0];
	float theta = 0;
	float dtheta = 360 / static_cast<float>(boundaries.size());

	switch (shape)
	{
	case 0:
		// Square
		theta = -45;

		for (auto he : boundaries) {
			auto v = he->Origin();
			if (theta >= -45 && theta < 45) {
				pointf2 tmp = { 1, 0.5 * tan(theta * PI<double> / 180.) + 0.5 };
				v->coord = tmp.cast_to<vecf2>();
			}
			else if (theta >= 45 && theta < 135) {
				pointf2 tmp = { 0.5 / tan(theta * PI<double> / 180.) + 0.5, 1 };
				v->coord = tmp.cast_to<vecf2>();
			}
			else if (theta >= 135 && theta < 225) {
				pointf2 tmp = { 0, -0.5 * tan(theta * PI<double> / 180.) + 0.5 };
				v->coord = tmp.cast_to<vecf2>();
			}
			else if (theta >= 225 && theta < 315) {
				pointf2 tmp = { -0.5 / tan(theta * PI<double> / 180.) + 0.5, 0 };
				v->coord = tmp.cast_to<vecf2>();
			}
			theta += dtheta;
		}
		break;
	case 1:
		// Circle
		for (auto he : boundaries) {
			auto v = he->Origin();
			pointf2 tmp = { 0.5 + 0.5 * cos(theta * PI<double> / 180.), 0.5 + 0.5 * sin(theta * PI<double> / 180.) };
			v->coord = tmp.cast_to<vecf2>();
			theta += dtheta;
		}
		break;
	default:
		break;
	}


}

bool Paramaterize::setShape(int type)
{
	shape = type;
	return true;
}

bool Paramaterize::setWeight(int type)
{
	weight = type;
	return true;
}

void Paramaterize::setWeightValue(V* vertex, vector<double>& weight_list_, double& sum)
{
	auto adjVertice_list_ = vertex->AdjVertices();
	sum = 0;

	switch (weight)
	{
	case 0:
		// Normal
		for (int i = 0; i < adjVertice_list_.size(); i++) {
			weight_list_.push_back(1);
		}
		sum = (double)vertex->Degree();
		break;
	case 1:
		// Cotangent
		for (int i = 0; i < adjVertice_list_.size(); i++) {
			auto v0 = adjVertice_list_[i];
			auto v1 = adjVertice_list_[i - 1 < 0 ? (int)adjVertice_list_.size() - 1 : i - 1];
			auto v2 = adjVertice_list_[i + 1 >= (int)adjVertice_list_.size() ? 0 : i + 1];

			double cos_alpha = vecf3::cos_theta(v0->pos - v1->pos, vertex->pos - v1->pos);
			double cos_beta = vecf3::cos_theta(v0->pos - v2->pos, vertex->pos - v2->pos);

			double tmp = pow(cos_alpha * cos_alpha / (1 - cos_alpha * cos_alpha), 0.5);
			tmp += pow(cos_beta * cos_beta / (1 - cos_beta * cos_beta), 0.5);
			weight_list_.push_back(tmp);
			sum += tmp;
		}
		break;
	default:
		break;
	}
}


/*
void Ubpa::Paramaterize::GetMinSurfMesh()
{
	// 转换 half-edge structure -> triangle mesh
	size_t nV = minSurf->heMesh->NumVertices();
	size_t nF = minSurf->heMesh->NumPolygons();
	vector<pointf3> positions;
	vector<unsigned> indice;
	positions.reserve(nV);
	indice.reserve(3 * nF);
	for (auto v : minSurf->heMesh->Vertices())
		positions.push_back(v->pos.cast_to<Ubpa::pointf3>());
	for (auto f : minSurf->heMesh->Polygons()) { // f is triangle
		for (auto v : f->BoundaryVertice()) // vertices of the triangle
			indice.push_back(static_cast<unsigned>(minSurf->heMesh->Index(v)));
	}
	triMesh->Init(indice, positions);
}

*/

/*

bool Paramaterize::Run(EnumMethod weightmethod, EnumMethod boundarymethod) {
	switch (weightmethod)
	{
	case UNIFORM_WEIGHT:
		GenWeight_Uniform();
		break;
	case COTANGENT_WEIGHT:
		GenWeight_Cotangent();
		break;
	case SHAPE_PRESERVING:
		GenWeight_ShapePreserving();
		break;
	default:
		cout << "Invalid Parameter: weightmethod" << std::endl;
		return false;
		break;
	}
	switch (boundarymethod)
	{
	case ROUND_BOUNDARY:
		MapBoundaryRound();
		break;
	case SQUARE_BOUNDARY:
		MapBoundarySquare();
		break;
	default:
		cout << "Invalid Parameter: boundarymethod" << std::endl;
		return false;
		break;
	}
	minSurf->GenVecb();
	//调用minSurf的最小化求解
	minSurf->Minimize();
	size_t nV = minSurf->heMesh->NumVertices();

	//texcoords是引用
	std::vector<Ubpa::pointf2>& texcoords = triMesh->GetTexcoords();

	//triMesh 和 heMesh 中的点索引一致， 所以对应赋值
	for (int i = 0; i < texcoords.size(); i++)
	{
		texcoords[i] =Ubpa::pointf2(minSurf->heMesh->Vertices()[i]->pos[0], minSurf->heMesh->Vertices()[i]->pos[1]);
	}
	return true;
}

*/

/*
void Ubpa::Paramaterize::GetMinSurfMesh()
{
	// 转换 half-edge structure -> triangle mesh
	size_t nV = minSurf->heMesh->NumVertices();
	size_t nF = minSurf->heMesh->NumPolygons();
	vector<pointf3> positions;
	vector<unsigned> indice;
	positions.reserve(nV);
	indice.reserve(3 * nF);
	for (auto v : minSurf->heMesh->Vertices())
		positions.push_back(v->pos.cast_to<Ubpa::pointf3>());
	for (auto f : minSurf->heMesh->Polygons()) { // f is triangle
		for (auto v : f->BoundaryVertice()) // vertices of the triangle
			indice.push_back(static_cast<unsigned>(minSurf->heMesh->Index(v)));
	}
	triMesh->Init(indice, positions);
}

*/