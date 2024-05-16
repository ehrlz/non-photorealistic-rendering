#version 330 core
in vec2 UV;
out vec3 outputColor;
uniform vec3 regilla=vec3(16, 16, 4);
uniform sampler2D unit;
uniform vec2 resolucion=vec2(800, 600);
void main(){
	float luz=1;

	int x = int(gl_FragCoord.x)%int(regilla.x);
	int y = int(gl_FragCoord.y)%int(regilla.y);
	if(x==0 || y==0){
		discard;
	}else{
		x = x>regilla.x/2 ? int(regilla.x)-x : x;
		y = y>regilla.y/2 ? int(regilla.y)-y : y;
		if(x<regilla.z || y<regilla.z){
			luz=min(x, y)/regilla.z;
		}
	}

	vec2 Pixels=resolucion/vec2(regilla.x, regilla.y);
	float dx = regilla.x * (1.0 / Pixels.x);
	float dy = regilla.y * (1.0 / Pixels.y);
	vec2 Coord = vec2(dx * floor(UV.x / dx),
						dy * floor(UV.y / dy));
	outputColor = texture(unit, Coord).rgb*luz;
 }