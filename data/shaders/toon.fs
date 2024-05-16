#version 330 core
in vec3 n;
in vec3 v;
out vec3 col;  // Color fragmento
uniform vec3 luz = vec3(1, 1, 0) / sqrt(2.0f);
vec4 coef = vec4(.2f,.7f,.1f,1.f);

void main()
{
	vec3 nn = normalize(n);
	vec3 vv = normalize(v);
	float difusa = dot(luz, nn); if (difusa < 0.f) difusa = 0;

	vec3 r = reflect(-luz,nn);

	float esp_dot = dot(r,vv);
	if (esp_dot < 0)
		esp_dot = 0;
	float esp = pow(esp_dot,coef.w);

	// float ilu = (0.15 + 0.85 * difusa + 0.01*esp);
	float ilu = (coef.x + coef.y * difusa + coef.z * esp);  //10% Ambiente + 60% difusa + 30% especular
	col = vec3(1, 1, 1);
	col = col*ilu;
}