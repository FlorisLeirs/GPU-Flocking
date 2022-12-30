#include "VAO.h"
#include "VBO.h"

VAO::VAO()
{
	glGenVertexArrays(1, &m_ID);
}

void VAO::LinkAttrib(VBO* VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizei stride, void* offset)
{
	VBO->Bind();
	glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(layout);
	VBO->UnBind();
}

void VAO::Bind()
{
	glBindVertexArray(m_ID);
}

void VAO::UnBind()
{
	glBindVertexArray(0);
}

void VAO::Delete()
{
	glDeleteVertexArrays(1, &m_ID);
}
