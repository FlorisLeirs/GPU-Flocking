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

	const float m_NeighbourRadius = 40.f;

	float m_SeperationWeigth = 0.65f;
	float m_CohesionWeigth = 0.45f;
	float m_AllignmentWeigth = 0.4f;
};
