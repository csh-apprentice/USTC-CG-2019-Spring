#pragma once

#include <Basic/HeapObj.h>
#include <Basic/HeapObj.h>
#include <UHEMesh/HEMesh.h>
#include <UGM/UGM>
#include <Engine/MeshEdit/MinSurf.h>
#include <Engine/MeshEdit/Boundary_Basic.h>
#include <Engine/MeshEdit/Boundary_Square.h>
#include <Engine/MeshEdit/Boundary_Circle.h>
#include <Engine/Primitive/TriMesh.h>
#include <Eigen/Sparse>


#define MAX 1e6
#define MIN 1e-6
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
		enum weight_type_
		{
			Uniform,
			KCotangent,
			Kdefault
		} my_weight_type_;
		enum display_status_
		{
			kon,
			koff
		}current_display_status_=koff;

		Boundary_Basic::Bounadary_type_ my_boundary_type_=Boundary_Basic::Bounadary_type_::Kdefault;
	public:
		void Clear();
		bool Init(Ptr<TriMesh> triMesh);
		
		bool Run();


		
	private:
		// kernel part of the algorithm
		void Para();

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
		static double Distance(V* v1, V* v2);
		static double vec_Cotangent(V* v1, V* v2, V* v3);
		static double Cos_V(V* v1, V* v2, V* v3);

		//set type
		void Set_Boundary_type_to_Square();
		void Set_Boundary_type_to_Circle();
		void Set_weight_type_to_Uniform();
		void Set_weight_type_to_Cotangent();
		void Turn_on_display();

		//set the parameter
		void get_boundary_index_();
		void Set_Boundary_();
		void Set_Big_Matrix_();
		void Set_Big_Matrix_Uniform_();
		void Set_Big_matrix_Cotangent_();
		void Solve();

	private:
		friend class ASAP;
		Ptr<TriMesh> triMesh;
		const Ptr<HEMesh<V>> heMesh; // vertice order is same with triMesh
		std::vector<T> big_matrix_coff_;
		SpMat big_matrix_;
		Eigen::VectorXd right_vector_;
		Eigen::VectorXd right_vector_x_;
		Eigen::VectorXd right_vector_y_;
		Eigen::VectorXd V_x_;
		Eigen::VectorXd V_y_;
		//Eigen::VectorXd V_z_;
		solver_LU big_solver;

		Ptr<Boundary_Basic> current_boundary_;
		std::vector<pointf2>Boundary_vertex_;
		std::vector<size_t>Boundary_index_list_;

		std::vector<pointf2> texcoords;
	};
}

