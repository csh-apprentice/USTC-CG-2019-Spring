#pragma once
#pragma once
#include <Basic/HeapObj.h>
#include <Basic/HeapObj.h>
#include <UHEMesh/HEMesh.h>
#include <UGM/UGM>
#include <Engine/MeshEdit/Paramaterize.h>
#include <Engine/Primitive/TriMesh.h>
#include <Eigen/SVD>
#include <Engine/MeshEdit/Paramaterize.h>
#include <Eigen/Eigen>


//typedef Eigen::SimplicialLLT<Eigen::SparseMatrix<double>> solver_LLT;
typedef Eigen::SparseMatrix<double> SpMat;
typedef Eigen::Triplet<double> T;

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
		void Local_update_();
		void Global_update_();
		void set_iteration_num_();
		void initiate_local_matrix_list();

		//elenmental calculation
		static double Distance(V* v1, V* v2);
		static double vec_Cotangent(V* v1, V* v2, V* v3);
		static double Cos_V(V* v1, V* v2, V* v3);

		//set the parameter
		void reset_para_();
		void Set_Big_Matrix_();
		void Upload_Para_();
	private:
		Ptr<TriMesh> triMesh;
		const Ptr<HEMesh<V>> heMesh; // vertice order is same with triMesh
		std::vector<T> big_matrix_coff_;
		SpMat big_matrix_;
		solver_LU big_solver;
		Eigen::VectorXd right_vector_x;
		Eigen::VectorXd right_vector_y;
		Eigen::VectorXd V_result_x_;
		Eigen::VectorXd V_result_y_;

		size_t anchor_point_index_1;                            //set the first anchor point to (0,0)
		size_t anchor_point_index_2;                            //set the second anchor point to (1,1)
		size_t iteration_num_ = 10;

		std::vector<Eigen::Matrix2d> Local_matrix_list;
		std::vector<std::vector<pointf2>> isometric_parameter_list_;
		std::vector<std::vector<double>>  diff_x_list_;
		std::vector<std::vector<double>>  diff_y_list_;
		std::vector<std::vector<double>>  diff_ux_list_;
		std::vector<std::vector<double>>  diff_uy_list_;

		std::vector<std::vector<double>>  Cot_list_;

		std::vector<pointf2> texcoords;
	};
}
