shader_type spatial;

uniform sampler2D noise_texture;
uniform float metallic_strength = 3.0;
uniform float roughness_strength = 1.0;
uniform float height_scale = 5;

void fragment() {
    vec2 uv = UV;
	float noise = texture(noise_texture, uv * 4.0).r;
	noise = pow(noise, .5);

    METALLIC = noise * metallic_strength;
    ROUGHNESS = (1.0 - noise) * roughness_strength;

	ALBEDO = vec3(.5, .5, .5) + noise * 0.1;
}
