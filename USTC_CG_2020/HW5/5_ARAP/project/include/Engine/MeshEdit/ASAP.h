#pragma once

#include <Basic/HeapObj.h>
#include <UHEMesh/HEMesh.h>
#include <UGM/UGM>

namespace Ubpa {
	class TriMesh;
	class MinSurf;

	// mesh boundary == 1
	class ASAP : public HeapObj {
	public:
		ASAP(Ptr<TriMesh> triMesh);
	public:
		static const Ptr<ASAP> New(Ptr<TriMesh> triMesh) {
			return Ubpa::New<ASAP>(triMesh);
		}
	public:
		void Clear();
		bool Init(Ptr<TriMesh> triMesh);

		bool Run();			// Run and only change the texcoords of vertices
		bool Show();		// Run and change the position of vertices

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
		class P : public TPolygon<V, E, P> { };

	private:
		void kernelASAP();
		void flatten();

	private:
		Ptr<TriMesh> triMesh;
		const Ptr<HEMesh<V>> heMesh;	// vertice order is same with triMesh

		std::vector<std::vector<double>> cot;
		std::vector<std::vector<double>> diff_x;
		std::vector<std::vector<double>> diff_y;
	};
}