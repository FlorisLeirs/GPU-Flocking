#pragma once
#include <glad/glad.h>

class VBO;

class VAO
{
public:
	VAO();

	void LinkAttrib(VBO* VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizei stride, void* offset);
	void Bind();
	void UnBind();
	void Delete();

	GLuint GetID() const { return m_ID; }
	//void SetID(const GLuint id) { m_ID = id; }

private:
	GLuint m_ID; // shader ID
};