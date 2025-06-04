shader_type spatial;

uniform float wave_amplitude = 0.1;
uniform float wave_speed = 2.0;
uniform float wave_frequency = 4.0;
uniform sampler2D flag_texture;

varying float wave_intensity;

void vertex() {
    wave_intensity = sin(UV.x * wave_frequency + TIME * wave_speed);
    VERTEX.y += wave_intensity * wave_amplitude;
}

void fragment() {
    ALBEDO = texture(flag_texture, UV).rgb * abs(wave_intensity);
}
