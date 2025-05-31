shader_type spatial;

uniform sampler2D noise_texture;
uniform float metallic_strength = 3.0;
uniform float roughness_strength = 1.0;

void fragment() {
    vec2 uv = UV;
	float noise = texture(noise_texture, uv * 4.0).r;
	noise = pow(noise, 2.0);

    METALLIC = noise * metallic_strength;
    ROUGHNESS = (1.0 - noise) * roughness_strength;

	ALBEDO = vec3(0.6, 0.65, 0.7) + noise * 0.1;
}
