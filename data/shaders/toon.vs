#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
//out float ilu;
out vec3 n;
out vec3 v;

uniform mat4 M;
uniform mat4 PV;
uniform vec3 campos; //pos camara

void main() {
	gl_Position = PV * M * vec4(pos, 1);

	mat3 M_adj = mat3(transpose(inverse(M)));
	n = M_adj * normal;

	vec3 pos_vertice = vec3(M*vec4(pos,1));
	v = campos - pos_vertice;
}