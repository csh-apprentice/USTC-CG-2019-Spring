#include <Engine/MeshEdit/Boundary_Circle.h>
using namespace Ubpa;
Boundary_Circle::Boundary_Circle()
{
}

Boundary_Circle::~Boundary_Circle()
{
}

void Boundary_Circle::Set_Boundary(std::vector<size_t> boundary_index, std::vector<pointf2>& boundary_list)
{
	int N_B_ = boundary_index.size();
	double stride = 2*Pi/ N_B_;
	for (size_t i = 0;i < N_B_;i++)
	{
		double temp = i * stride;
		boundary_list.push_back(pointf2(cos(temp), sin(temp)));
	}
}