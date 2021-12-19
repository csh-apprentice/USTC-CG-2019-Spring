#include <Engine/MeshEdit/ASAP.h>
using namespace Ubpa;

using namespace std;

ASAP::ASAP(Ptr<TriMesh> triMesh)
	:heMesh(make_shared<HEMesh<V>>())
{

	Init(triMesh);

}

void ASAP::Clear() {
	// TODO
	heMesh->Clear();
	triMesh = nullptr;
}

bool ASAP::Init(Ptr<TriMesh> triMesh) {
	// TODO
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

	// triangle mesh's positions ->  half-edge structure's positions
	for (int i = 0; i < nV; i++) {
		auto v = heMesh->Vertices().at(i);
		v->pos = triMesh->GetPositions()[i].cast_to<vecf3>();
	}

	this->triMesh = triMesh;
	return true;
}

bool ASAP::Run() {

	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::ASAP::Run\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}
	std::cout << "debug 1" << std::endl;
	ASAP_exe_();
	std::cout << "debug 2" << std::endl;
	// half-edge structure -> triangle mesh
	size_t nV = heMesh->NumVertices();
	size_t nF = heMesh->NumPolygons();
	vector<pointf3> positions;
	vector<unsigned> indice;
	positions.reserve(nV);
	indice.reserve(3 * nF);
	for (auto v : heMesh->Vertices())
		positions.push_back(v->pos.cast_to<pointf3>());
	for (auto f : heMesh->Polygons()) { // f is triangle
		for (auto v : f->BoundaryVertice()) // vertices of the triangle
			indice.push_back(static_cast<unsigned>(heMesh->Index(v)));
	}
	triMesh->Init(indice, positions);
	//triMesh->Update(texcoords);
	return true;
}

void ASAP::ASAP_exe_()
{
	std::cout << "the exe debug 1" << std::endl;
	flatten();
	get_anchor_point_index_();
	std::cout << "the exe debug 2" << std::endl;
	//the ERROR HERE
	Set_Big_Matrix_();
	std::cout << "the exe debug 3" << std::endl;
	Solve();
	std::cout << "the exe debug 4" << std::endl;
//TODO
}

double ASAP::Distance(V* v1, V* v2)
{
	return sqrt((v1->pos.at(0) - v2->pos.at(0)) * (v1->pos.at(0) - v2->pos.at(0)) + (v1->pos.at(1) - v2->pos.at(1)) * (v1->pos.at(1) - v2->pos.at(1)) +
		(v1->pos.at(2) - v2->pos.at(2)) * (v1->pos.at(2) - v2->pos.at(2)));
}

double ASAP::Cos_V(V* v0, V* v1, V* v2)
{
	double edge1 = Distance(v0, v1);
	double edge2 = Distance(v1, v2);
	double edge3 = Distance(v0, v2);
	return (edge1 * edge1 + edge2 * edge2 - edge3 * edge3) / (2 * edge1 * edge2);
}

double ASAP::vec_Cotangent(V* v1, V* v2, V* v3)
{
	vecf3 v1v2 = v1->pos - v2->pos;
	vecf3 v3v2 = v3->pos - v2->pos;
	double sin_V = v1v2.sin_theta(v3v2);
	if (sin_V >= 0 && sin_V < MIN)
		return MAX;
	else if (sin_V<0 && sin_V>-MIN)
		return -MAX;
	else
		return Cos_V(v1, v2, v3) / sin_V;

}



void ASAP::flatten()
{
	size_t NP = heMesh->NumPolygons();
	auto tri_list_ = heMesh->Polygons();
	for (int i = 0;i < NP;i++)
	{
		auto first = tri_list_[i]->BoundaryVertice()[0];
		auto second = tri_list_[i]->BoundaryVertice()[1];
		auto third = tri_list_[i]->BoundaryVertice()[2];	
		float third_side_ = (first->pos - second->pos).norm();
		float second_side_ = (first->pos - third->pos).norm();
		float signed_cos_ = Cos_V(third, first, second);
		pointf2 current_first_point = pointf2(0., 0.);
		pointf2 current_second_point = pointf2(third_side_, 0.);
		pointf2 current_third_point = pointf2(second_side_*signed_cos_, second_side_*sqrt(1-pow(signed_cos_,2)));
		isometric_parameter_list_.push_back({ current_first_point,current_second_point,current_third_point });
		diff_x_list_.push_back({ (current_first_point - current_second_point)[0], (current_second_point - current_third_point)[0], (current_third_point - current_first_point)[0] });
		diff_y_list_.push_back({ (current_first_point - current_second_point)[1], (current_second_point - current_third_point)[1], (current_third_point - current_first_point)[1] });
		Cot_list_.push_back({ vec_Cotangent(second,first,third),vec_Cotangent(first,second,third),vec_Cotangent(second,third,first) });
	}
	//for (int i = 0;i < 10;i++)
	//{
	//std:;cout << "the isometric_parameter_list_[" << i << "] is " << isometric_parameter_list_[i][0] << "," << isometric_parameter_list_[i][1] << "," << isometric_parameter_list_[i][2] << std::endl;
	//}

}

void ASAP::get_anchor_point_index_()
{
	auto anchor_point_1 = heMesh->Boundaries()[0][0]->Origin();
	size_t half_index = heMesh->Boundaries()[0].size()/2;
	auto anchor_point_2 = heMesh->Boundaries()[0][half_index % heMesh->Boundaries()[0].size()]->Origin();
	anchor_point_index_1 = heMesh->Index(anchor_point_1);
	anchor_point_index_2 = heMesh->Index(anchor_point_2);
	std::cout << "the first anchor point is " << anchor_point_index_1 << std::endl;
	std::cout << "the second anchor point is " << anchor_point_index_2 << std::endl;
}


void ASAP::Set_Big_Matrix_()
{
	size_t T_ = heMesh->NumPolygons();
	size_t N = heMesh->NumVertices();
	auto tri_list_ = heMesh->Polygons();
	size_t size = 2*(T_ + N);
	big_matrix_ = SpMat(size, size);
	auto Vec_list_ = heMesh->Vertices();

	//set u[k]--------------------------------------
	std::cout << "sey uk start" << std::endl;
	for (int k = 0;k < N;k++)
	{
		if (k != anchor_point_index_1 && k != anchor_point_index_2)
		{
			auto adjedges = Vec_list_[k]->OutHEs();
			int size = adjedges.size();
			//auto neighbor_tri_list_= Vec_list_[k]->AdjPolygons();		
			
				for (int j=0;j<size;j++)
				{
					//std::cout << "find start!" <<std::endl;

					auto neighbor_tri = adjedges[j]->Polygon();
					if (neighbor_tri != nullptr)
					{
						int tri_index_ = heMesh->Index(neighbor_tri);
						//std::cout << "find end!" << std::endl;
						for (int i = 0;i < 3;i++)
						{
							if (tri_list_[tri_index_]->BoundaryVertice()[i % 3]->pos == Vec_list_[k]->pos)
								continue;
							else
							{

								//set u[k],1-----------------------------------------

								double Cot_i2 = Cot_list_[tri_index_][(i + 2) % 3];
								double Cot_i1 = Cot_list_[tri_index_][(i + 1) % 3];

								size_t i_plus_0_index = heMesh->Index(tri_list_[tri_index_]->BoundaryVertice()[i % 3]);
								size_t i_plus_1_index = heMesh->Index(tri_list_[tri_index_]->BoundaryVertice()[(i + 1) % 3]);
								size_t i_plus_2_index = heMesh->Index(tri_list_[tri_index_]->BoundaryVertice()[(i + 2) % 3]);

								double diff_xi = diff_x_list_[tri_index_][i];
								double diff_yi = diff_y_list_[tri_index_][i];

								double diff_xi2 = diff_x_list_[tri_index_][(i + 2) % 3];
								double diff_yi2 = diff_y_list_[tri_index_][(i + 2) % 3];

								big_matrix_coff_.push_back(T(k, i_plus_0_index, Cot_i2));
								big_matrix_coff_.push_back(T(k, i_plus_1_index, -Cot_i2));
								big_matrix_coff_.push_back(T(k, (2 * N + 2 * (tri_index_)), -diff_xi * Cot_i2));
								big_matrix_coff_.push_back(T(k, (2 * N + 2 * (tri_index_)+1), -diff_yi * Cot_i2));

								big_matrix_coff_.push_back(T(k, i_plus_2_index, -Cot_i1));
								big_matrix_coff_.push_back(T(k, i_plus_0_index, Cot_i1));
								big_matrix_coff_.push_back(T(k, (2 * N + 2 * (tri_index_)), diff_xi2 * Cot_i1));
								big_matrix_coff_.push_back(T(k, (2 * N + 2 * (tri_index_)+1), diff_yi2 * Cot_i1));


								//set u[k],2-----------------------------------------------------

								big_matrix_coff_.push_back(T(k + N, i_plus_0_index+N, Cot_i2));
								big_matrix_coff_.push_back(T(k + N, i_plus_1_index+N, -Cot_i2));
								big_matrix_coff_.push_back(T(k + N, (2 * N + 2 * (tri_index_)), -diff_yi * Cot_i2));
								big_matrix_coff_.push_back(T(k + N, (2 * N + 2 * (tri_index_)+1), diff_xi * Cot_i2));

								big_matrix_coff_.push_back(T(k + N, i_plus_2_index+N, -Cot_i1));
								big_matrix_coff_.push_back(T(k + N, i_plus_0_index+N, Cot_i1));
								big_matrix_coff_.push_back(T(k + N, (2 * N + 2 * (tri_index_)), diff_yi2 * Cot_i1));
								big_matrix_coff_.push_back(T(k + N, (2 * N + 2 * (tri_index_)+1), -diff_xi2 * Cot_i1));
							}
						}
					}


				}
			
		}

		else
		{
			big_matrix_coff_.push_back(T(k, k, 1));
			big_matrix_coff_.push_back(T(k + N, k + N, 1));
		}

	}
	std::cout << "set at,bt start" << std::endl;

	//set ak.bk
	for (int t = 0;t < T_;t++)
	{

				for (int i = 0;i < 3;i++)
				{

						//set a[t],1-----------------------------------------

						double Cot_i2 = Cot_list_[t][(i + 2) % 3];
						double Cot_i1 = Cot_list_[t][(i + 1) % 3];
						double Cot_i0 = Cot_list_[t][i% 3];

						size_t i_plus_0_index = heMesh->Index(tri_list_[t]->BoundaryVertice()[i % 3]);
						size_t i_plus_1_index = heMesh->Index(tri_list_[t]->BoundaryVertice()[(i + 1) % 3]);
						size_t i_plus_2_index = heMesh->Index(tri_list_[t]->BoundaryVertice()[(i + 2) % 3]);

						double diff_xi = diff_x_list_[t][i];
						double diff_yi = diff_y_list_[t][i];

						big_matrix_coff_.push_back(T(2*(N+t), i_plus_0_index, -Cot_i2*diff_xi));
						big_matrix_coff_.push_back(T(2 * (N + t), i_plus_1_index, Cot_i2 * diff_xi));
						big_matrix_coff_.push_back(T(2 * (N + t), (2 * N + 2 * t), diff_xi * Cot_i2 * diff_xi));
						big_matrix_coff_.push_back(T(2 * (N + t), (2 * N + 2 * t+1), diff_yi * Cot_i2 * diff_xi));

						big_matrix_coff_.push_back(T(2 * (N + t), i_plus_0_index+N, -Cot_i2 * diff_yi));
						big_matrix_coff_.push_back(T(2 * (N + t), i_plus_1_index+N, Cot_i2 * diff_yi));
						big_matrix_coff_.push_back(T(2 * (N + t), (2 * N + 2 * t), diff_yi * Cot_i2 * diff_yi));
						big_matrix_coff_.push_back(T(2 * (N + t), (2 * N + 2 * t+1), -diff_xi * Cot_i2 * diff_yi));


						//set b[t]-----------------------------------------------------

						big_matrix_coff_.push_back(T(2 * (N + t)+1, i_plus_0_index, -Cot_i2 * diff_yi));
						big_matrix_coff_.push_back(T(2 * (N + t)+1, i_plus_1_index, Cot_i2* diff_yi));
						big_matrix_coff_.push_back(T(2 * (N + t)+1, (2 * N + 2 * t), diff_xi* Cot_i2* diff_yi));
						big_matrix_coff_.push_back(T(2 * (N + t)+1, (2 * N + 2 * t+1), diff_yi* Cot_i2* diff_yi));

						big_matrix_coff_.push_back(T(2 * (N + t)+1, i_plus_0_index+N, Cot_i2 * diff_xi));
						big_matrix_coff_.push_back(T(2 * (N + t)+1, i_plus_1_index+N, -Cot_i2* diff_xi));
						big_matrix_coff_.push_back(T(2 * (N + t)+1, (2 * N + 2 * t), -diff_yi* Cot_i2* diff_xi));
						big_matrix_coff_.push_back(T(2 * (N + t)+1, (2 * N + 2 * t+1), diff_xi * Cot_i2 * diff_xi));

					
				}

			

	}
	//std::cout << "final set" << std::endl;
	big_matrix_.setFromTriplets(big_matrix_coff_.begin(), big_matrix_coff_.end());
	big_solver.compute(big_matrix_);
	if (big_solver.info() != Eigen::Success) {
		std::cout << "decomposition failed!" << std::endl;
		return;
	}
}

void ASAP::Solve()
{
	size_t T_ = heMesh->NumPolygons();
	size_t N = heMesh->NumVertices();
	size_t size = 2 * (T_ + N);
	right_vector_all= Eigen::VectorXd::Zero(size);
	for (int k = 0;k < N;k++)
	{
		if (k == anchor_point_index_1)
		{
			right_vector_all[k] = 0.;
			right_vector_all[k + N] = 0.;
		}
		else if (k == anchor_point_index_2)
		{
			right_vector_all[k] = 1.;
			right_vector_all[k + N] = 1.;
		}
	}

	V_result_ = big_solver.solve(right_vector_all);
	for (size_t i = 0; i < N; i++)
	{
		if (current_display_status_ == koff)
		{
			heMesh->Vertices()[i]->pos.at(0) = V_result_[i];
			heMesh->Vertices()[i]->pos.at(1) = V_result_[i+N];
			heMesh->Vertices()[i]->pos.at(2) = 0.;
		}
		texcoords.push_back(pointf2(V_result_[i], V_result_[i + N]));
	}
	
}