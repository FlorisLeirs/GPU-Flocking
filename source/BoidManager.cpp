#include "pch.h"
#include "BoidManager.h"
#include "Boid.h"
#include "Renderer.h"


void BoidManager::Initialize(int nrOfBoids, cl::Program* pProgram)
{
	for (int i{}; i != nrOfBoids; ++i)
	{
		m_pBoids.emplace_back(new Boid{ glm::vec3{0,0,0} });
	}
	m_pProgram = pProgram;
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

}

void BoidManager::Render() const
{
	for (auto pBoid : m_pBoids)
	{
		Renderer::GetInstance().RenderBoid(glm::mat4{});
	}

}
