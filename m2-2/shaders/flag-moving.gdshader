shader_type spatial;

uniform float wave_amplitude = 0.1;
uniform float wave_speed = 2.0;
uniform float wave_frequency = 4.0;

varying float wave_intensity;

void vertex() {
    wave_intensity = sin(UV.x * wave_frequency + TIME * wave_speed);
    VERTEX.y += wave_intensity * wave_amplitude;
}

void fragment() {
    ALBEDO = vec3(1.0, 1.0, 1.0) * abs(wave_intensity);
}
