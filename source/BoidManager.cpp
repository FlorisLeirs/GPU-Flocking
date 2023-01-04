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
		m_Tranforms.push_back(m_pBoids.back()->GetTransform()[0][1]);
		m_Tranforms.push_back(m_pBoids.back()->GetTransform()[0][2]);
		m_Tranforms.push_back(m_pBoids.back()->GetTransform()[0][3]);

		m_Tranforms.push_back(m_pBoids.back()->GetTransform()[1][0]);
		m_Tranforms.push_back(m_pBoids.back()->GetTransform()[1][1]);
		m_Tranforms.push_back(m_pBoids.back()->GetTransform()[1][2]);
		m_Tranforms.push_back(m_pBoids.back()->GetTransform()[1][3]);

		m_Tranforms.push_back(m_pBoids.back()->GetTransform()[2][0]);
		m_Tranforms.push_back(m_pBoids.back()->GetTransform()[2][1]);
		m_Tranforms.push_back(m_pBoids.back()->GetTransform()[2][2]);
		m_Tranforms.push_back(m_pBoids.back()->GetTransform()[2][3]);

		m_Tranforms.push_back(m_pBoids.back()->GetTransform()[3][0]);
		m_Tranforms.push_back(m_pBoids.back()->GetTransform()[3][1]);
		m_Tranforms.push_back(m_pBoids.back()->GetTransform()[3][2]);
		m_Tranforms.push_back(m_pBoids.back()->GetTransform()[3][3]);
		//std::cout << "x: " << std::to_string(x) << "\ny: " << std::to_string(y) << "\nz: " << std::to_string(z) << std::endl;
	}

	m_pProgram = pProgram;
	SetUpOpenCL();


}



void BoidManager::Destroy()
{
	for (auto pBoid : m_pBoids)
	{
		delete pBoid;
		pBoid = nullptr;
	}

	cl::finish();
}

void BoidManager::Update(float deltaTime)
{
	cl_int err = 0;
	
	std::vector<cl::Event> events{};
	cl::Event timeEvent{};
	cl::Event flockingEvent{};
	cl::Event transformEvent{};

	err = m_Queue.enqueueWriteBuffer(m_TimeBuf, CL_TRUE, 0, sizeof(float), &deltaTime, NULL, &timeEvent);
	events.push_back(timeEvent);
	err = m_Queue.enqueueNDRangeKernel(m_Kernel, cl::NullRange, cl::NDRange(m_pBoids.size()), cl::NullRange, &events, &flockingEvent);
	events.push_back(flockingEvent);
	err = m_Queue.enqueueReadBuffer(m_TransformBuf, CL_TRUE, 0, sizeof(float) * m_Tranforms.size(),
		m_Tranforms.data());


	//debug
	//std::vector<int> debugVector(m_pBoids.size());
	//err = m_Queue.enqueueReadBuffer(m_DebugBuf, CL_FALSE, 0, sizeof(int) * m_pBoids.size(),
	//	debugVector.data());

	//err = transformEvent.wait();//wait until transform buffer is read by host

	for (int i{}; i != m_pBoids.size(); ++i)
	{
		m_pBoids[i]->SetTransform(glm::mat4(
			m_Tranforms[i * 16], m_Tranforms[i * 16 + 1], m_Tranforms[i * 16 + 2], m_Tranforms[i * 16 + 3], // [0]
			m_Tranforms[i * 16 + 4], m_Tranforms[i * 16 + 5], m_Tranforms[i * 16 + 6], m_Tranforms[i * 16 + 7], // [1]
			m_Tranforms[i * 16 + 8], m_Tranforms[i * 16 + 9], m_Tranforms[i * 16 + 10], m_Tranforms[i * 16 + 11], // [2]
			m_Tranforms[i * 16 + 12], m_Tranforms[i * 16 + 13], m_Tranforms[i * 16 + 14], m_Tranforms[i * 16 + 15])); // [3]
	}
}

void BoidManager::Render() const
{
	for (auto pBoid : m_pBoids)
	{
		pBoid->Render();
	}

}

void BoidManager::SetUpOpenCL()
{
	m_Context = m_pProgram->getInfo<CL_PROGRAM_CONTEXT>();
	auto devices = m_Context.getInfo<CL_CONTEXT_DEVICES>();
	m_Device = devices.front();
	m_Kernel = cl::Kernel(*m_pProgram, "Flocking");

	//Create buffers
	cl_int err = 0;
	m_TransformBuf = cl::Buffer(
		m_Context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_ALLOC_HOST_PTR | CL_MEM_COPY_HOST_PTR,
		sizeof(float) * m_Tranforms.size(), m_Tranforms.data(), &err);

	m_PrevPosBuf = cl::Buffer(
		m_Context, CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS | CL_MEM_ALLOC_HOST_PTR | CL_MEM_COPY_HOST_PTR,
		sizeof(float) * m_PrevPositions.size(), m_PrevPositions.data(), &err);

	m_VelocityBuf = cl::Buffer(
		m_Context, CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS | CL_MEM_ALLOC_HOST_PTR | CL_MEM_COPY_HOST_PTR,
		sizeof(float) * m_CurrentVelocities.size(), m_CurrentVelocities.data(), &err);

	std::vector<float> weights{ m_CohesionWeigth, m_AllignmentWeigth, m_SeperationWeigth };
	m_WeightsBuf = cl::Buffer(m_Context, CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR | CL_MEM_COPY_HOST_PTR,
		sizeof(float) * weights.size(), weights.data(), &err);

	m_TimeBuf = cl::Buffer(
		m_Context, CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR | CL_MEM_COPY_HOST_PTR,
		sizeof(float), &m_DeltaTime, &err);

	m_MaxSpeedBuf = cl::Buffer(m_Context, CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR | CL_MEM_COPY_HOST_PTR,
		sizeof(float), &m_MaxSpeed, &err);

	m_NeighbourhoodSizeBuf = cl::Buffer(m_Context, CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR | CL_MEM_COPY_HOST_PTR,
		sizeof(float), &m_NeighbourRadius, &err);


	//m_DebugBuf = cl::Buffer(m_Context, CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR | CL_MEM_COPY_HOST_PTR | CL_MEM_HOST_READ_ONLY,
	//	sizeof(int) * m_pBoids.size());

	//Set kernel arguments
	err = m_Kernel.setArg(0, m_TransformBuf);
	err = m_Kernel.setArg(1, m_PrevPosBuf);
	err = m_Kernel.setArg(2, m_VelocityBuf);
	err = m_Kernel.setArg(3, m_WeightsBuf);
	err = m_Kernel.setArg(4, m_TimeBuf);
	err = m_Kernel.setArg(5, m_MaxSpeedBuf);
	err = m_Kernel.setArg(6, m_NeighbourhoodSizeBuf);
	//err = m_Kernel.setArg(7, m_DebugBuf);

	m_Queue = cl::CommandQueue(m_Context, m_Device);
}
