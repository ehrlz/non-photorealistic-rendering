#version 330 core
in vec3 position;
out vec2 uv;

uniform sampler2D bufferTexture;

void main()
{
	gl_Position = vec4(position, 1.0);
	uv = position.xy * 0.5 + 0.5; // position sets the postrender uv
}
