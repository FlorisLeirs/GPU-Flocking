#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

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
};
