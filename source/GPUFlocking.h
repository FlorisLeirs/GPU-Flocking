#pragma once
#include <GLFW/glfw3.h>

#pragma warning( push )
#pragma warning( disable : 4005 )
#include <CL/cl.hpp>
#pragma warning( pop )

class Camera;

class GPUFlocking
{
public:
	GPUFlocking(int windowWidth, int windowHeight)
		: m_Width(windowWidth), m_Height(windowHeight) {}

	~GPUFlocking();

	void Run();
	void Initialize();

private:
	int m_Width;
	int m_Height;

	GLFWwindow* m_pWindow = nullptr;
	cl::Program* m_pProgram = nullptr;
	Camera* m_pCamera;
};
