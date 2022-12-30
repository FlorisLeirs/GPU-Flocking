#include "pch.h"
#include "EBO.h"

EBO::EBO(GLuint* pIndices, GLsizeiptr pSize)
{
	glGenBuffers(1, &m_ID);
	//Bind element buffer object and add indices to buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, pSize, pIndices, GL_STATIC_DRAW);
}

void EBO::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
}

void EBO::UnBind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::Delete()
{
	glDeleteBuffers(1, &m_ID);
}
