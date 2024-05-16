#version 330 core
in vec2 UV;
out vec3 outputColor;
uniform vec3 regilla=vec3(16, 16, 4);
uniform sampler2D unit;
uniform vec2 resolucion=vec2(800, 600);
void main(){
    vec2 p = UV * resolucion;

    p.x -= mod(p.x, regilla.x);
    p.y -= mod(p.y, regilla.y);
    
    outputColor = texture(unit, p / resolucion).rgb;
}