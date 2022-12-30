#pragma once
#include <glm/glm.hpp>

class Boid
{
public:
	Boid(glm::vec3 pos, float neighbourRadius);

	void Render();

private:
	glm::vec3 m_ForwardVector{0.f,0.f, 1.f};
	float m_MaxSpeed = 7.f;
	glm::vec3 m_Pos;

	float m_NeighbourRadius;

	float m_SeperationWeigth = 0.65f;
	float m_CohesionWeigth = 0.45f;
	float m_AllignmentWeigth = 0.4f;
	bool m_WeightChanged = false;
};
