import bpy
import random

# Clear existing objects
bpy.ops.object.select_all(action='SELECT')
bpy.ops.object.delete(use_global=False)

ROOM_SIZE = 6
WALL_HEIGHT = 3
GRID_SIZE = 4  # 4x4 dungeon
SEED = 42
random.seed(SEED)

# Helper function to create a room
def create_room(x, y):
    bpy.ops.mesh.primitive_cube_add(size=ROOM_SIZE, location=(x * ROOM_SIZE * 1.5, y * ROOM_SIZE * 1.5, 0))
    room = bpy.context.active_object
    room.scale.z = 0.1  # flatten into a floor
    room.name = f"Room_{x}_{y}"
    return room

# Helper to create a corridor between two rooms
def create_corridor(x1, y1, x2, y2):
    mid_x = (x1 + x2) / 2 * ROOM_SIZE * 1.5
    mid_y = (y1 + y2) / 2 * ROOM_SIZE * 1.5
    dx = abs(x2 - x1)
    dy = abs(y2 - y1)
    length = ROOM_SIZE * 1.5
    width = 2

    if dx:
        bpy.ops.mesh.primitive_cube_add(size=1, location=(mid_x, mid_y, 0))
        corridor = bpy.context.active_object
        corridor.scale = (length/2, width/2, 0.1)
    elif dy:
        bpy.ops.mesh.primitive_cube_add(size=1, location=(mid_x, mid_y, 0))
        corridor = bpy.context.active_object
        corridor.scale = (width/2, length/2, 0.1)

# Place key object
def create_key(x, y):
    bpy.ops.mesh.primitive_uv_sphere_add(radius=0.5, location=(x * ROOM_SIZE * 1.5, y * ROOM_SIZE * 1.5, 1))
    key = bpy.context.active_object
    key.name = "Key"
    key.data.materials.append(make_material("KeyMat", (1, 1, 0, 1)))

# Place door (barrier)
def create_door(x, y):
    bpy.ops.mesh.primitive_cube_add(size=1, location=(x * ROOM_SIZE * 1.5, y * ROOM_SIZE * 1.5, WALL_HEIGHT / 2))
    door = bpy.context.active_object
    door.scale = (0.5, 2, WALL_HEIGHT / 2)
    door.name = "Door"
    door.data.materials.append(make_material("DoorMat", (0.6, 0.2, 0.2, 1)))

# Helper to make materials
def make_material(name, color):
    mat = bpy.data.materials.new(name)
    mat.use_nodes = True
    bsdf = mat.node_tree.nodes["Principled BSDF"]
    bsdf.inputs["Base Color"].default_value = color
    return mat

# Dungeon grid
rooms = []
for x in range(GRID_SIZE):
    row = []
    for y in range(GRID_SIZE):
        if random.random() < 0.7:  # 70% chance room exists
            room = create_room(x, y)
            row.append(1)
        else:
            row.append(0)
    rooms.append(row)

# Corridors + connections
for x in range(GRID_SIZE):
    for y in range(GRID_SIZE):
        if rooms[x][y] == 0:
            continue
        if x < GRID_SIZE - 1 and rooms[x+1][y]:
            create_corridor(x, y, x+1, y)
            if random.random() < 0.3:
                create_door((x + x+1)/2, y)
        if y < GRID_SIZE - 1 and rooms[x][y+1]:
            create_corridor(x, y, x, y+1)
            if random.random() < 0.3:
                create_door(x, (y + y+1)/2)

# Place a key randomly
while True:
    kx = random.randint(0, GRID_SIZE-1)
    ky = random.randint(0, GRID_SIZE-1)
    if rooms[kx][ky]:
        create_key(kx, ky)
        break
