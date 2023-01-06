#include "pch.h"
#include "Renderer.h"

#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>
#include <imgui_impl_opengl3.h>

#include "Camera.h"
#include "EBO.h"
#include "Shader.h"
#include "VAO.h"
#include "VBO.h"
#include "BoidManager.h"

GLfloat vertices[] =
{
	-0.25f, -.4f,  0.5f,
	-0.25f, -.4f, -0.5f,
	 0.25f, -.4f, -0.5f,
	 0.25f, -.4f,  0.5f,
	 0.00f, 0.4f,  0.0f
};

GLuint indices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 1, 4,
	1, 2, 4,
	2, 3, 4,
	3, 0, 4
};

void Renderer::Initialize(GLFWwindow* pWindow, Camera* pCamera)
{
	//load glad
	gladLoadGL();

	int width, height;
	glfwGetWindowSize(pWindow, &width, &height);
	//specify viewport
	glViewport(0, 0, width, height);

	m_pShader = new Shader{ "default.vert", "default.frag" };

	m_pVAO = new VAO();
	m_pVAO->Bind();

	// create vertex buffer object and element buffer object
	m_pVBO = new VBO{ vertices, sizeof(vertices) };
	m_pEBO = new EBO{ indices, sizeof(indices) };

	// link VBO to VAO
	m_pVAO->LinkAttrib(m_pVBO, 0, 3, GL_FLOAT, 3 * sizeof(GLfloat), nullptr);

	m_pVAO->UnBind();
	m_pVBO->UnBind();
	m_pEBO->UnBind();

	glEnable(GL_DEPTH_TEST);

	m_pCamera = pCamera;
}

void Renderer::Destroy()
{
	if (m_pVAO)
		m_pVAO->Delete();
	delete m_pVAO;
	m_pVAO = nullptr;

	if (m_pVBO)
		m_pVBO->Delete();
	delete m_pVBO;
	m_pVBO = nullptr;

	if (m_pEBO)
		m_pEBO->Delete();
	delete m_pEBO;
	m_pEBO = nullptr;

	if (m_pShader)
		m_pShader->Delete();
	delete m_pShader;
	m_pShader = nullptr;

	delete m_pCamera;
	m_pCamera = nullptr;

}

void Renderer::Render(GLFWwindow* pWindow) const
{
	m_pShader->Activate();

	m_pCamera->Matrix(45.f, 0.1f, 600.f, m_pShader, "camMatrix");

	m_pVAO->Bind();
	BoidManager::GetInstance().Render();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(pWindow);

	glClearColor(0.2f, 0.4f, 0.7f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::RenderBoids(const std::vector<float>& positions, int nrOfBoids)
{
	GLuint transformLoc = glGetUniformLocation(m_pShader->GetID(), "pos");
	for (int i{}; i != nrOfBoids; ++i)
	{
		glUniform3f(transformLoc, positions[i * 16 + 12], positions[i * 16 + 13], positions[i * 16 + 14]);
		//glUniform3f(transformLoc, positions[i * 3], positions[i * 3 + 1], positions[i * 3 + 2]);
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
	}

}
