#pragma once
#include <string>


std::string get_file_contents(const char* filename);

class Shader
{
public:
	Shader(const char* vertexFile, const char* fragmentFile);

	void Activate();
	void Delete();

	GLuint GetID() const { return m_ID; }
	//void SetID(const GLuint id) { m_ID = id; }

private:
	GLuint m_ID; // Shader Program ID

	void CompileErrors(GLuint shader, const char* type);
};