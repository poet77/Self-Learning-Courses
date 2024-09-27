#pragma once
#include <Basic/HeapObj.h>
#include <UHEMesh/HEMesh.h>
#include <UGM/UGM>
#include <Eigen/Core>
#include <Eigen/SparseLU>
#include <Engine/MeshEdit/MinSurf.h>
#include <Basic/HeapObj.h>
#include <Basic/HeapObj.h>

namespace Ubpa {
	class TriMesh;
	class MinSurf;

	// mesh boundary == 1
	class Paramaterize : public HeapObj {
	public:
		Paramaterize(Ptr<TriMesh> triMesh);
	public:
		static const Ptr<Paramaterize> New(Ptr<TriMesh> triMesh) {
			return Ubpa::New<Paramaterize>(triMesh);
		}
	public:
		void Clear();
		bool Init(Ptr<TriMesh> triMesh);

		bool setShape(int type);
		bool setWeight(int type);
		bool Run();			// Run and only change the texcoords of vertices
		bool Show();		// Run and change the position of vertices

	private:
		// kernel part of the algorithm
		void Para();

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
		class P :public TPolygon<V, E, P> { };

	

	private:
		Ptr<MinSurf> minSurf;     // sloved by minSurf 
		Ptr<TriMesh> triMesh;
		
		//friend class Paramaterize;

		
		const Ptr<HEMesh<V>> heMesh; // vertice order is same with triMesh
		bool tex_;
		int type_;
	

		void kernelParamaterize();
		void setBoundary();				// Set the data of the boundary
		void setWeightValue(V* vertex, std::vector<double>& weight_list_, double& sum);
		// Set the weight of the neighbours of a vertice 

	private:
		int shape = 0;					// 0->square, 1->circle
		int weight = 0;					// 0->uniform, 1->cotangent
	
	

	protected:
		void ParaU();
		void ParaTan();
		void FindInside();
		void SetBoundary();
		void GetMap();
		//void BuildLMat();
		//void BuildDeltaMat();
		void UpdatePara();
	};
}
