shader_type spatial;

uniform float pulse_speed = 4.0;
uniform float pulse_amplitude = 0.2;
uniform vec3 base_color;

void vertex() {
    float scale = 1.0 + sin(TIME * pulse_speed) * pulse_amplitude;
    VERTEX *= scale;
}

void fragment() {
    float pulse = abs(sin(TIME * pulse_speed));
    ALBEDO = base_color * pulse;
}
