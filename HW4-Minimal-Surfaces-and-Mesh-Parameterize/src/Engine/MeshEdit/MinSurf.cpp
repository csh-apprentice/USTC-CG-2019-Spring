#include <Engine/MeshEdit/MinSurf.h>

#include <Engine/Primitive/TriMesh.h>

#include <Eigen/Sparse>

using namespace Ubpa;

using namespace std;
using namespace Eigen;

MinSurf::MinSurf(Ptr<TriMesh> triMesh)
	: heMesh(make_shared<HEMesh<V>>())
{
	Init(triMesh);
}

void MinSurf::Clear() {
	heMesh->Clear();
	triMesh = nullptr;
}

bool MinSurf::Init(Ptr<TriMesh> triMesh) {
	Clear();

	if (triMesh == nullptr)
		return true;

	if (triMesh->GetType() == TriMesh::INVALID) {
		printf("ERROR::MinSurf::Init:\n"
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
		printf("ERROR::MinSurf::Init:\n"
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

bool MinSurf::Run() {
	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::MinSurf::Run\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}

	Minimize();

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
	return true;
}

void MinSurf::Minimize() {
	// TODO
	size_t nV = heMesh->NumVertices();
	big_matrix_ = SpMat(nV, nV);
	Eigen::VectorXd right_vector_x_ = Eigen::VectorXd::Zero(nV);
	Eigen::VectorXd right_vector_y_ = Eigen::VectorXd::Zero(nV);
	Eigen::VectorXd right_vector_z_ = Eigen::VectorXd::Zero(nV);
	for(int i=0;i<nV;i++)
	{
		auto current_V_ = heMesh->Vertices()[i];
		if (current_V_->IsBoundary())
		{
			big_matrix_coff_.push_back(T(i,i, 1));
			right_vector_x_[i] = current_V_->pos[0];
			right_vector_y_[i] = current_V_->pos[1];
			right_vector_z_[i] = current_V_->pos[2];
		}
		else
		{
			big_matrix_coff_.push_back(T(i,i, current_V_->Degree()));

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
V_x_ = big_solver.solve(right_vector_x_);
V_y_ = big_solver.solve(right_vector_y_);
V_z_ = big_solver.solve(right_vector_z_);

for (size_t i = 0; i < nV; i++)
{
	heMesh->Vertices()[i]->pos.at(0) = V_x_[i];
	heMesh->Vertices()[i]->pos.at(1) = V_y_(i);
	heMesh->Vertices()[i]->pos.at(2) = V_z_(i);
}
}
