#version 330 core
in vec2 uv;
uniform sampler2D bufferTexture;
uniform vec3 rejilla=vec3(16, 16, 4);
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
    // Se divide la pantalla en píxeles
    vec2 pixelSize = vec2(rejilla.x, rejilla.y) / resolucion;
    vec2 pixelCenter = floor(uv / pixelSize) * pixelSize + pixelSize * 0.5;

    // Integrar sobre el área del pixel para obtener el color medio
    vec4 color = vec4(0.0);
    int numSamples = 4; // Puedes ajustar este valor para mayor precisión
    float offset = 0.5 / float(numSamples);
    for (int i = 0; i < numSamples; ++i) {
        for (int j = 0; j < numSamples; ++j) {
            vec2 sampleCoord = pixelCenter + vec2(float(i) - offset, float(j) - offset) * pixelSize / float(numSamples);
            color += texture(bufferTexture, sampleCoord);
        }
    }
    color /= float(numSamples * numSamples);

    gl_FragColor = color * luz;
}