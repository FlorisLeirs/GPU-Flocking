#include "GPUFlocking.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <CL/cl.hpp>

#include "Shader.h"
#include "VBO.h"
#include "EBO.h"
#include "VAO.h"
#include "Camera.h"


GLfloat vertices[] =
{ //	COORDINATES		  /			COLORS
	-0.25f, 0.0f,  0.5f,	  0.83f, 0.70f, 0.44f,
	-0.25f, 0.0f, -0.5f,	  0.83f, 0.70f, 0.44f,
	 0.25f, 0.0f, -0.5f,	  0.83f, 0.70f, 0.44f,
	 0.25f, 0.0f,  0.5f,	  0.83f, 0.70f, 0.44f,
	 0.00f, 0.8f,  0.0f,	  0.92f, 0.86f, 0.76f
};

GLuint indices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 1, 4,
	1, 2, 4,
	2, 3, 4,
	3, 0, 4
};


GPUFlocking::~GPUFlocking()
{
	glfwDestroyWindow(m_pWindow);
	glfwTerminate();
}

void GPUFlocking::Run()
{
	Initialize();

	//Create program
	Shader shaderProgram{ "default.vert", "default.frag" };

	//create vertex array object
	VAO VAO1;
	VAO1.Bind();

	// create vertex buffer object and element buffer object
	VBO VBO1{ vertices, sizeof(vertices) };
	EBO EBO1{ indices, sizeof(indices) };

	// link VBO to VAO
	VAO1.LinkAttrib(&VBO1, 0, 3, GL_FLOAT, 6 * sizeof(GLfloat), nullptr);
	VAO1.LinkAttrib(&VBO1, 1, 3, GL_FLOAT, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));

	VAO1.UnBind();
	VBO1.UnBind();
	EBO1.UnBind();


	float rotation = 0.0f;
	double prevTime = glfwGetTime();

	glEnable(GL_DEPTH_TEST);

	Camera* pCamera = new Camera(m_Width, m_Height, glm::vec3(0.f, 0.f, 2.f));


	//loop
	while (!glfwWindowShouldClose(m_pWindow))
	{
		glClearColor(0.2f, 0.4f, 0.7f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderProgram.Activate();

		pCamera->Inputs(m_pWindow);
		pCamera->Matrix(45.f, 0.1f, 100.f, &shaderProgram, "camMatrix");


		// scale
		//glUniform1f(uniID, 1.5f);

		VAO1.Bind();

		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(m_pWindow);

		glfwPollEvents();
	}

	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	shaderProgram.Delete();
}

void GPUFlocking::Initialize()
{
	glfwInit();
	//Setup version and profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Create window
	m_pWindow = glfwCreateWindow(m_Width, m_Height, "GPUFlocking", NULL, NULL);
	if (m_pWindow == nullptr)
	{
		std::cout << "Window invalid" << std::endl;
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(m_pWindow);

	//load glad
	gladLoadGL();

	//specify viewport
	glViewport(0, 0, m_Width, m_Height);



	//OpenCL
	// Create platform
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	auto platform = platforms.front();

	//Create device
	std::vector<cl::Device> devices;
	platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
	cl::Device device = devices.front(); 

}
