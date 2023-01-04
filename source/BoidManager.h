﻿#pragma once
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
private:
	int m_NrOfBoids;

	cl::Program* m_pProgram;
	cl::Context m_Context;
	cl::Device m_Device;
	cl::Kernel m_Kernel;
	cl::CommandQueue m_Queue;
	cl::Buffer m_TransformBuf;
	cl::Buffer m_PrevPosBuf;
	cl::Buffer m_VelocityBuf;
	cl::Buffer m_WeightsBuf;
	cl::Buffer m_TimeBuf;
	cl::Buffer m_RandomsBuf;
	cl::Buffer m_NeighbourhoodSizeBuf;
	cl::Buffer m_DebugBuf;

	float m_NeighbourRadius = 20.f;
	float m_MaxSpeed = 1.f;
	float m_DeltaTime{};
	
	float m_SeperationWeigth = 0.4f;
	float m_CohesionWeigth = 0.45f;
	float m_AllignmentWeigth = 0.4f;
	float m_WanderWeight = 0.35f;

	glm::vec3 m_MinPos{-50.f, -50.f, -50.f};// x, y and z min spawn positions
	glm::vec3 m_MaxPos{50.f, 50.f, 50.f};// x, y and z max spawn positions

	std::vector<float> m_Tranforms{};
	std::vector<float> m_PrevPositions{};
	std::vector<float> m_CurrentVelocities{};

	bool m_ChangeSettings{false};


	void SetUpOpenCL();

};
