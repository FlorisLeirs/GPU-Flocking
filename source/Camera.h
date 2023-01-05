#pragma once
#include <GLFW/glfw3.h>

class Shader;

class Camera
{
public:
	Camera(int& width, int& height, glm::vec3 pos);

	void Matrix(float FOVdeg, float nearPlane, float farPlane, Shader* pShader, const char* uniform);
	void Inputs(GLFWwindow* pWindow);

	glm::vec3 GetPosition() const { return m_Pos; }
	glm::vec3 GetOrientation() const { return m_Orientation; }
	glm::vec3 GetUpVector() const { return m_Up; }
	int GetWidth()const { return m_Width; }
	int GetHeight()const { return m_Height; }

	void SetWidth(int& width) { m_Width = width; }
	void SetHeight(int& height) { m_Height = height; }


private:
	glm::vec3 m_Pos;
	glm::vec3 m_Orientation{ 0.f,0.f,-1.f };
	glm::vec3 m_Up{ 0.f,1.f,0.f };
	int m_Width;
	int m_Height;
	float m_Speed = 20.f;
	float m_Sensitivity = 100.f;

	bool m_FirstClick = true;
};
