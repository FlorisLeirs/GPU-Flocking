#pragma once

class Boid
{
public:
	Boid(glm::vec3 pos);

	void Render();
	glm::mat4 GetLookat();

private:
	glm::vec3 m_ForwardVector{0.f,1.f, 0.f};
	float m_MaxSpeed = 7.f;
	glm::mat4 m_LookAt{};
};
