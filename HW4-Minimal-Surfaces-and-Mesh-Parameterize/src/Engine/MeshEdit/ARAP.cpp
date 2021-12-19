#include <Engine/MeshEdit/ARAP.h>
using namespace Ubpa;

using namespace std;

ARAP::ARAP(Ptr<TriMesh> triMesh)
	:heMesh(make_shared<HEMesh<V>>())
{

	Init(triMesh);

}

void ARAP::Clear() {
	// TODO
	heMesh->Clear();
	triMesh = nullptr;
}

bool ARAP::Init(Ptr<TriMesh> triMesh) {
	// TODO
	Clear();

	if (triMesh == nullptr)
		return true;

	if (triMesh->GetType() == TriMesh::INVALID) {
		printf("ERROR::ARAP::Init:\n"
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
		printf("ERROR::ARAP::Init:\n"
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

bool ARAP::Run() {

	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::ARAP::Run\n"
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

void ARAP::ASAP_exe_()
{
	std::cout << "the exe debug 1" << std::endl;
	flatten();
	std::cout << "the exe debug 2" << std::endl;
	get_anchor_point_index_();
	std::cout << "the exe debug 3" << std::endl;
	initiate_local_matrix_list();
	std::cout << "the exe debug 4" << std::endl;
	Set_Big_Matrix_();
	std::cout << "the exe debug 5" << std::endl;
	for (int i = 0;i < iteration_num_;i++)
	{
		std::cout << "local " << i << std::endl;
		Local_update_();
		std::cout << "global " << i << std::endl;
		Global_update_();
	}
	//TODO
	std::cout << "the exe debug 6" << std::endl;
	Upload_Para_();
	std::cout << "the exe debug 7" << std::endl;
}

double ARAP::Distance(V* v1, V* v2)
{
	return sqrt((v1->pos.at(0) - v2->pos.at(0)) * (v1->pos.at(0) - v2->pos.at(0)) + (v1->pos.at(1) - v2->pos.at(1)) * (v1->pos.at(1) - v2->pos.at(1)) +
		(v1->pos.at(2) - v2->pos.at(2)) * (v1->pos.at(2) - v2->pos.at(2)));
}

double ARAP::Cos_V(V* v0, V* v1, V* v2)
{
	double edge1 = Distance(v0, v1);
	double edge2 = Distance(v1, v2);
	double edge3 = Distance(v0, v2);
	return (edge1 * edge1 + edge2 * edge2 - edge3 * edge3) / (2 * edge1 * edge2);
}

double ARAP::vec_Cotangent(V* v1, V* v2, V* v3)
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



void ARAP::flatten()
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
		pointf2 current_third_point = pointf2(second_side_ * signed_cos_, second_side_ * sqrt(1 - pow(signed_cos_, 2)));
		isometric_parameter_list_.push_back({ current_first_point,current_second_point,current_third_point });
		diff_x_list_.push_back({ (current_first_point - current_second_point)[0], (current_second_point - current_third_point)[0], (current_third_point - current_first_point)[0] });
		diff_y_list_.push_back({ (current_first_point - current_second_point)[1], (current_second_point - current_third_point)[1], (current_third_point - current_first_point)[1] });
		//diff_ux_list_.push_back({ (first->pos - second->pos)[0], (second->pos - third->pos)[0], (third->pos - first->pos)[0] });
		//diff_uy_list_.push_back({ (first->pos - second->pos)[1], (second->pos - third->pos)[1], (third->pos - first->pos)[1] });
		diff_ux_list_.push_back({ (current_first_point - current_second_point)[0], (current_second_point - current_third_point)[0], (current_third_point - current_first_point)[0] });
		diff_uy_list_.push_back({ (current_first_point - current_second_point)[1], (current_second_point - current_third_point)[1], (current_third_point - current_first_point)[1] });

		Cot_list_.push_back({ vec_Cotangent(second,first,third),vec_Cotangent(first,second,third),vec_Cotangent(second,third,first) });
	}
	//for (int i = 0;i < 10;i++)
	//{
	//std:;cout << "the isometric_parameter_list_[" << i << "] is " << isometric_parameter_list_[i][0] << "," << isometric_parameter_list_[i][1] << "," << isometric_parameter_list_[i][2] << std::endl;
	//}

}

void ARAP::get_anchor_point_index_()
{
	auto anchor_point_1 = heMesh->Boundaries()[0][0]->Origin();
	size_t half_index = heMesh->Boundaries()[0].size() / 2;
	auto anchor_point_2 = heMesh->Boundaries()[0][half_index % heMesh->Boundaries()[0].size()]->Origin();
	anchor_point_index_1 = heMesh->Index(anchor_point_1);
	anchor_point_index_2 = heMesh->Index(anchor_point_2);
	std::cout << "the first anchor point is " << anchor_point_index_1 << std::endl;
	std::cout << "the second anchor point is " << anchor_point_index_2 << std::endl;
}


void ARAP::Set_Big_Matrix_()
{
	size_t T_ = heMesh->NumPolygons();
	size_t N = heMesh->NumVertices();
	auto tri_list_ = heMesh->Polygons();
	big_matrix_ = SpMat(N,N);
	auto Vec_list_ = heMesh->Vertices();
	for (int i = 0;i < N;i++)
	{
		if (i != anchor_point_index_1 && i != anchor_point_index_2)
		{
			auto adjedges = Vec_list_[i]->OutHEs();
			int size = adjedges.size();
			for (int j = 0;j < size;j++)
			{
				auto neighbor_tri_right = adjedges[j]->Polygon();
				int neighbor_V_index = heMesh->Index(adjedges[j]->End());

				if (neighbor_tri_right != nullptr)
				{
					int tri_index_ = heMesh->Index(neighbor_tri_right);
					int index_right_ = 0;
					for (int index = 0;index < 3;index++)
					{
						assert(Vec_list_[i]->pos != adjedges[j]->End()->pos);
						if (neighbor_tri_right->BoundaryVertice()[index]->pos != Vec_list_[i]->pos && neighbor_tri_right->BoundaryVertice()[index]->pos != adjedges[j]->End()->pos)
							index_right_ = index;
					}
					big_matrix_coff_.push_back(T(i, i, Cot_list_[tri_index_][index_right_]));
					big_matrix_coff_.push_back(T(i, neighbor_V_index, -Cot_list_[tri_index_][index_right_]));
				}

				auto neighbor_tri_left = adjedges[j]->Pair()->Polygon();
				if (neighbor_tri_left != nullptr)
				{
					int tri_index_ = heMesh->Index(neighbor_tri_left);
					int index_left_ = 0;
					for (int index = 0;index < 3;index++)
					{
						assert(Vec_list_[i]->pos != adjedges[j]->End()->pos);
						if (neighbor_tri_left->BoundaryVertice()[index]->pos != Vec_list_[i]->pos && neighbor_tri_left->BoundaryVertice()[index]->pos != adjedges[j]->End()->pos)
							index_left_ = index;
					}
					big_matrix_coff_.push_back(T(i, i, Cot_list_[tri_index_][index_left_]));
					big_matrix_coff_.push_back(T(i, neighbor_V_index, -Cot_list_[tri_index_][index_left_]));
				}

			}
		}

		else
		{
			big_matrix_coff_.push_back(T(i, i, 1));
			//big_matrix_coff_.push_back(T(k + N, k + N, 1));
		}
	}
	big_matrix_.setFromTriplets(big_matrix_coff_.begin(), big_matrix_coff_.end());
	big_solver.compute(big_matrix_);
	if (big_solver.info() != Eigen::Success) {
		std::cout << "decomposition failed!" << std::endl;
		return;
	}
}

void ARAP::initiate_local_matrix_list()
{
	size_t T_ = heMesh->NumPolygons();
	Local_matrix_list= std::vector<Eigen::Matrix2d>(T_);
}

void ARAP::Local_update_()
{
	size_t T_ = heMesh->NumPolygons();
	size_t N = heMesh->NumVertices();
	auto polygon_list = heMesh->Polygons();
	
	for (int t = 0;t < T_;t++)
	{
		Eigen::Matrix2d Stu = Eigen::Matrix2d::Zero();
		for (int i = 0;i < 3;i++)
		{
        Eigen::Vector2d diff_u;
	    Eigen::Vector2d diff_x;
		diff_x << diff_x_list_[t][i], diff_y_list_[t][i] ;
		diff_u << diff_ux_list_[t][i], diff_uy_list_[t][i];
		Stu += Cot_list_[t][(i + 2) % 3] * diff_u * diff_x.transpose();
		}	
		Eigen::JacobiSVD<Eigen::Matrix2d> svd(Stu, Eigen::DecompositionOptions::ComputeFullU | Eigen::DecompositionOptions::ComputeFullV);
	if (Stu.determinant() > 0)
	{
		Local_matrix_list[t]=(svd.matrixU() * svd.matrixV().transpose());
	}
	else
	{
		Eigen::Matrix2d sign_matrix_;
		sign_matrix_ << 1., 0, 0, -1.;
		Local_matrix_list[t]= (svd.matrixU() *sign_matrix_* svd.matrixV().transpose());
		//Local_matrix_list[t] = (svd.matrixU() * svd.matrixV().transpose());
	}
	}
	
}


void ARAP::Global_update_()
{
	size_t T_ = heMesh->NumPolygons();
	size_t N = heMesh->NumVertices();
	auto tri_list_ = heMesh->Polygons();
	//big_matrix_ = SpMat(N, N);
	auto Vec_list_ = heMesh->Vertices();
	
	Eigen::VectorXd right_vector_temp_x_= Eigen::VectorXd::Zero(N);
	Eigen::VectorXd right_vector_temp_y_ = Eigen::VectorXd::Zero(N);
	//std::cout << "a test debug location " << std::endl;
	for (int i = 0;i < N;i++)
	{
		
		
		if (i != anchor_point_index_1 && i != anchor_point_index_2)
		{
			
			//std::cout << "a test debug location " << std::endl;
			auto adjedges = Vec_list_[i]->OutHEs();
			int size = adjedges.size();
			for (int j = 0;j < size;j++)
			{
				
				auto neighbor_tri_right = adjedges[j]->Polygon();
				//int neighbor_V_index = heMesh->Index(adjedges[j]->End());
				//std::cout << "a test debug right location 0" << std::endl;
				if (neighbor_tri_right != nullptr)
				{   
					//std::cout << "a test debug location right 1" << std::endl;
					int tri_index_ = heMesh->Index(neighbor_tri_right);
					//std::cout << "a test debug location right 2" << std::endl;
					int index_right_ = 0;
					int index_origin_ = 0;
					int index_end_ = 0;
					//std::cout << "a test debug location right 3" << std::endl;
					for (int index = 0;index < 3;index++)
					{
						assert(Vec_list_[i]->pos != adjedges[j]->End()->pos);
						if (neighbor_tri_right->BoundaryVertice()[index]->pos != Vec_list_[i]->pos && neighbor_tri_right->BoundaryVertice()[index]->pos != adjedges[j]->End()->pos)
							index_right_ = index;
						if (neighbor_tri_right->BoundaryVertice()[index]->pos == Vec_list_[i]->pos)
							index_origin_ = index;
						if (neighbor_tri_right->BoundaryVertice()[index]->pos == adjedges[j]->End()->pos)
							index_end_ = index;
					}
					//std::cout << "a test debug location 3" << std::endl;
					Eigen::Vector2d matrix_temp = Eigen::Vector2d::Zero();
					matrix_temp = Cot_list_[tri_index_][index_right_] * Local_matrix_list[tri_index_] * Eigen::Vector2d(isometric_parameter_list_[tri_index_][index_origin_][0]- isometric_parameter_list_[tri_index_][index_end_][0], isometric_parameter_list_[tri_index_][index_origin_][1] - isometric_parameter_list_[tri_index_][index_end_][1]);
					right_vector_temp_x_[i] += matrix_temp[0];
					right_vector_temp_y_[i] += matrix_temp[1];
					//std::cout << "a test debug location 4" << std::endl;
				}

				auto neighbor_tri_left = adjedges[j]->Pair()->Polygon();
				//std::cout << "a test debug left location 0" << std::endl;
				if (neighbor_tri_left != nullptr)
				{
					//std::cout << "a test debug location left 1" << std::endl;
					int tri_index_ = heMesh->Index(neighbor_tri_left);
					int index_left_ = 0;
					int index_origin_ = 0;
					int index_end_ = 0;
					//std::cout << "a test debug location left 2" << std::endl;
					for (int index = 0;index < 3;index++)
					{
						 assert(Vec_list_[i]->pos != adjedges[j]->End()->pos);
						if (neighbor_tri_left->BoundaryVertice()[index]->pos != Vec_list_[i]->pos && neighbor_tri_left->BoundaryVertice()[index]->pos != adjedges[j]->End()->pos)
							index_left_ = index;
						if (neighbor_tri_left->BoundaryVertice()[index]->pos == Vec_list_[i]->pos)
							index_origin_ = index;
						if (neighbor_tri_left->BoundaryVertice()[index]->pos == adjedges[j]->End()->pos)
							index_end_ = index;
					}
					//std::cout << "a test debug location left 3" << std::endl;
					Eigen::Vector2d matrix_temp = Eigen::Vector2d::Zero();
					matrix_temp = Cot_list_[tri_index_][index_left_] * Local_matrix_list[tri_index_] * Eigen::Vector2d(Eigen::Vector2d(isometric_parameter_list_[tri_index_][index_origin_][0] - isometric_parameter_list_[tri_index_][index_end_][0], isometric_parameter_list_[tri_index_][index_origin_][1] - isometric_parameter_list_[tri_index_][index_end_][1]));
					right_vector_temp_x_[i] += matrix_temp[0];
					right_vector_temp_y_[i] += matrix_temp[1];
					//std::cout << "a test debug location left 4" << std::endl;
				}

			}
		}

		else if(i==anchor_point_index_1)
		{
			//std::cout << "a test debug location 1" << std::endl;
			right_vector_temp_x_[i] = 0.;
			right_vector_temp_y_[i] = 0.;
			//big_matrix_coff_.push_back(T(k + N, k + N, 1));
		}
		else
		{
			//std::cout << "a test debug location 2" << std::endl;
			right_vector_temp_x_[i] = 1.;
			right_vector_temp_y_[i] = 1.;
		}
	}
	
	V_result_x_ = big_solver.solve(right_vector_temp_x_);
	V_result_y_ = big_solver.solve(right_vector_temp_y_);
	//std::cout<<"a test debug location "<<std::endl;
	reset_para_();

}

void ARAP::reset_para_()
{
	size_t NP = heMesh->NumPolygons();
	size_t N = heMesh->NumVertices();
	auto tri_list_ = heMesh->Polygons();

	for (int i = 0;i < NP;i++)
	{
		
		auto first_index = heMesh->Index(tri_list_[i]->BoundaryVertice()[0]);
		auto second_index = heMesh->Index(tri_list_[i]->BoundaryVertice()[1]);
		auto third_index = heMesh->Index(tri_list_[i]->BoundaryVertice()[2]);
		diff_ux_list_[i]={ V_result_x_[first_index]-V_result_x_[second_index],  V_result_x_[second_index] - V_result_x_[third_index], V_result_x_[third_index] - V_result_x_[first_index] };
		diff_uy_list_[i] = { V_result_y_[first_index] - V_result_y_[second_index],  V_result_y_[second_index] - V_result_y_[third_index], V_result_y_[third_index] - V_result_y_[first_index] };

		//9.6 剩余工作：重设置Cotlist 参数：待完成
		//auto first = new V(*heMesh->Vertices()[first_index]);
		//auto second = new V(*heMesh->Vertices()[second_index]);
		//auto third = new V(*heMesh->Vertices()[third_index]);
		//auto first = make_shared<V>(*heMesh->Vertices()[first_index]);

		 //Cot_list_[i]={ vec_Cotangent(second,first,third),vec_Cotangent(first,second,third),vec_Cotangent(second,third,first) };

		 //delete first;
		// delete second;
		 //delete third;
	}
}
void  ARAP::set_iteration_num_()
{
//TODO
}



void ARAP::Upload_Para_()
{
	size_t NP = heMesh->NumPolygons();
	size_t N = heMesh->NumVertices();
	auto tri_list_ = heMesh->Polygons();
	for (size_t i = 0; i < N; i++)
	{
		if (current_display_status_ == koff)
		{
			heMesh->Vertices()[i]->pos.at(0) = V_result_x_[i];
			heMesh->Vertices()[i]->pos.at(1) = V_result_y_[i];
			heMesh->Vertices()[i]->pos.at(2) = 0.;
		}
		texcoords.push_back(pointf2(V_result_x_[i], V_result_y_[i ]));
	}

}