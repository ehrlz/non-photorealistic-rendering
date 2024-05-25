#version 330 core
in vec3 n;
in vec3 v;
in vec2 UV;

out vec3 col;  // Fragment colour

uniform vec3 light; // normalized
uniform sampler2D unit; // texture
uniform float toon_border;
uniform int render_texture;
uniform int color_levels;
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

	// siluette
	float angle_view = dot(view,normal);
	if (angle_view >= -toon_border && angle_view <= toon_border){
		col = vec3(0,0,0);
		return;
	}
	// color scaling
	float level_range = 1.f / color_levels; // defines how big is a color range
	ilu = (trunc(ilu / level_range)+1) * level_range;

	if(render_texture == 1)
		col = texture(unit, UV).rgb * ilu;
	else
		col = model_color * ilu;
}