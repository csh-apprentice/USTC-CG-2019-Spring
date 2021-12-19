#include <Engine/MeshEdit/Boundary_Square.h>
using namespace Ubpa;
Boundary_Square::Boundary_Square()
{
}

Boundary_Square::~Boundary_Square()
{
}

void Boundary_Square::Set_Boundary(std::vector<size_t> boundary_index, std::vector<pointf2>& boundary_list)
{
	int N_B_ = boundary_index.size();
	double stride = 4. / N_B_;
	for (size_t i = 0;i < N_B_;i++)
	{
		double temp = i * stride;
		if (0 <= temp && temp < 1)
			boundary_list.push_back(pointf2(0., temp));
		else if (1 <= temp && temp < 2)
			boundary_list.push_back(pointf2(temp - 1, 1.));
		else if (2 <= temp && temp < 3)
			boundary_list.push_back(pointf2(1., 3 - temp));
		else
			boundary_list.push_back(pointf2(4. - temp, 0.));
	}
}