#pragma once

class Boid
{
public:
	Boid(glm::vec3 pos);

	void Render() const;

	void SetTransform(glm::mat4 transform) {m_Transform = transform;}

	glm::mat4& GetTransform() { return m_Transform; }
	glm::vec3& GetForwardVector() { return m_ForwardVector; }
	glm::vec3& GetPrevPosition() { return m_PrevPos; }

private:
	glm::vec3 m_ForwardVector{ 1.f, 0.f, 0.f };
	glm::mat4 m_Transform{ 1.f };
	glm::vec3 m_PrevPos;
};
