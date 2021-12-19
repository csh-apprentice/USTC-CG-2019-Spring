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
		bool If_fixed(int index);                      //�ж�index���Ƿ񱻹̶�ס��


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
		//void update_fint();               //���´�ʱÿ���ʵ��յ����������
		VectorXd init_y();              //���س�ʼ���ĵ���ֵx(0)
		VectorXd get_gxk(VectorXd xk);              //���ظõ���ʱ�̵�gxk

		//Accer
		void Init_Accer();                      //��ʼ����ص�L��J��solver�������Ϣ�����Ծ������Ԥ�ֽ�
		void Init_position_vec();               //��ʼ��λ��������
		void RemoveRow(Eigen::MatrixXd& matrix, unsigned int rowToRemove);  //ɾ�������ĳһ��
		void RemoveRow_Vector(Eigen::VectorXd& matrix, unsigned int rowToRemove);  //ɾ����������ĳһ��

	private:
		float h = 0.03f;  //����
		float stiff = 1e5;     //���ɵĵ���ϵ��
		double max_error = 1e-5;       //ŷ����ʽ����ʱ�����������
		size_t iteration_num = 4;          //��С������������
		std::vector<unsigned> fixed_id;  //fixed point id


		//mesh data
		std::vector<unsigned> edgelist;


		//simulation data
		std::vector<pointf3> positions;
		std::vector<pointf3> velocity;
		std::vector<double> nature_length_list;
		std::vector<vecf3> fext;                            //�洢ÿ���ʵ��ܵ�������
		std::vector<vecf3> fint;                             //�洢ÿ���ʵ��ܵ���������ʵʱ���£�
		//SpMat grad_force;                                    //�洢�й������ݶȵľ�����й���Ϣ
		MatrixXd M;                                          //�ʵ������������

		//ŷ����ʽ����
		//MatrixXd gxk;                                       //��������
		VectorXd x_position;                                  //�ʵ�λ�õ�λ����Ϣ����
		VectorXd fext_Eigen;                                 //�ʵ���������������
		VectorXd fint_Eigen;                                 //�ʵ���������������
		VectorXd y;                                           //readme�е�������ֵ
		//vector<T> grad_force_list;


		//Liu���ٷ���
		MatrixXd L;                                          //���ķ����е�L����
		MatrixXd J;                                          //���ķ����е�J����
		MatrixXd I3;                                         //3*3�ĵ�λ����
		MatrixXd K;                                            //���Ǳ߽�����������ľ���K
		VectorXd position_vector;                                //λ��������

		solver_LU solver_global;                              //�̶����е�dֵʱȷ����ֽ����global���裩
		std::vector<double> current_length_list;
		VectorXd spring_vector;                         //�ڼ��ٷ�����local�����д洢�ǻ�֮���d��Ϣ


		//simulate type
		Simulatetype type_;
	};
}



