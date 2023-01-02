#include "pch.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Boid.h"
#include "Renderer.h"


Boid::Boid(glm::vec3 pos)
	: m_PrevPos(pos)
{
	// right = randomVector x forward
	//glm::vec3 right = glm::cross(glm::vec3(0, 1, 0), m_ForwardVector);

	glm::vec3 up{ 0.f,1.f,0.f };

	//m_Transform = glm::lookAt(pos, pos + m_ForwardVector, up);

	float angle{};
	angle = glm::acos(glm::dot(up, m_ForwardVector));
	if (angle < FLT_EPSILON && angle > -FLT_EPSILON)
		m_Transform = glm::rotate(m_Transform, angle, glm::cross(m_ForwardVector, up));
	m_Transform = glm::translate(m_Transform, pos);

}

void Boid::Render() const
{
	Renderer::GetInstance().RenderBoid(m_Transform);
}
