#include <Engine/MeshEdit/Simulate.h>


#include <Eigen/Sparse>

using namespace Ubpa;

using namespace std;
using namespace Eigen;


void Simulate::Clear() {
	this->positions.clear();
	this->velocity.clear();
	//this->fixed_id.clear();
}

bool Simulate::Init() {
	//Clear();
	std::cout << "Initializiation start." << std::endl;
	int n = positions.size();
	int s = edgelist.size() / 2;

	current_length_list.resize(s);
	spring_vector.resize(3 * s);
	//fext.resize(n);
	for (int i = 0;i < n;i++)
		fext.push_back(vecf3(9.8 / 1.414, 0, 9.8 / 1.414));     //设置质点的重力加速度
		//fext.push_back( vecf3(0, -9.8, 0));     //设置质点的重力加速度

	fext_Eigen.resize(3 * n);
	for (int i = 0;i < n;i++)
		for (int j = 0;j < 3;j++)
			fext_Eigen[3 * i + j] = fext[i][j];


	fint.resize(n, vecf3(0, 0, 0));              //设置质点受到的初始内力
	M = MatrixXd::Zero(3 * n, 3 * n);
	for (int i = 0;i < 3 * n;i++)
		M(i, i) = 1;


	this->velocity.resize(positions.size());
	for (int i = 0; i < positions.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			this->velocity[i][j] = 0;
		}
	}
	std::cout << "positions size is" << positions.size() << "edge size is " << s << std::endl;
	for (int i = 0;i < s;i++)
		nature_length_list.push_back((positions[edgelist[2 * i]] - positions[edgelist[2 * i + 1]]).norm());      //设置每一段弹簧的初始长度


	//for (int i = 0;i < nature_length_list.size();i++)
		//cout << "nature_length_list" << i << "is " << nature_length_list[i] << endl;
	size_t size = positions.size();
	y = VectorXd::Zero(3 * size);

	cout << "debug 1" << endl;
	Init_Accer();

	//TODO
	std::cout << "Initializiation done." << std::endl;
	return true;
}

bool Simulate::Run() {
	SimulateOnce();

	// half-edge structure -> triangle mesh

	return true;
}

void Ubpa::Simulate::SetLeftFix()
{
	//固定网格x坐标最小点
	fixed_id.clear();
	double x = 100000;
	for (int i = 0; i < positions.size(); i++)
	{
		if (positions[i][0] < x)
		{
			x = positions[i][0];
		}
	}

	for (int i = 0; i < positions.size(); i++)
	{
		if (abs(positions[i][0] - x) < 1e-5)
		{
			fixed_id.push_back(i);
		}
	}

	sort(fixed_id.begin(), fixed_id.end());                //将固定点从小到大排序

	Init();
}

void Simulate::SimulateOnce() {
	// TODO
	//cout << "WARNING::Simulate::SimulateOnce:" << endl;
//		<< "\t" << "not implemented" << endl;
	//type_ = Simulatetype::Liu_accelerate;
	switch (type_)
	{
	case Simulatetype::explict_Euler:
		/*for (int row = 0; row < positions.size(); row++)
		{

			this->velocity[row][0] += -h * 0.1f;
			this->positions[row][0] += h * this->velocity[row][0];

		}*/
		Euler_Simu();

		break;
	case Simulatetype::Liu_accelerate:
		//TODO
		Accer_Simu();
		break;
	}
}


void Simulate::set_type_Euler()
{
	type_ = Simulatetype::explict_Euler;
}


void Simulate::set_type_Accer()
{
	type_ = Simulatetype::Liu_accelerate;
}

bool Simulate::If_fixed(int index)
{
	for (auto temp : fixed_id)
		if (index == temp)
			return true;
	return false;
}

void Simulate::Euler_Simu()
{


	assert(type_ == Simulatetype::explict_Euler);
	int n = positions.size();
	int s = edgelist.size() / 2;
	x_position = init_y();
	size_t iter = 0;
	double error = 0;
	vector<T> grad_force_list;
	SpMat grad_force;                                    //存储有关力的梯度的矩阵的有关信息
	grad_force.resize(3 * n, 3 * n);


	do
	{
		VectorXd gxk;
		//cout << "debug2" << endl;
		grad_force.setZero();
		grad_force_list.clear();

		//内力参数初始化
		for (int i = 0;i < n;i++)
			fint[i] = vecf3(0, 0, 0);



		//设置内力参数
		for (int i = 0;i < s;i++)
		{
			pointf3 x1 = pointf3(x_position[3 * edgelist[2 * i]], x_position[3 * edgelist[2 * i] + 1], x_position[3 * edgelist[2 * i] + 2]);
			pointf3 x2 = pointf3(x_position[3 * edgelist[2 * i + 1]], x_position[3 * edgelist[2 * i + 1] + 1], x_position[3 * edgelist[2 * i + 1] + 2]);
			Vector3d spring = Vector3d(x1[0] - x2[0], x1[1] - x2[1], x1[2] - x2[2]);
			double current_length = (x1 - x2).norm();
			fint[edgelist[2 * i]] += vecf3((stiff * (current_length - nature_length_list[i]) / current_length) * (x2 - x1));
			fint[edgelist[2 * i + 1]] -= vecf3((stiff * (current_length - nature_length_list[i]) / current_length) * (x2 - x1));
		}


		for (int i = 0;i < s;i++)
		{

			MatrixXd df1x1 = MatrixXd::Zero(3, 3);
			auto x1 = positions[edgelist[2 * i]];
			auto x2 = positions[edgelist[2 * i + 1]];
			Vector3d spring(x1[0] - x2[0], x1[1] - x2[1], x1[2] - x2[2]);
			double current_length = (x1 - x2).norm();

			df1x1 += MatrixXd::Identity(3, 3) * stiff * (nature_length_list[i] / current_length - 1);
			df1x1 -= stiff * nature_length_list[i] / pow(current_length, 3) * spring * spring.transpose();
			//cout << "debug3" << endl;
			//grad_force_list
			if (!If_fixed(edgelist[2 * i]))
				for (int row = 0;row < 3;row++)
					for (int col = 0;col < 3;col++)
					{
						//cout << "debug4" << endl;
						grad_force_list.push_back(T(3 * edgelist[2 * i] + row, 3 * edgelist[2 * i] + col, -pow(h, 2) * df1x1(row, col)));
						grad_force_list.push_back(T(3 * edgelist[2 * i] + row, 3 * edgelist[2 * i + 1] + col, pow(h, 2) * df1x1(row, col)));
					}



			if (!If_fixed(edgelist[2 * i + 1]))
				for (int row = 0;row < 3;row++)
					for (int col = 0;col < 3;col++)
					{
						grad_force_list.push_back(T(3 * edgelist[2 * i + 1] + row, 3 * edgelist[2 * i + 1] + col, -pow(h, 2) * df1x1(row, col)));
						grad_force_list.push_back(T(3 * edgelist[2 * i + 1] + row, 3 * edgelist[2 * i] + col, pow(h, 2) * df1x1(row, col)));
					}



		}

		for (int i = 0;i < n;i++)
			for (int j = 0;j < 3;j++)
				grad_force_list.push_back(T(3 * i + j, 3 * i + j, 1));


		//grad_force
		grad_force.setFromTriplets(grad_force_list.begin(), grad_force_list.end());

		solver_LU solver;
		solver.compute(grad_force);
		assert(solver.info() == Success);      //9月17日工作日志：断言失败！

		gxk = get_gxk(x_position);
		VectorXd delta = solver.solve(gxk);
		assert(solver.info() == Success);
		error = fabs(delta(0));
		for (int i = 0;i < 3 * n;i++)
			if (fabs(delta(i) > error))
				error = fabs(delta(i));
		x_position -= delta;

		iter++;
		//error
	} while (max_error < error && iter < iteration_num);

	//std::cout << "the error is " << error << std::endl;
	for (int i = 0;i < n;i++)
		if (!If_fixed(i))
			for (int j = 0;j < 3;j++)
			{
				velocity[i][j] = (float(x_position(3 * i + j)) - positions[i][j]) / h;
				positions[i][j] = float(x_position(3 * i + j));
			}

}

VectorXd Simulate::init_y()
{

	size_t size = positions.size();
	y = VectorXd::Zero(3 * size);
	for (size_t i = 0; i < size; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (If_fixed(i))
			{
				y(3 * i + j) = positions[i][j];
			}
			else
			{
				y(3 * i + j) = positions[i][j] + h * velocity[i][j] + h * h * fext[i][j];
			}
		}
	}

	return y;

}



VectorXd Simulate::get_gxk(VectorXd xk)
{
	int n = positions.size();
	VectorXd fint_Eigen;
	fint_Eigen.resize(3 * n);

	for (int i = 0;i < n;i++)
		for (int j = 0;j < 3;j++)
			fint_Eigen(3 * i + j) = fint[i][j];


	VectorXd result = M * (xk - y) - pow(h, 2) * fint_Eigen;

	for (auto fix_index : fixed_id)
		for (int j = 0;j < 3;j++)
			result(3 * fix_index + j) = 0;

	return result;

}

void Simulate::Init_Accer()
{
	I3 = MatrixXd::Identity(3, 3);

	int n = positions.size();
	int s = edgelist.size() / 2;
	int m = n - fixed_id.size();


	//初始化K
	int num = 0;
	K = MatrixXd::Identity(3 * n, 3 * n);
	for (int i = 0;i < fixed_id.size();i++)
		for (int j = 0;j < 3;j++)
		{
			RemoveRow(K, 3 * fixed_id[i] + j-num);
			num++;
			//RemoveRow(K, 3 * fixed_id[i+1] + j);
		}

	//cout << "debug 2" << endl;
	//初始化L
	MatrixXd left_L = MatrixXd::Zero(n, n);
	L = MatrixXd::Zero(3 * n, 3 * n);

	//L的左式
	for (int i = 0;i < s;i++)
	{
		VectorXd Ai = VectorXd::Zero(n);
		size_t i1 = edgelist[2 * i];
		size_t i2 = edgelist[2 * i + 1];
		Ai(i1) = 1;
		Ai(i2) = -1;
		left_L += stiff * Ai * Ai.transpose();
	}

	//克氏积
	for (int i = 0;i < n;i++)
		for (int j = 0;j < n;j++)
			L.block(3 * i, 3 * j, 3, 3) = I3 * left_L(i, j);


	//初始化J
	MatrixXd left_J = MatrixXd::Zero(n, s);
	J = MatrixXd::Zero(3 * n, 3 * s);
	//J的左式
	for (int i = 0;i < s;i++)
	{
		VectorXd Ai = VectorXd::Zero(n);
		size_t i1 = edgelist[2 * i];
		size_t i2 = edgelist[2 * i + 1];
		Ai(i1) = 1;
		Ai(i2) = -1;
		VectorXd Si = VectorXd::Zero(s);
		Si(i) = 1;
		left_J += stiff * Ai * Si.transpose();
	}

	//克氏积
	for (int i = 0;i < n;i++)
		for (int j = 0;j < s;j++)
			J.block(3 * i, 3 * j, 3, 3) = I3 * left_J(i, j);
	//cout << "debug 3" << endl;

	MatrixXd global = K * (M + pow(h, 2) * L) * K.transpose();

	//从稠密阵转换为稀疏阵

	SpMat global_sparse(3 * m, 3 * m);

	vector<T> global_trans;
	for (int i = 0;i < 3 * m;i++)
		for (int j = 0;j < 3 * m;j++)
			if (global(i, j) != 0)
				global_trans.push_back(T(i, j, global(i, j)));
	//cout << "debug 4" << endl;

	global_sparse.setFromTriplets(global_trans.begin(), global_trans.end());
	solver_global.compute(global_sparse);
	assert(solver_global.info() == Success);


	cout << "the none zero num of the global_sparse is " << global_sparse.nonZeros() << endl;
	cout << "Init Accer part done" << endl;

}

void Simulate::Accer_Simu()
{
	//cout << K.block(0, 0, 50, 50) << endl;

	int n = positions.size();
	int s = edgelist.size() / 2;
	int m = n - fixed_id.size();

	//初始化b
	VectorXd b(3 * n);                                           //位移常量
	Init_position_vec();
	b = position_vector - K.transpose() * K * position_vector;
	//for (int i = 0;i < 10;i++)
		//cout << "the parameter b " << i << "is " << b[i] << endl;

	//初始化y

	VectorXd x_position = VectorXd::Zero(3 * n);
	x_position = init_y();
	VectorXd xf_ = x_position;
	int count = 0;
	for (int i = 0;i < fixed_id.size();i++)
		for (int j = 0;j < 3;j++)
			RemoveRow_Vector(xf_, 3 * fixed_id[i] + j-count++);


	for (int i = 0;i < s;i++)
	{
		vecf3 pi1(x_position(3 * edgelist[2 * i]), x_position(3 * edgelist[2 * i] + 1), x_position(3 * edgelist[2 * i] + 2));
		vecf3 pi2(x_position(3 * edgelist[2 * i + 1]), x_position(3 * edgelist[2 * i + 1] + 1), x_position(3 * edgelist[2 * i + 1] + 2));
		current_length_list[i] = (pi1 - pi2).norm();
		for (int j = 0;j < 3;j++)
			spring_vector[3 * i + j] = (nature_length_list[i] * (pi1 - pi2) / current_length_list[i])[j];
	}



	for (int iter = 0;iter < 10;iter++)                 //local_global收敛次数先假定为10次
	{


		//global
		VectorXd RHS = K * (pow(h, 2) * J * spring_vector + M * y - (M + pow(h, 2) * L) * b);
		xf_ = solver_global.solve(RHS);
		assert(solver_global.info() == Success);      //9月17日工作日志：断言失败！

		int num = 0;
		for (int i = 0;i < n;i++)
			for (int j = 0;j < 3;j++)
				if (!If_fixed(i))
					x_position[3 * i + j] = xf_[num++];
		assert(num == 3 * m);


		//local

		//fix x

		for (int i = 0;i < s;i++)
		{
			vecf3 pi1(x_position(3 * edgelist[2 * i]), x_position(3 * edgelist[2 * i] + 1), x_position(3 * edgelist[2 * i] + 2));
			vecf3 pi2(x_position(3 * edgelist[2 * i + 1]), x_position(3 * edgelist[2 * i + 1] + 1), x_position(3 * edgelist[2 * i + 1] + 2));
			current_length_list[i] = (pi1 - pi2).norm();
			for (int j = 0;j < 3;j++)
				spring_vector[3 * i + j] = (nature_length_list[i] * (pi1 - pi2) / current_length_list[i])[j];
		}

	}

	int num = 0;
	for (int i = 0;i < n;i++)
		for (int j = 0;j < 3;j++)
			if (!If_fixed(i))
			{
				velocity[i][j] = (float(xf_(num)) - positions[i][j]) / h;
				positions[i][j] = xf_[num++];
			}
	assert(num == 3 * m);
}

void Simulate::RemoveRow(Eigen::MatrixXd& matrix, unsigned int rowToRemove) {
	unsigned int numRows = matrix.rows() - 1;
	unsigned int numCols = matrix.cols();

	if (rowToRemove < numRows) {
		matrix.block(rowToRemove, 0, numRows - rowToRemove, numCols) =
			matrix.block(rowToRemove + 1, 0, numRows - rowToRemove, numCols);
	}

	matrix.conservativeResize(numRows, numCols);
}

void Simulate::RemoveRow_Vector(Eigen::VectorXd& matrix, unsigned int rowToRemove) {
	unsigned int numRows = matrix.rows() - 1;
	unsigned int numCols = matrix.cols();

	if (rowToRemove < numRows) {
		matrix.block(rowToRemove, 0, numRows - rowToRemove, numCols) =
			matrix.block(rowToRemove + 1, 0, numRows - rowToRemove, numCols);
	}

	matrix.conservativeResize(numRows, numCols);
}

void Simulate::Init_position_vec()
{

	size_t size = positions.size();
	position_vector = VectorXd::Zero(3 * size);
	for (size_t i = 0; i < size; i++)
		for (int j = 0; j < 3; j++)
			position_vector(3 * i + j) = positions[i][j];

}