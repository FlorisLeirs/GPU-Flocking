#pragma once

class VBO
{
public:
	VBO(GLfloat* pVertices, GLsizeiptr pSize);

	void Bind();
	void UnBind();
	void Delete();

	GLuint GetID() const { return m_ID; }
	//void SetID(const GLuint id) { m_ID = id; }

private:
	GLuint m_ID; // shader ID
};
