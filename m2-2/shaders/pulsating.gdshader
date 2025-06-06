shader_type spatial;

uniform float pulse_speed = 3.5;
uniform float pulse_amplitude = 0.15;
uniform vec3 base_color = vec3(0.2, 0.7, 1.0);
uniform float emission_boost = 1.5;

void vertex() {
    float t = TIME * pulse_speed;
    float scale = 1.0 + sin(t) * pulse_amplitude;
    VERTEX *= vec3(scale, 1.0 + sin(t * 0.5) * pulse_amplitude * 0.5, scale);
}

void fragment() {
    float brightness = abs(sin(TIME * pulse_speed));
	ALBEDO = base_color * brightness;
	EMISSION = vec3(
    sin(TIME * pulse_speed * 0.5),
    sin(TIME * pulse_speed * 0.8 + 1.0),
    sin(TIME * pulse_speed + 2.0)
) * pow(brightness, 2.0) * emission_boost;

}

