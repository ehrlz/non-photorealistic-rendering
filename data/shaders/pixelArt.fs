#version 330 core
in vec2 UV;
out vec3 outputColor;
uniform sampler2D unit;
void main(){
	outputColor = texture(unit, UV).rgb;
	// outputColor = vec3(1.f, 1.f, 1.f);
}