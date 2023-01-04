#include "pch.h"
#include "GPUFlocking.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include "BoidManager.h"
#include "Camera.h"
#include "Renderer.h"




GPUFlocking::~GPUFlocking()
{
	Renderer::GetInstance().Destroy();
	BoidManager::GetInstance().Destroy();

	glfwDestroyWindow(m_pWindow);
	glfwTerminate();
}

void GPUFlocking::Run()
{
	Initialize();

	auto lastTime = std::chrono::high_resolution_clock::now();
	//loop
	while (!glfwWindowShouldClose(m_pWindow))
	{
		
		const auto currentTime = std::chrono::high_resolution_clock::now();
		const float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();

		
		BoidManager::GetInstance().Update(deltaTime);
		
		m_pCamera->Inputs(m_pWindow);

		Renderer::GetInstance().Render(m_pWindow);
		glfwPollEvents();
		lastTime = currentTime;
	}

}

void GPUFlocking::Initialize()
{
	srand (time(NULL));

	glfwInit();
	//Setup version and profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Create window
	m_pWindow = glfwCreateWindow(m_Width, m_Height, "GPUFlocking", NULL, NULL);
	if (m_pWindow == nullptr)
	{
		std::cout << "Window invalid" << std::endl;
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(m_pWindow);


	m_pCamera = new Camera(m_Width, m_Height, glm::vec3(0.f, 0.f, 50.f));



	//OpenCL
	// Create platform
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	auto platform = platforms.front();

	//Create device
	std::vector<cl::Device> devices;
	platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
	cl::Device device = devices.front();

	std::ifstream kernelFile{ "Flocking.cl" };
	std::string srcStr{ std::istreambuf_iterator<char>(kernelFile), std::istreambuf_iterator<char>() };

	cl::Program::Sources sources(1, std::make_pair(srcStr.c_str(), srcStr.length() + 1));
	cl::Context context(device);
	cl::Program program(context, sources);
	cl_int err = 0;
	err = program.build();
	std::string info;
	program.getBuildInfo(device, CL_PROGRAM_BUILD_LOG, &info);
	std::cout << info << std::endl;


	Renderer::GetInstance().Initialize(m_pWindow, m_pCamera);
	BoidManager::GetInstance().Initialize(5000, &program);
}
