#pragma once

#include <Basic/HeapObj.h>
#include <UHEMesh/HEMesh.h>
#include <UGM/UGM>
#include <Engine/MeshEdit/MinSurf.h>

#include <Engine/Primitive/TriMesh.h>

#include <Eigen/Sparse>

typedef Eigen::SparseLU<Eigen::SparseMatrix<double>> solver_LU;
typedef Eigen::SparseMatrix<double> SpMat;
typedef Eigen::Triplet<double> T;

namespace Ubpa {
	class TriMesh;
	class Paramaterize;

	class MinSurf : public HeapObj {
	public:
		MinSurf(Ptr<TriMesh> triMesh);
	public:
		static const Ptr<MinSurf> New(Ptr<TriMesh> triMesh) {
			return Ubpa::New<MinSurf>(triMesh);
		}
	public:
		// clear cache data
		void Clear();

		// init cache data (eg. half-edge structure) for Run()
		bool Init(Ptr<TriMesh> triMesh);

		// call it after Init()
		bool Run();

	private:
		// kernel part of the algorithm
		void Minimize();

	private:
		class V;
		class E;
		class P;
		class V : public TVertex<V, E, P> {
		public:
			vecf3 pos;
		};
		class E : public TEdge<V, E, P> { };
		class P :public TPolygon<V, E, P> { };
	private:
		friend class Paramaterize;

		Ptr<TriMesh> triMesh;
		const Ptr<HEMesh<V>> heMesh; // vertice order is same with triMesh

		std::vector<T> big_matrix_coff_;
		SpMat big_matrix_;
		Eigen::VectorXd right_vector_;
		Eigen::VectorXd V_x_;
		Eigen::VectorXd V_y_;
		Eigen::VectorXd V_z_;
		solver_LU big_solver;
		
	};
}
