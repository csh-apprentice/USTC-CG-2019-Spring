#include <Engine/MeshEdit/Paramaterize.h>


using namespace Ubpa;

using namespace std;

Paramaterize::Paramaterize(Ptr<TriMesh> triMesh)
	:heMesh(make_shared<HEMesh<V>>())
{
	// TODO
	//std::cout << "debug a" << std::endl;
	Init(triMesh);
	//std::cout << "debug b" << std::endl;
	//cout << "Paramaterize::Paramaterize:" << endl
		//<< "\t" << "not implemented" << endl;
}

void Paramaterize::Clear() {
	// TODO
	heMesh->Clear();
	triMesh = nullptr;

	//cout << "Paramaterize::Clear:" << endl
	//	<< "\t" << "not implemented" << endl;
}

bool Paramaterize::Init(Ptr<TriMesh> triMesh) {
	// TODO
	Clear();

	if (triMesh == nullptr)
		return true;

	if (triMesh->GetType() == TriMesh::INVALID) {
		printf("ERROR::Paramaterize::Init:\n"
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
		printf("ERROR::Paramaterize::Init:\n"
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

bool Paramaterize::Run() {
	
	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::Paramaterize::Run\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}
	//std::cout << "debug 2" << std::endl;
	Para();

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
	triMesh->Update(texcoords);
	return true;
}

void Paramaterize::Para()
{
	//std::cout << "debug 1" << std::endl;
	get_boundary_index_();
	//for debug	
	//std::cout << "debug 2" << std::endl;
	Set_Boundary_();
	//std::cout << "debug 4" << std::endl;
	Set_Big_Matrix_();
	//std::cout << "debug 5" << std::endl;
	Solve();
	
}

void Paramaterize::Set_Boundary_type_to_Square()
{
	my_boundary_type_ = Boundary_Basic::Bounadary_type_::KSquare;
}
void Paramaterize::Set_Boundary_type_to_Circle()
{
	my_boundary_type_ = Boundary_Basic::Bounadary_type_::KCircle;
}
void Paramaterize::Set_weight_type_to_Uniform()
{
	my_weight_type_ = weight_type_::Uniform;
}
void Paramaterize::Set_weight_type_to_Cotangent()
{
	my_weight_type_ = weight_type_::KCotangent;

}

double Paramaterize::Distance(V* v1, V* v2)
{
	return sqrt((v1->pos.at(0) - v2->pos.at(0)) * (v1->pos.at(0) - v2->pos.at(0)) + (v1->pos.at(1) - v2->pos.at(1)) * (v1->pos.at(1) - v2->pos.at(1)) +
		(v1->pos.at(2) - v2->pos.at(2)) * (v1->pos.at(2) - v2->pos.at(2)));
}

double Paramaterize::Cos_V(V* v0, V* v1, V* v2)
{
	double edge1 = Distance(v0, v1);
	double edge2 = Distance(v1, v2);
	double edge3 = Distance(v0, v2);
	return (edge1 * edge1 + edge2 * edge2 - edge3 * edge3) / (2 * edge1 * edge2);
}

double Paramaterize::vec_Cotangent(V* v1, V* v2, V* v3)
{
	vecf3 v1v2= v1->pos - v2->pos;
	vecf3 v3v2 = v3->pos - v2->pos;
	double sin_V = v1v2.sin_theta(v3v2);
	if (sin_V >= 0 && sin_V < MIN)
		return MAX;
	else if (sin_V<0 && sin_V>-MIN)
		return -MAX;
	else
		return Cos_V(v1, v2, v3) / sin_V;

}

void Paramaterize::get_boundary_index_()
{
	size_t nB = heMesh->Boundaries()[0].size();
	for (int i = 0; i < nB; i++)
	{
		Boundary_index_list_.push_back(heMesh->Index(heMesh->Boundaries()[0][i]->Origin()));
	}

}

void Paramaterize::Set_Boundary_()
{
	switch (my_boundary_type_)
	{
	case Boundary_Basic::KSquare:
		current_boundary_ = make_shared<Boundary_Square>();
		current_boundary_->Set_Boundary(Boundary_index_list_, Boundary_vertex_);
		break;
	case Boundary_Basic::KCircle:
		 current_boundary_ = make_shared<Boundary_Circle>();
		 current_boundary_->Set_Boundary(Boundary_index_list_, Boundary_vertex_);
		break;
	case Boundary_Basic::Kdefault:
		break;
	}
}

void Paramaterize::Set_Big_Matrix_Uniform_()
{
	size_t nV = heMesh->NumVertices();
	big_matrix_ = SpMat(nV, nV);
	right_vector_x_ = Eigen::VectorXd::Zero(nV);
	right_vector_y_ = Eigen::VectorXd::Zero(nV);
	for (int i = 0;i < nV;i++)
	{
		auto current_V_ = heMesh->Vertices()[i];
		if (current_V_->IsBoundary())
		{
			big_matrix_coff_.push_back(T(i, i, 1));

		}
		else
		{
			big_matrix_coff_.push_back(T(i, i, current_V_->Degree()));

			for (auto adj : current_V_->AdjVertices())
			{
				big_matrix_coff_.push_back(T(i, heMesh->Index(adj), -1));
			}

		}
	}
	big_matrix_.setFromTriplets(big_matrix_coff_.begin(), big_matrix_coff_.end());
	big_solver.compute(big_matrix_);
	if (big_solver.info() != Eigen::Success) {
		std::cout << "decomposition failed!" << std::endl;
		return;
	}

	int BV = Boundary_index_list_.size();
	for (int i = 0;i < BV;i++)
	{
		int index = Boundary_index_list_[i];
		right_vector_x_[index] = Boundary_vertex_[i][0];
		right_vector_y_[index] = Boundary_vertex_[i][1];
	}

}

void Paramaterize::Set_Big_matrix_Cotangent_()
{
	size_t nV = heMesh->NumVertices();
	big_matrix_ = SpMat(nV, nV);
right_vector_x_ = Eigen::VectorXd::Zero(nV);
right_vector_y_ = Eigen::VectorXd::Zero(nV);

	for (int i = 0;i < nV;i++)
	{
		auto current_V_ = heMesh->Vertices()[i];
		if (current_V_->IsBoundary())
		{
			big_matrix_coff_.push_back(T(i, i, 1));
			
		}
		else
		{
			big_matrix_coff_.push_back(T(i, i, 1));

			int num_adj = current_V_->AdjVertices().size();
			double sum = 0;
			//get weight (sum)
			for (int j = 0;j < num_adj;j++)
			{
				//std::cout << "debug a" << std::endl;
				auto adj = current_V_->AdjVertices()[j];
				auto adj_pre_ = current_V_->AdjVertices()[(j - 1 + num_adj)%num_adj];
				auto adj_next_ = current_V_->AdjVertices()[(j + 1) % num_adj];
				//std::cout << "debug b" << std::endl;
				double weight = fabs(vec_Cotangent(current_V_, adj_pre_, adj)) +fabs(vec_Cotangent(current_V_, adj_next_, adj));
				sum += weight;
			}
			for (int j = 0;j < num_adj;j++)
			{
				auto adj = current_V_->AdjVertices()[j];
				auto adj_pre_ = current_V_->AdjVertices()[(j - 1 + num_adj) % num_adj];
				auto adj_next_ = current_V_->AdjVertices()[(j + 1) % num_adj];
				double weight = fabs(vec_Cotangent(current_V_, adj_pre_, adj)) + fabs(vec_Cotangent(current_V_, adj_next_, adj));
				big_matrix_coff_.push_back(T(i, heMesh->Index(adj), -weight / sum));
			}


		}
	}
	big_matrix_.setFromTriplets(big_matrix_coff_.begin(), big_matrix_coff_.end());
	big_solver.compute(big_matrix_);
	if (big_solver.info() != Eigen::Success) {
		std::cout << "decomposition failed!" << std::endl;
		return;
	}
	int BV = Boundary_index_list_.size();
	for (int i = 0;i < BV;i++)
	{
		int index = Boundary_index_list_[i];
		right_vector_x_[index] = Boundary_vertex_[i][0];
		right_vector_y_[index] = Boundary_vertex_[i][1];
	}

}

void Paramaterize::Solve()
{
	size_t N_x = heMesh->NumVertices();

	V_x_ = big_solver.solve(right_vector_x_);
	V_y_ = big_solver.solve(right_vector_y_);

	for (size_t i = 0; i < N_x; i++)
	{
		if(current_display_status_==koff)
		{
			heMesh->Vertices()[i]->pos.at(0) = V_x_[i];
		    heMesh->Vertices()[i]->pos.at(1) = V_y_(i);
		    heMesh->Vertices()[i]->pos.at(2) = 0.;
        }	
		    texcoords.push_back(pointf2(V_x_(i), V_y_(i)));		
	}
}
void Paramaterize::Set_Big_Matrix_()
{
	switch (my_weight_type_)
	{
	case Uniform:
		Set_Big_Matrix_Uniform_();
		break;
	case KCotangent:
		Set_Big_matrix_Cotangent_();
		break;
	case Kdefault:
		std::cout << "the weight type haven't been initiated yet!" << std::endl;
		break;
	}
}

void Paramaterize::Turn_on_display()
{
	current_display_status_ = kon;
}