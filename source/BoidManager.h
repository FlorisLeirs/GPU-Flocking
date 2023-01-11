#pragma once
#pragma warning( push )
#pragma warning( disable : 4005 )
#include <CL/cl.hpp>
#pragma warning( pop )

#include "Singleton.h"

class BoidManager : public Singleton<BoidManager>
{
public:
	void Initialize(int nrOfBoids, cl::Program* pProgram);
	void Destroy();

	void Update(float deltaTime);
	void Render() const;

	void UpdateUI(const int width);
private:
	int m_NrOfBoids;

	cl::Program* m_pProgram;
	cl::Context m_Context;
	cl::Device m_Device;
	cl::Kernel m_FlockingKernel;
	cl::Kernel m_DataKernel;
	cl::CommandQueue m_Queue;

	cl::Buffer m_TransformBuf;
	cl::Buffer m_PrevPosBuf;
	cl::Buffer m_PrevVelocityBuf;
	cl::Buffer m_CurrVelocityBuf;
	cl::Buffer m_WeightsBuf;
	cl::Buffer m_RandomsBuf;


	float m_NeighbourRadius = 20.f;
	float m_MaxSpeed = 15.0f;
	float m_DeltaTime{};
	
	float m_SeperationWeigth = 0.5f;
	float m_CohesionWeigth = 0.5f;
	float m_AllignmentWeigth = 0.5f;
	float m_WanderWeight = 0.5f;

	float m_BoxSize{150.f};

	std::vector<float> m_Tranforms{};
	std::vector<float> m_PrevPositions{};
	std::vector<float> m_CurrentVelocities{};

	bool m_ChangeSpeed{false};
	bool m_ChangeNeighbourhoodSize{false};

	void SetUpOpenCL();
};
