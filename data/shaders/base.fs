#version 330 core
in vec2 UV;
uniform sampler2D unit;

void main(){
	gl_FragColor = texture(unit, UV);
}