#version 330 core
in vec3 n;
in vec3 v;
in vec2 UV;

out vec3 col;  // Color fragmento

uniform vec3 luz = vec3(1, 1, 0) / sqrt(2.0f); // light vector
uniform sampler2D unit; // texture

vec4 coef = vec4(.2f,.7f,.1f,1.f); // phong coef 20% ambiental + 70% diffuse + 10% spec.

void main()
{
	// Phong model
	vec3 nn = normalize(n);
	vec3 vv = normalize(v);
	float difusa = dot(luz, nn); if (difusa < 0.f) difusa = 0;

	vec3 r = reflect(-luz,nn);

	float esp_dot = dot(r,vv);
	if (esp_dot < 0)
		esp_dot = 0;
	float esp = pow(esp_dot,coef.w);

	float ilu = (coef.x + coef.y * difusa + coef.z * esp);

	col = texture(unit, UV).rgb * ilu;
}