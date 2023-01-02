#include "pch.h"
#include "BoidManager.h"

#include <iostream>

#include "Boid.h"
#include "Renderer.h"



void BoidManager::Initialize(int nrOfBoids, cl::Program* pProgram)
{
	float x, y, z;

	for (int i{}; i != nrOfBoids; ++i)
	{
		x = rand() % static_cast<int>(m_MaxPos.x - m_MinPos.x) + m_MinPos.x;
		y = rand() % static_cast<int>(m_MaxPos.y - m_MinPos.y) + m_MinPos.y;
		z = rand() % static_cast<int>(m_MaxPos.z - m_MinPos.z) + m_MinPos.z;
		m_pBoids.emplace_back(new Boid{ glm::vec3(x, y, z) });

		m_CurrentVelocities.push_back(m_pBoids.back()->GetForwardVector().x);
		m_CurrentVelocities.push_back(m_pBoids.back()->GetForwardVector().y);
		m_CurrentVelocities.push_back(m_pBoids.back()->GetForwardVector().z);

		m_PrevPositions.push_back(x);
		m_PrevPositions.push_back(y);
		m_PrevPositions.push_back(z);

		m_Tranforms.push_back(m_pBoids.back()->GetTransform()[0][0]);
		m_Tranforms.push_back(m_pBoids.back()->GetTransform()[1][0]);
		m_Tranforms.push_back(m_pBoids.back()->GetTransform()[2][0]);
		m_Tranforms.push_back(m_pBoids.back()->GetTransform()[3][0]);

		m_Tranforms.push_back(m_pBoids.back()->GetTransform()[0][1]);
		m_Tranforms.push_back(m_pBoids.back()->GetTransform()[1][1]);
		m_Tranforms.push_back(m_pBoids.back()->GetTransform()[2][1]);
		m_Tranforms.push_back(m_pBoids.back()->GetTransform()[3][1]);

		m_Tranforms.push_back(m_pBoids.back()->GetTransform()[0][2]);
		m_Tranforms.push_back(m_pBoids.back()->GetTransform()[1][2]);
		m_Tranforms.push_back(m_pBoids.back()->GetTransform()[2][2]);
		m_Tranforms.push_back(m_pBoids.back()->GetTransform()[3][2]);

		m_Tranforms.push_back(m_pBoids.back()->GetTransform()[0][3]);
		m_Tranforms.push_back(m_pBoids.back()->GetTransform()[1][3]);
		m_Tranforms.push_back(m_pBoids.back()->GetTransform()[2][3]);
		m_Tranforms.push_back(m_pBoids.back()->GetTransform()[3][3]);
		//std::cout << "x: " << std::to_string(x) << "\ny: " << std::to_string(y) << "\nz: " << std::to_string(z) << std::endl;
	}


	

	m_pProgram = pProgram;
	m_pContext = new cl::Context(m_pProgram->getInfo<CL_PROGRAM_CONTEXT>());
	auto devices = m_pContext->getInfo<CL_CONTEXT_DEVICES>();
	m_pDevice = &devices.front();
	m_pKernel = new cl::Kernel(*m_pProgram, "Flocking");


}



void BoidManager::Destroy()
{
	for (auto pBoid : m_pBoids)
	{
		delete pBoid;
		pBoid = nullptr;
	}

}

void BoidManager::Update(float deltaTime)
{
	cl::Buffer()
}

void BoidManager::Render() const
{
	for (auto pBoid : m_pBoids)
	{
		pBoid->Render();
	}

}
