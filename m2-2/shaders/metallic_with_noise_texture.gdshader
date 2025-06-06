shader_type spatial;

uniform sampler2D noise_texture;

uniform float time_scale = 1.5;
uniform float uv_scale = 4.0;
uniform float distortion_strength = 0.2;

uniform float metallic_strength = 1.0;
uniform float roughness_strength = 0.5;
uniform float height_strength = 0.1;

varying float noise_value;

void vertex() {
    // Usa a posição do vértice no espaço local como coordenada de amostragem
    vec3 sample_pos = (NORMAL * 0.5) + 0.5;
    vec2 uv = sample_pos.xy * uv_scale;

    // Aplica distorção com tempo
    uv.x += sin(sample_pos.y * 8.0 + TIME * time_scale) * distortion_strength;
    uv.y += cos(sample_pos.x * 8.0 + TIME * time_scale) * distortion_strength;

    float noise = texture(noise_texture, uv).r;
    noise = pow(noise, 1.2);
    noise_value = noise;

    // Aplica deformação real na superfície (parece derreter)
    VERTEX += NORMAL * noise * height_strength;
}

void fragment() {
    float n = noise_value;

    METALLIC = metallic_strength;
    ROUGHNESS = mix(0.2, 1.0, n) * roughness_strength;

    ALBEDO = vec3(0.4, 0.6, 0.8) + n * 0.2;
	EMISSION = vec3(0.3, 0.4, 0.6) * pow(noise_value, 5.0) * 1.0;
}
