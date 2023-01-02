#pragma once
#pragma warning( push )
#pragma warning( disable : 4005 )
#include <CL/cl.hpp>
#pragma warning( pop )

#include "Singleton.h"

class Boid;

class BoidManager : public Singleton<BoidManager>
{
public:
	void Initialize(int nrOfBoids, cl::Program* pProgram);
	void Destroy();

	void Update(float deltaTime);
	void Render() const;
private:
	std::vector<Boid*> m_pBoids;
	cl::Program* m_pProgram;
	cl::Context* m_pContext;
	cl::Device* m_pDevice;
	cl::Kernel* m_pKernel;

	const float m_NeighbourRadius = 40.f;

	float m_SeperationWeigth = 0.65f;
	float m_CohesionWeigth = 0.45f;
	float m_AllignmentWeigth = 0.4f;

	glm::vec3 m_MinPos{-50.f, -50.f, -50.f};// x, y and z min spawn positions
	glm::vec3 m_MaxPos{50.f, 50.f, 50.f};// x, y and z max spawn positions

	std::vector<float> m_Tranforms{};
	std::vector<float> m_PrevPositions{};
	std::vector<float> m_CurrentVelocities{};


};
