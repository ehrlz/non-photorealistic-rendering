#version 330 core
in vec2 UV;
out vec3 outputColor;
// Estructura de la rejilla: 
//		x: Tamaño del pixel en horizontal
//		y: Tamaño del pixel en vertical
//		z: Tamaño del sombreado en pixeles
uniform vec3 rejilla=vec3(16, 16, 4);
uniform sampler2D unit;
uniform vec2 resolucion=vec2(800, 600);
void main(){
	float luz=1; // Sombreado de casillas

	// Se busca el limite de los pixeles para descartarlos
	int x = int(gl_FragCoord.x)%int(rejilla.x);
	int y = int(gl_FragCoord.y)%int(rejilla.y);
	if(x==0 || y==0){
		discard;
	}else{ // Si esta cerca de los bordes se sombrea 
		x = x>rejilla.x/2 ? int(rejilla.x)-x : x;
		y = y>rejilla.y/2 ? int(rejilla.y)-y : y;
		if(x<rejilla.z || y<rejilla.z){ // La luz se calcula dependiendo del borde más cercano
			luz=min(x, y)/rejilla.z;
		}
	}

	// Color de la zona
	// Se divide la pantalla en pixeles
	vec2 Pixels=resolucion/vec2(rejilla.x, rejilla.y);
	// Se coge el color de una de las esquinas del pixel
	float dx = rejilla.x * (1.0 / Pixels.x);
	float dy = rejilla.y * (1.0 / Pixels.y);
	vec2 Coord = vec2(dx * floor(UV.x / dx),
						dy * floor(UV.y / dy));
	outputColor = texture(unit, Coord).rgb*luz;
}