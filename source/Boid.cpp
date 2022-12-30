#include "pch.h"
#include <glm/glm.hpp>

#include "Boid.h"

Boid::Boid(glm::vec3 pos)
{
	// right = randomVector x forward
	glm::vec3 right = glm::cross(glm::vec3(1, 0, 0), m_ForwardVector);

	glm::vec3 up = glm::cross(m_ForwardVector, right);

	m_LookAt[0] = glm::vec4{ right, 0.f };
	m_LookAt[1] = glm::vec4{ up, 0.f };
	m_LookAt[2] = glm::vec4{ m_ForwardVector, 0.f };
	m_LookAt[3] = glm::vec4{ pos, 1.f };
}
