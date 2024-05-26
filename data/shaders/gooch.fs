#version 330 core
in vec3 n;
in vec3 v;
in vec2 UV;

out vec3 col;  // Fragment colour

uniform vec3 light; // normalized
uniform sampler2D unit;
uniform int render_texture;
uniform vec3 model_color;

uniform float b_lightness; // shader params
uniform float y_lightness;
uniform float alpha;
uniform float beta;

float border = 0.15;

// gooch
vec3 blue = vec3(0,0,1);
vec3 yellow = vec3(1,1,0);

void main()
{
	// Phong model
	vec3 normal = normalize(n);
    vec3 view = normalize(v);
	float diffuse = dot(light, normal);

    // siluette
	float angle_view = dot(view,normal);
	if (angle_view >= -border && angle_view <= border){
		col = vec3(0,0,0);
		return;
	}

    vec3 render_color;
    if (render_texture == 1)
        render_color = texture(unit, UV).rgb;
    else
        render_color = model_color;
    vec3 coolColor = b_lightness * blue + alpha * render_color;
    vec3 warmColor = y_lightness * yellow + beta * render_color;

    float scaled_diffuse = diffuse * 0.5 + 0.5; // adjust [-1,1] to [0,1]
    col = mix(coolColor, warmColor, scaled_diffuse); // interpolates between cool and warm
}