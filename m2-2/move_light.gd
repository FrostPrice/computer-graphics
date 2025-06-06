extends OmniLight3D

@export var radius: float = 2.0
@export var speed: float = 1.0
@export var center: Vector3 = Vector3(0.0, 2.0, 2.0)

var angle: float = 0.0

func _process(delta):
	angle += delta * speed

	# Calcula nova posição circular no plano XZ
	var x = center.x + cos(angle) * radius
	var z = center.z + sin(angle) * radius
	var y = center.y

	global_position = Vector3(x, y, z)

	look_at(center)
