#include "pch.h"
#include "VBO.h"

VBO::VBO(GLfloat* pVertices, GLsizeiptr pSize)
{
	glGenBuffers(1, &m_ID);
	glBindBuffer(GL_ARRAY_BUFFER, m_ID);
	//Add vertices to buffer
	glBufferData(GL_ARRAY_BUFFER, pSize, pVertices, GL_STATIC_DRAW);
}

void VBO::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_ID);
}

void VBO::UnBind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::Delete()
{
	glDeleteBuffers(1, &m_ID);
}
