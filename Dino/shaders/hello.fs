#version 450

out vec4 color;

in vec2 texCoords;

uniform sampler2D tex_buffer;

uniform vec2 offsets;

void main()
{
	color = texture(tex_buffer, texCoords + offsets);
}
