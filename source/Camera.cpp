#include "pch.h"
#include "Camera.h"

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "Shader.h"

Camera::Camera(int& width, int& height, glm::vec3 pos)
	: m_Pos(pos)
	, m_Width(width)
	, m_Height(height)
{
}

void Camera::Matrix(float FOVdeg, float nearPlane, float farPlane, Shader* pShader, const char* uniform)
{
	glm::mat4 view{ 1.f };
	glm::mat4 projection{ 1.f };

	view = glm::lookAt(m_Pos, m_Pos + m_Orientation, m_Up);
	projection = glm::perspective(glm::radians(FOVdeg), static_cast<float>(m_Width / m_Height),
		nearPlane, farPlane);

	glUniformMatrix4fv(glGetUniformLocation(pShader->GetID(), uniform), 1, GL_FALSE, glm::value_ptr(projection * view));
}

void Camera::Inputs(GLFWwindow* pWindow)
{

	//WASD
	if (glfwGetKey(pWindow, GLFW_KEY_W) == GLFW_PRESS)
	{
		m_Pos += m_Speed * m_Orientation;
	}
	if (glfwGetKey(pWindow, GLFW_KEY_A) == GLFW_PRESS)
	{
		m_Pos += m_Speed * -glm::normalize(glm::cross(m_Orientation, m_Up));
	}
	if (glfwGetKey(pWindow, GLFW_KEY_S) == GLFW_PRESS)
	{
		m_Pos += m_Speed * -m_Orientation;
	}
	if (glfwGetKey(pWindow, GLFW_KEY_D) == GLFW_PRESS)
	{
		m_Pos += m_Speed * glm::normalize(glm::cross(m_Orientation, m_Up));
	}

	// UP
	if (glfwGetKey(pWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		m_Pos += m_Speed * m_Up;
	}
	// DOWN
	if (glfwGetKey(pWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		m_Pos += m_Speed * -m_Up;
	}

	// Speed up
	if (glfwGetKey(pWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		m_Speed = 0.8f;
	}
	// Put speed back to normal
	if (glfwGetKey(pWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		m_Speed = 0.5f;
	}


	// Mouse
	if (!ImGui::GetIO().WantCaptureMouse)
	{
		if (glfwGetMouseButton(pWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			
			if (m_FirstClick)
			{
				glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
				glfwSetCursorPos(pWindow, m_Width / 2, m_Height / 2);
				m_FirstClick = false;
			}


			double mouseX;
			double mouseY;

			glfwGetCursorPos(pWindow, &mouseX, &mouseY);

			float rotX = m_Sensitivity * static_cast<float>(mouseY - (m_Height / 2)) / m_Height;
			float rotY = m_Sensitivity * static_cast<float>(mouseX - (m_Width / 2)) / m_Width;

			glm::vec3 newOrientation = glm::rotate(m_Orientation, glm::radians(-rotX),
				glm::normalize(glm::cross(m_Orientation, m_Up)));

			if (!(glm::angle(newOrientation, m_Up) <= glm::radians(5.f) || glm::angle(newOrientation, -m_Up) <=
				glm::radians(-5.f)))
			{
				m_Orientation = newOrientation;
			}

			m_Orientation = glm::rotate(m_Orientation, glm::radians(-rotY), m_Up);


			glfwSetCursorPos(pWindow, m_Width / 2, m_Height / 2);

		}
		else if (glfwGetMouseButton(pWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
		{
			glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			m_FirstClick = true;
		}
	}
}
