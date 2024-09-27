#pragma once

#include <Basic/HeapObj.h>
#include <UHEMesh/HEMesh.h>
#include <UGM/UGM>
#include <Engine/MeshEdit/ASAP.h>
#include <Eigen/Dense>
#include <Eigen/Sparse>

namespace Ubpa {
	class TriMesh;
	class MinSurf;

	// mesh boundary == 1
	class ARAP : public HeapObj {
	public:
		ARAP(Ptr<TriMesh> triMesh);
	public:
		static const Ptr<ARAP> New(Ptr<TriMesh> triMesh) {
			return Ubpa::New<ARAP>(triMesh);
		}
	public:
		void Clear();
		bool Init(Ptr<TriMesh> triMesh);

		bool Run();			// Run and only change the texcoords of vertices
		bool Show();		// Run and change the position of vertices
		void setTimes(int time);

	private:
		class V;
		class E;
		class P;
		class V : public TVertex<V, E, P> {
		public:
			vecf2 coord;
			vecf3 pos;
		};
		class E : public TEdge<V, E, P> { };
		class P : public TPolygon<V, E, P> {
		public:
			Eigen::Matrix2d L;
			std::vector<vecf2> x_flatten;
			std::vector<double> cot;	// (i,i+1)
		};

	private:
		void kernelARAP();
		void flatten();
		void LocalSetL();
		void GlobalSolveU(Eigen::SimplicialLLT<Eigen::SparseMatrix<double>>& solver);
		void GlobalMatrixA(Eigen::SparseMatrix<double>& A);

	private:
		Ptr<TriMesh> triMesh;
		const Ptr<HEMesh<V>> heMesh;	// vertice order is same with triMesh
		int times = 1;
		std::vector<size_t> fixed_vertices_;
		std::vector<vecf2> fixed_coords_;
	};
}