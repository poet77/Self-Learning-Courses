#pragma once


#include <Basic/HeapObj.h>
//#include <Engine/Primitive/MassSpring.h>
#include <UGM/UGM>
#include <Eigen/dense>
#include <Eigen/Sparse>
#include <Eigen/src/SparseCholesky/SimplicialCholesky.h>
#include <Eigen/src/SparseLU/SparseLU.h>

namespace Ubpa {

	class Simulate : public HeapObj {
	public:
		Simulate(const std::vector<pointf3>& plist,
			const std::vector<unsigned>& elist) {
			edgelist = elist;
			isFixed = nullptr;
			this->positions.resize(plist.size());
			for (int i = 0; i < plist.size(); i++)
			{
				for (int j = 0; j < 3; j++)
				{
					this->positions[i][j] = plist[i][j];
				}
			}
		};
		~Simulate()
		{
			delete[] isFixed;
		}
	public:
		static const Ptr<Simulate> New(const std::vector<pointf3>& plist,
			const std::vector<unsigned>& elist) {
			return Ubpa::New<Simulate>(plist, elist);
		}

	public:
		// clear cache data
		void Clear();

		// init cache data (eg. half-edge structure) for Run()
		bool ReInit();
		bool Init();

		// call it after Init()
		bool Run();

		const std::vector<pointf3>& GetPositions() const { return positions; };

		const float GetStiff() { return stiff; };
		void SetStiff(float k) { stiff = k; ReInit(); };
		const float GetTimeStep() { return h; };
		void SetTimeStep(float k) { h = k; ReInit(); };
		std::vector<unsigned>& GetFix() { return this->fixed_id; };
		void SetFix(const std::vector<unsigned>& f) { this->fixed_id = f; ReInit(); };
		const std::vector<pointf3>& GetVelocity() { return velocity; };
		//void SetVelocity(const std::vector<pointf3>& v) { velocity = v; };

		pointf3& GetGravity() { return gravity; };

		void SetLeftFix();

	private:
		// kernel part of the algorithm
		void SimulateOnce();
		bool SimImplictEuler();
		//bool SimExplictEuler();
		bool SimAccelarated();
		//void GenMat();
		Eigen::Matrix<float, -1, 1> TofVector(Eigen::Matrix<float, -1, 3> a);
		Eigen::Matrix<float, -1, 3> TovVector(Eigen::Matrix<float, -1, 1> a);
		void PushBackToCoef(std::vector<Eigen::Triplet<float>>& coeflist, const int arow, const int acol, const Eigen::Matrix3f& mat);


		void gFunc(const Eigen::Matrix<float, -1, 3>& X, const Eigen::Matrix<float, -1, 3>& Y, Eigen::Matrix<float, -1, 3>& ans);
		void InitAcc();
		void GenMh2L();
		void GenJ();
		void GenRight();
		void LocalPhase();
		void GlobalPhase();

	private:
		float h = 0.03f;  //²½³¤
		float stiff = 1.f;
		std::vector<unsigned> fixed_id;  //fixed point id
		Ubpa::pointf3 gravity;
		float mass = 1.f;

		//mesh data
		std::vector<unsigned> edgelist;
		std::vector<float>	originlen;
		std::vector<std::list<size_t>> linkedlist;
		bool* isFixed;
		//simulation data
		std::vector<pointf3> positions;
		std::vector<pointf3> velocity;

		//Accelarate method
		Eigen::SparseMatrix<float> Mh2L;
		Eigen::SparseMatrix<float> J;
		Eigen::MatrixX3f	d;
		Eigen::MatrixX3f	Y;
		Eigen::MatrixX3f	Phi;
		Eigen::MatrixX3f	vecB;
		Eigen::MatrixX3f	X;
		Eigen::MatrixX3f	deltaX;
		Eigen::SparseLU<Eigen::SparseMatrix<float>> AcSolver;
		//Eigen::Sparse
	};
}
