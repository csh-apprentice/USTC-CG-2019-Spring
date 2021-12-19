#pragma once

#include <Basic/HeapObj.h>
//#include <Engine/Primitive/MassSpring.h>
#include <UGM/UGM>
#include <Eigen/Eigen>
#include <math.h>


using namespace Eigen;

typedef Eigen::SparseLU<Eigen::SparseMatrix<double>> solver_LU;
typedef Eigen::SparseMatrix<double> SpMat;
typedef Eigen::Triplet<double> T;

namespace Ubpa {
	class Simulate : public HeapObj {

	public:
		Simulate(const std::vector<pointf3>& plist,
			const std::vector<unsigned>& elist) {
			edgelist = elist;
			this->positions.resize(plist.size());
			for (int i = 0; i < plist.size(); i++)
			{
				for (int j = 0; j < 3; j++)
				{
					this->positions[i][j] = plist[i][j];
				}
			}
		};
	public:enum class Simulatetype
	{
		explict_Euler,
		Liu_accelerate

	};

	public:
		static const Ptr<Simulate> New(const std::vector<pointf3>& plist,
			const std::vector<unsigned>& elist) {
			return Ubpa::New<Simulate>(plist, elist);
		}
	public:
		// clear cache data
		void Clear();

		// init cache data (eg. half-edge structure) for Run()
		bool Init();
		//bool Init();

		// call it after Init()
		bool Run();

		const std::vector<pointf3>& GetPositions() const { return positions; };

		const float GetStiff() { return stiff; };
		void SetStiff(float k) { stiff = k; Init(); };
		const float GetTimeStep() { return h; };
		void SetTimeStep(float k) { h = k; Init(); };
		std::vector<unsigned>& GetFix() { return this->fixed_id; };
		void SetFix(const std::vector<unsigned>& f) { this->fixed_id = f; Init(); };
		const std::vector<pointf3>& GetVelocity() { return velocity; };
		//void SetVelocity(const std::vector<pointf3>& v) { velocity = v; };

		void SetLeftFix();
		bool If_fixed(int index);                      //判断index点是否被固定住了


		//set the simulate type of the given stl/tet
		void set_type_Euler();
		void set_type_Accer();

		//two ways of the operation of the simulation
		void Euler_Simu();
		void Accer_Simu();

	private:
		// kernel part of the algorithm
		void SimulateOnce();

		//Euler
		//void update_fint();               //更新此时每个质点收到的内力情况
		VectorXd init_y();              //返回初始化的迭代值x(0)
		VectorXd get_gxk(VectorXd xk);              //返回该迭代时刻的gxk

		//Accer
		void Init_Accer();                      //初始化相关的L，J与solver的相关信息，并对矩阵进行预分解
		void Init_position_vec();               //初始化位置列向量
		void RemoveRow(Eigen::MatrixXd& matrix, unsigned int rowToRemove);  //删除矩阵的某一行
		void RemoveRow_Vector(Eigen::VectorXd& matrix, unsigned int rowToRemove);  //删除列向量的某一行

	private:
		float h = 0.03f;  //步长
		float stiff = 1e5;     //弹簧的弹性系数
		double max_error = 1e-5;       //欧拉隐式迭代时允许的最大误差
		size_t iteration_num = 4;          //最小迭代次数设置
		std::vector<unsigned> fixed_id;  //fixed point id


		//mesh data
		std::vector<unsigned> edgelist;


		//simulation data
		std::vector<pointf3> positions;
		std::vector<pointf3> velocity;
		std::vector<double> nature_length_list;
		std::vector<vecf3> fext;                            //存储每个质点受到的外力
		std::vector<vecf3> fint;                             //存储每个质点受到的内力（实时更新）
		//SpMat grad_force;                                    //存储有关力的梯度的矩阵的有关信息
		MatrixXd M;                                          //质点组的质量矩阵

		//欧拉隐式迭代
		//MatrixXd gxk;                                       //迭代矩阵
		VectorXd x_position;                                  //质点位置的位置信息矩阵
		VectorXd fext_Eigen;                                 //质点组所受外力矩阵
		VectorXd fint_Eigen;                                 //质点组所受内力矩阵
		VectorXd y;                                           //readme中迭代的外值
		//vector<T> grad_force_list;


		//Liu加速方法
		MatrixXd L;                                          //论文方法中的L矩阵
		MatrixXd J;                                          //论文方法中的J矩阵
		MatrixXd I3;                                         //3*3的单位矩阵
		MatrixXd K;                                            //考虑边界限制条件后的矩阵K
		VectorXd position_vector;                                //位置列向量

		solver_LU solver_global;                              //固定所有的d值时确定与分解矩阵（global步骤）
		std::vector<double> current_length_list;
		VectorXd spring_vector;                         //在加速方法的local步骤中存储忧患之后的d信息


		//simulate type
		Simulatetype type_;
	};
}



