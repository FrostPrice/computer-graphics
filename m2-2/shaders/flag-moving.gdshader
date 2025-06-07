shader_type spatial;

render_mode cull_disabled;

uniform float wave_amplitude = 0.2;
uniform float wave_speed = 3.0;
uniform float wave_frequency = 6.0;

uniform sampler2D flag_texture;

varying float wave_strength;

void vertex() {
    // Gera ondas mais dinâmicas combinando seno e cosseno
    float wave = sin(UV.x * wave_frequency + TIME * wave_speed)
               + cos(UV.y * wave_frequency * 1.2 + TIME * wave_speed * 1.5);
    wave_strength = wave;
    VERTEX.y += wave * wave_amplitude;
}

void fragment() {
      vec3 tex_color = texture(flag_texture, UV).rgb;

    // Transforma fortemente a cor com base no wave_strength
    float intensity = abs(wave_strength);

    vec3 wave_shift = vec3(
        tex_color.r * (1.0 - intensity),
        tex_color.g * (0.5 + 0.5 * intensity),
        tex_color.b * (0.3 + 0.7 * intensity)
    );

    ALBEDO = clamp(wave_shift, 0.0, 1.0);
}
