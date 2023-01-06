#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 color;

uniform mat4 camMatrix;
uniform vec3 pos;

void main()
{
   mat4 translation = mat4(
        vec4(1.0, 0.0, 0.0, 0.0),
        vec4(0.0, 1.0, 0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        vec4(pos, 1.0));
	gl_Position = camMatrix * (translation * vec4(aPos, 1.0));
	color = vec3(pos.x/100.0f, pos.y/100.0f, pos.z/100.0f);
}
