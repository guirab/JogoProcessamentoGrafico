#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoords;

uniform mat4 projection;
uniform mat4 model;

out vec2 texCoords;

void main()
{
	//...pode ter mais linhas de código aqui!
	gl_Position = projection * model * vec4(position.x, position.y, position.z, 1.0);
	texCoords = vec2(texcoords.x,1 - texcoords.y);
}