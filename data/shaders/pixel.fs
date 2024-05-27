#version 330 core
in vec2 UV;
out vec3 outputColor;
uniform vec3 rejilla=vec3(16, 16, 4);
// Estructura de la rejilla: 
//		x: Tamaño del pixel en horizontal
//		y: Tamaño del pixel en vertical
//		z: Tamaño del sombreado en pixeles (No se usa en este shader)
uniform sampler2D unit;
uniform vec2 resolucion=vec2(800, 600);
void main(){
    //Se amplia UV a la resolucion usada
    vec2 p = UV * resolucion;

    // Se coje un solo pixel de la textura con el modulo (Esquina)
    p.x -= mod(p.x, rejilla.x);
    p.y -= mod(p.y, rejilla.y);
    
    // Se resescala a cordenadas UV y se obtiene el color
    outputColor = texture(unit, p / resolucion).rgb;
}