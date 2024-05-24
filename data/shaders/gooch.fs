#version 330 core
in vec3 n;
in vec3 v;
in vec2 UV;

out vec3 col;  // Color fragmento

uniform vec3 luz = vec3(1, 1, 0) / sqrt(2.0f); // light vector
uniform sampler2D unit;
uniform int render_texture = 1;
uniform vec3 model_color;
uniform float b_lightness;
uniform float y_lightness;
uniform float alpha;
uniform float beta;


vec4 coef = vec4(.2f,.7f,.1f,1.f); // phong coef 20% ambiental + 70% diffuse + 10% spec.

// gooch
vec3 blue = vec3(0,0,1);
vec3 yellow = vec3(1,1,0);

void main()
{
	// Phong model
	vec3 nn = normalize(n);
	float difusa = dot(luz, nn);

    vec3 coolColor = b_lightness * blue + alpha * model_color;
    vec3 warmColor = y_lightness * yellow + beta * model_color;

    float difusa_corregida = difusa * 0.5 + 0.5; // adjust [-1,1] to [0,1]
    // texture render is not allowed in gooch shader
    col = mix(coolColor, warmColor, difusa_corregida); // interpolates between cool and warm
}