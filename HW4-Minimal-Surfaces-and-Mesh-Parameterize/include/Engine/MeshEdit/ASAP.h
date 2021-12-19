#pragma once
#include <Basic/HeapObj.h>
#include <Basic/HeapObj.h>
#include <UHEMesh/HEMesh.h>
#include <UGM/UGM>
#include <Engine/MeshEdit/Paramaterize.h>
#include <Engine/Primitive/TriMesh.h>
#include <Eigen/Sparse>


typedef Eigen::SimplicialLLT<Eigen::SparseMatrix<double>> solver_LLT;
typedef Eigen::SparseMatrix<double> SpMat;
typedef Eigen::Triplet<double> T;

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

		bool Run();



	private:
		// kernel part of the algorithm
		void ASAP_exe_();

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

	public:
		
		enum display_status_
		{
			kon,
			koff
		}current_display_status_ = koff;

		//get the isometrix parameter
		void flatten();
		void get_anchor_point_index_();

		//elenmental calculation
		static double Distance(V* v1, V* v2);
		static double vec_Cotangent(V* v1, V* v2, V* v3);
		static double Cos_V(V* v1, V* v2, V* v3);

		//set the parameter
		void Set_Big_Matrix_();
		void Solve();
	private:
		Ptr<TriMesh> triMesh;
		const Ptr<HEMesh<V>> heMesh; // vertice order is same with triMesh
		std::vector<T> big_matrix_coff_;
		SpMat big_matrix_;
		solver_LU big_solver;
		Eigen::VectorXd right_vector_all;
		Eigen::VectorXd V_result_;

		size_t anchor_point_index_1;                            //set the first anchor point to (0,0)
		size_t anchor_point_index_2;                            //set the second anchor point to (1,1)
		std::vector<std::vector<pointf2>> isometric_parameter_list_;
		std::vector<std::vector<float>>  diff_x_list_;
		std::vector<std::vector<float>>  diff_y_list_;
		std::vector<std::vector<double>>  Cot_list_;

		std::vector<pointf2> texcoords;
	};
}

