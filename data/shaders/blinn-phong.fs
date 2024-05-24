#version 330 core
in vec3 n;
in vec3 v;
in vec2 UV;

out vec3 col;  // Fragment colour

uniform vec3 light; // normalized
uniform sampler2D unit;
uniform int render_texture = 1;
uniform vec3 model_color;

vec4 coef = vec4(.2f,.7f,.1f,1.f); // phong coef 20% ambiental + 70% diffuse + 10% spec.

void main()
{
	// Phong model
	vec3 normal = normalize(n);
	vec3 view = normalize(v);

	float diffuse = max(dot(light, normal), 0);

    // blinn-phong calculates spec from a half-angle vector 'H'
    vec3 h = normalize(light+view);

	float spec_dot = max(dot(normal,h), 0);
	float spec = pow(spec_dot,coef.w);

	float ilu = coef.x + coef.y * diffuse + coef.z * spec;

	if(render_texture == 1)
		col = texture(unit, UV).rgb * ilu;
	else
		col = model_color * ilu;
}