#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 color;

uniform mat4 camMatrix;
uniform mat4 transformMatrix;

void main()
{
	gl_Position = camMatrix * (transformMatrix * vec4(aPos, 1.0));
	color = vec3(transformMatrix[3][0], transformMatrix[3][1], transformMatrix[3][2]);
}
