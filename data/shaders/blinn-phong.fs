#version 330 core
in vec3 n;
in vec3 v;
in vec2 UV;

out vec3 col;  // Fragment colour

uniform vec3 light; // normalized
uniform sampler2D unit;
uniform int render_texture = 1;
uniform vec3 model_color;
uniform vec4 coefs;

void main()
{
	// Phong model
	vec3 normal = normalize(n);
	vec3 view = normalize(v);

	float diffuse = max(dot(light, normal), 0);

    // blinn-phong calculates spec from a half-angle vector 'H'
    vec3 h = normalize(light+view);

	float spec_dot = max(dot(normal,h), 0);
	float spec = pow(spec_dot,coefs.w);

	float ilu = coefs.x + coefs.y * diffuse + coefs.z * spec;

	if(render_texture == 1)
		col = texture(unit, UV).rgb * ilu;
	else
		col = model_color * ilu;
}