#version 330 core
in vec2 uv;
uniform sampler2D bufferTexture;

void main()
{
	gl_FragColor = vec4(1) - texture(bufferTexture, uv);
}