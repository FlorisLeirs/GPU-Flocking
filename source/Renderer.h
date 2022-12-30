#pragma once
#include <GLFW/glfw3.h>

#include "Singleton.h"

class Shader;
class VBO;
class EBO;
class VAO;
class Camera;

class Renderer : public Singleton<Renderer>
{
public:
	void Initialize(GLFWwindow* pWindow, Camera* pCamera);
	void Destroy();

	void Render(GLFWwindow* pWindow) const;
	void RenderBoid(glm::mat4 transform);

private:
	Shader* m_pShader = nullptr;
	Camera* m_pCamera = nullptr;

	VBO* m_pVBO = nullptr;
	EBO* m_pEBO = nullptr;
	VAO* m_pVAO = nullptr;

};
