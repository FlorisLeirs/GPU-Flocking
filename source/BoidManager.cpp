#include "pch.h"
#include "BoidManager.h"

#include <iostream>
#include <glm/gtx/transform.hpp>
#include "Renderer.h"
#include "imgui.h"
#include "Camera.h"

void BoidManager::Initialize(int nrOfBoids, cl::Program* pProgram)
{
	m_NrOfBoids = nrOfBoids;

	float x, y, z;
	glm::vec3 forward{ 1.f,0.f,0.f };

	for (int i{}; i != nrOfBoids; ++i)
	{
		// randomize position and velocity
		x = rand() % static_cast<int>(m_BoxSize * 2) - m_BoxSize;
		y = rand() % static_cast<int>(m_BoxSize * 2) - m_BoxSize;
		z = rand() % static_cast<int>(m_BoxSize * 2) - m_BoxSize;

		forward.x = rand() % static_cast<int>(m_BoxSize * 2) - m_BoxSize;
		forward.y = rand() % static_cast<int>(m_BoxSize * 2)- m_BoxSize;
		forward.z = rand() % static_cast<int>(m_BoxSize * 2)- m_BoxSize;
		forward = normalize(forward) * 10.f;
		m_CurrentVelocities.push_back(forward.x);
		m_CurrentVelocities.push_back(forward.y);
		m_CurrentVelocities.push_back(forward.z);

		m_PrevPositions.push_back(x);
		m_PrevPositions.push_back(y);
		m_PrevPositions.push_back(z);

		m_Tranforms.resize(16 * nrOfBoids, 0.f);
	}

	m_pProgram = pProgram;
	SetUpOpenCL();


}

void BoidManager::Destroy()
{
	cl::finish();
}

void BoidManager::Update(float deltaTime)
{
	cl_int err = 0;
	//setup events
	std::vector<cl::Event> events{};
	cl::Event flockingEvent{};
	cl::Event transformEvent;

	// Only change if necessary
	if (m_ChangeWeight)
	{
		std::vector<float> weights{ m_CohesionWeigth, m_AllignmentWeigth, m_SeperationWeigth, m_WanderWeight };
		err = m_Queue.enqueueWriteBuffer(m_WeightsBuf, CL_FALSE, 0, sizeof(float) * weights.size(), weights.data());
	}
	if (m_ChangeSpeed)
		err = m_Kernel.setArg(5, m_MaxSpeed);
	if (m_ChangeNeighbourhoodSize)
		err = m_Kernel.setArg(6, m_NeighbourRadius);

	// change random-values and deltaTime
	std::vector<UINT> randomVector{ static_cast<unsigned>(std::rand()), static_cast<unsigned>(std::rand()) };
	err = m_Queue.enqueueWriteBuffer(m_RandomsBuf, CL_TRUE, 0, sizeof(UINT) * randomVector.size(), randomVector.data());
	err = m_Kernel.setArg(3, deltaTime);
	// run kernel
	err = m_Queue.enqueueNDRangeKernel(m_Kernel, cl::NullRange, cl::NDRange(m_NrOfBoids), cl::NullRange, &events, &flockingEvent);
	events.push_back(flockingEvent);
	// read new transforms back to cpu
	err = m_Queue.enqueueReadBuffer(m_TransformBuf, CL_TRUE, 0, sizeof(float) * m_Tranforms.size(),
		m_Tranforms.data(), NULL, &transformEvent);

	err = transformEvent.wait(); // wait until transform buffer is read by host

}

void BoidManager::Render() const
{
	Renderer::GetInstance().RenderBoids(m_Tranforms, m_NrOfBoids);
}

void BoidManager::UpdateUI(const int width)
{
	const float menuWidth = 350.f;
	const float menuHeight = 400.f;
	bool open = true;
	ImGui::SetNextWindowPos(ImVec2(static_cast<float>(width) - menuWidth - 5, 5));
	ImGui::SetNextWindowSize(ImVec2(menuWidth, menuHeight));
	ImGui::Begin("GPU Flocking", &open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	ImGui::PushAllowKeyboardFocus(false);

	ImGui::SliderFloat("Cohesion", &m_CohesionWeigth, 0.0f, 1.0f);
	ImGui::SliderFloat("Allignment", &m_AllignmentWeigth, 0.0f, 1.0f);
	ImGui::SliderFloat("Seperation", &m_SeperationWeigth, 0.0f, 1.0f);
	ImGui::SliderFloat("Wander", &m_WanderWeight, 0.1f, 1.0f);
	ImGui::Spacing();
	m_ChangeWeight = ImGui::Button("Save Weight");
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Spacing();
	ImGui::SliderFloat("Speed", &m_MaxSpeed, 0.f, 40.f);
	m_ChangeSpeed = ImGui::Button("Save Speed");
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Spacing();
	ImGui::SliderFloat("Neighbourhood", &m_NeighbourRadius, 5.f, 40.f);
	m_ChangeNeighbourhoodSize = ImGui::Button("Save Neighbourhood Size");
	ImGui::Text("Values need to be saved to take effect");

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::PopAllowKeyboardFocus();
	ImGui::End();
}

void BoidManager::SetUpOpenCL()
{
	m_Context = m_pProgram->getInfo<CL_PROGRAM_CONTEXT>();
	auto devices = m_Context.getInfo<CL_CONTEXT_DEVICES>();
	m_Device = devices.front();
	m_Kernel = cl::Kernel(*m_pProgram, "Flocking");
	m_Queue = cl::CommandQueue(m_Context, m_Device);

	//Create buffers
	cl_int err = 0;

	m_TransformBuf = cl::Buffer(
		m_Context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_ALLOC_HOST_PTR | CL_MEM_COPY_HOST_PTR,
		sizeof(float) * m_Tranforms.size(), m_Tranforms.data(), &err);

	m_PrevPosBuf = cl::Buffer(
		m_Context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_ALLOC_HOST_PTR | CL_MEM_COPY_HOST_PTR,
		sizeof(float) * m_PrevPositions.size(), m_PrevPositions.data(), &err);

	m_VelocityBuf = cl::Buffer(
		m_Context, CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS | CL_MEM_ALLOC_HOST_PTR | CL_MEM_COPY_HOST_PTR,
		sizeof(float) * m_CurrentVelocities.size(), m_CurrentVelocities.data(), &err);

	std::vector<float> weights{ m_CohesionWeigth, m_AllignmentWeigth, m_SeperationWeigth, m_WanderWeight };
	m_WeightsBuf = cl::Buffer(m_Context, CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR | CL_MEM_COPY_HOST_PTR,
		sizeof(float) * weights.size(), weights.data(), &err);

	m_TimeBuf = cl::Buffer(
		m_Context, CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR | CL_MEM_COPY_HOST_PTR,
		sizeof(float), &m_DeltaTime, &err);

	std::vector<UINT> randomVector{ static_cast<unsigned>(std::rand()), static_cast<unsigned>(std::rand()) };
	m_RandomsBuf = cl::Buffer(m_Context, CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR | CL_MEM_COPY_HOST_PTR,
		sizeof(UINT) * randomVector.size(), randomVector.data(), &err);

	//Set kernel arguments
	err = m_Kernel.setArg(0, m_PrevPosBuf);
	err = m_Kernel.setArg(1, m_VelocityBuf);
	err = m_Kernel.setArg(2, m_WeightsBuf);
	err = m_Kernel.setArg(3, m_TimeBuf);
	err = m_Kernel.setArg(4, m_RandomsBuf);
	err = m_Kernel.setArg(5, m_MaxSpeed);
	err = m_Kernel.setArg(6, m_NeighbourRadius);
	err = m_Kernel.setArg(7, m_BoxSize);
	err = m_Kernel.setArg(8, m_TransformBuf);

}
