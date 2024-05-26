#version 330 core
in vec2 UV;
uniform sampler2D unit;
uniform vec3 model_color;
uniform int render_texture;

void main(){
	if(render_texture == 1)
		gl_FragColor = texture(unit, UV);
	else
		gl_FragColor = vec4(model_color,1);
}