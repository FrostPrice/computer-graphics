"""
Procedural Dungeon Generator with Graph-based Pathfinding
A Blender script that generates procedural dungeons using NetworkX graphs.
"""

import random
import bpy
import networkx as nx
import math
from typing import Dict, List, Set, Tuple, Optional


class DungeonConfig:
    """Configuration settings for dungeon generation."""
    
    def __init__(self):
        # Grid and probability settings
        self.grid_size = 4
        self.room_probability = 0.7
        self.floors = 1
        self.floor_thickness_ratio = 0.1
        self.barrier_probability = 0.3
        
        # Dimensional settings
        self.room_size = 6
        self.wall_height = 5
        self.corridor_height = 0.2
        self.corridor_width = 2
        self.wall_thickness = 0.2
        
        # Spacing and positioning
        self.room_spacing = 1.5  # Multiplier for room positioning
        self.wall_offset = 0.1   # Offset to prevent overlap
        
        # Random seed
        self.seed = 42


class MaterialManager:
    """Manages Blender materials for the dungeon."""
    
    @staticmethod
    def create_material(name: str, color: Tuple[float, float, float, float]):
        """Create or get a material with the specified color."""
        mat = bpy.data.materials.get(name) or bpy.data.materials.new(name)
        mat.use_nodes = True
        bsdf = mat.node_tree.nodes.get("Principled BSDF")
        if bsdf:
            bsdf.inputs["Base Color"].default_value = color
        return mat
    
    @classmethod
    def get_floor_material(cls):
        return cls.create_material("FloorMat", (0.6, 0.6, 0.6, 1))
    
    @classmethod
    def get_wall_material(cls):
        return cls.create_material("WallMat", (0.3, 0.3, 0.3, 1))
    
    @classmethod
    def get_corridor_material(cls):
        return cls.create_material("CorridorMat", (0.5, 0.5, 0.5, 1))
    
    @classmethod
    def get_door_material(cls):
        return cls.create_material("DoorMat", (0.6, 0.2, 0.2, 1))
    
    @classmethod
    def get_stairs_material(cls):
        return cls.create_material("StairMat", (0.8, 0.6, 0.4, 1))
    
    @classmethod
    def get_key_material(cls):
        return cls.create_material("KeyMat", (1.0, 1.0, 0.2, 1))


class PositionCalculator:
    """Handles position calculations for dungeon elements."""
    
    def __init__(self, config: DungeonConfig):
        self.config = config
    
    def get_room_position(self, x: int, y: int, z: int) -> Tuple[float, float, float]:
        """Calculate the world position for a room."""
        ox = x * self.config.room_size * self.config.room_spacing
        oy = y * self.config.room_size * self.config.room_spacing
        oz = z * (self.config.wall_height + 1)
        return ox, oy, oz
    
    def get_floor_top(self, z_base: float) -> float:
        """Calculate the top surface of a floor after scaling."""
        return z_base + (self.config.room_size * self.config.floor_thickness_ratio) / 2
            
    def get_corridor_position(self, x1: int, y1: int, x2: int, y2: int, z: int) -> Tuple[float, float, float]:
        """Calculate the position for a corridor between two rooms."""
        mid_x = ((x1 + x2) / 2) * self.config.room_size * self.config.room_spacing
        mid_y = ((y1 + y2) / 2) * self.config.room_size * self.config.room_spacing
        # Position corridor at the same level as room floors
        base_z = z * (self.config.wall_height + 1)
        floor_level = self.get_floor_top(base_z)
        return mid_x, mid_y, floor_level


class GraphGenerator:
    """Generates the graph structure for the dungeon."""
    
    def __init__(self, config: DungeonConfig):
        self.config = config
    
    def generate_floor_graph(self) -> Tuple[nx.Graph, List[Tuple[int, int]]]:
        """Generate a graph representing rooms and connections for one floor."""
        G = nx.Graph()
        rooms = {}
        
        # Generate rooms based on probability
        for x in range(self.config.grid_size):
            for y in range(self.config.grid_size):
                if random.random() < self.config.room_probability:
                    node = (x, y)
                    G.add_node(node)
                    rooms[node] = True
        
        # Connect adjacent rooms
        for x in range(self.config.grid_size):
            for y in range(self.config.grid_size):
                if (x, y) not in rooms:
                    continue
                for dx, dy in [(1, 0), (0, 1), (-1, 0), (0, -1)]:
                    nx_, ny_ = x + dx, y + dy
                    if (nx_, ny_) in rooms and not G.has_edge((x, y), (nx_, ny_)):
                        G.add_edge((x, y), (nx_, ny_))

        return G, list(rooms.keys())


class MeshCreator:
    """Creates 3D meshes for dungeon elements."""
    
    def __init__(self, config: DungeonConfig, position_calc: PositionCalculator):
        self.config = config
        self.pos_calc = position_calc
        self.materials = MaterialManager()
    
    def create_room(self, x: int, y: int, z: int = 0, name: str = "Room", 
                   connected_dirs: Optional[Set[str]] = None) -> None:
        """Create a room with floor and walls."""
        if connected_dirs is None:
            connected_dirs = set()
        
        ox, oy, oz = self.pos_calc.get_room_position(x, y, z)
        
        # Create floor
        self._create_floor(ox, oy, oz, x, y, z, name)
        floor_top = self.pos_calc.get_floor_top(oz)
        
        # Create walls
        self._create_walls(ox, oy, floor_top, x, y, z, name, connected_dirs)
    
    def _create_floor(self, ox: float, oy: float, oz: float, 
                     x: int, y: int, z: int, name: str) -> None:
        """Create a floor for a room."""
        bpy.ops.mesh.primitive_cube_add(size=self.config.room_size, location=(ox, oy, oz))
        floor = bpy.context.active_object
        floor.scale.z = self.config.floor_thickness_ratio
        floor.name = f"{name}_Floor_{x}_{y}_{z}"
        floor.data.materials.append(self.materials.get_floor_material())
    
    def _create_walls(self, ox: float, oy: float, floor_top: float,
                     x: int, y: int, z: int, name: str, connected_dirs: Set[str]) -> None:
        """Create walls for a room."""
        directions = {
            "E": (1, 0),
            "W": (-1, 0),
            "N": (0, 1),
            "S": (0, -1),
        }
        
        for dir_name, (dx, dy) in directions.items():
            if dir_name in connected_dirs:
                continue
            
            # Position walls at the edge of the floor, sitting directly on the floor surface
            wx = ox + dx * (self.config.room_size / 2 + self.config.wall_thickness / 2)
            wy = oy + dy * (self.config.room_size / 2 + self.config.wall_thickness / 2)
            # Position wall center lower so bottom extends below floor level
            wall_z = floor_top + (self.config.wall_height / 2)
            
            bpy.ops.mesh.primitive_cube_add(size=2, location=(wx, wy, wall_z))
            wall = bpy.context.active_object
            wall.name = f"{name}_Wall_{dir_name}_{x}_{y}_{z}"
            
            # Scale walls properly - full room height and proper thickness
            if dx != 0:  # East/West walls
                wall.scale = (self.config.wall_thickness / 2, self.config.room_size / 2, self.config.wall_height / 2)
            else:  # North/South walls
                wall.scale = (self.config.room_size / 2, self.config.wall_thickness / 2, self.config.wall_height / 2)
            
            wall.data.materials.append(self.materials.get_wall_material())
    
    def create_corridor(self, x1: int, y1: int, z: int, x2: int, y2: int) -> None:
        """Create a corridor between two rooms."""
        mid_x, mid_y, floor_level = self.pos_calc.get_corridor_position(x1, y1, x2, y2, z)
        
        dx = abs(x2 - x1)
        dy = abs(y2 - y1)
        
        # Create corridor at the same level as room floors
        if dx == 1:  # Horizontal corridor
            length = self.config.room_size * self.config.room_spacing
            width = self.config.corridor_width
            bpy.ops.mesh.primitive_cube_add(size=1, location=(mid_x, mid_y, floor_level))
            corridor = bpy.context.active_object
            corridor.scale = (length / 2, width / 1, self.config.corridor_height / 2)
            corridor.name = f"Corridor_H_{x1}_{y1}_{x2}_{y2}_{z}"
        elif dy == 1:  # Vertical corridor
            length = self.config.room_size * self.config.room_spacing
            width = self.config.corridor_width
            bpy.ops.mesh.primitive_cube_add(size=1, location=(mid_x, mid_y, floor_level))
            corridor = bpy.context.active_object
            corridor.scale = (width / 1, length / 2, self.config.corridor_height / 2)
            corridor.name = f"Corridor_V_{x1}_{y1}_{x2}_{y2}_{z}"
        
        corridor.data.materials.append(self.materials.get_corridor_material())
    
    def create_barrier(self, x: float, y: float, z: int, dx: int = 0, dy: int = 1) -> None:
        """Create a door near a room edge, oriented toward that room."""
        import math

        spacing = self.config.room_size * self.config.room_spacing
        loc_x = x * spacing
        loc_y = y * spacing

        base_z = z * (self.config.wall_height + 1)
        floor_top = self.pos_calc.get_floor_top(base_z)
        loc_z = floor_top + self.config.wall_height / 2

        bpy.ops.mesh.primitive_cube_add(size=2, location=(loc_x, loc_y, loc_z))
        door = bpy.context.active_object
        door.name = f"Door_{x:.2f}_{y:.2f}_{z}"

        # Handle rotation and scale by direction
        if dx != 0:
            # Horizontal corridor
            door.scale = (
                self.config.wall_thickness / 2,
                self.config.corridor_width / 2,
                self.config.wall_height / 2,
            )
            if dx >= 0:
                door.rotation_euler[2] = 0            # +X
            else:
                door.rotation_euler[2] = math.pi      # -X
        if dy != 0:
            # Vertical corridor
            door.scale = (
                self.config.corridor_width / 2,
                self.config.wall_thickness / 2,
                self.config.wall_height / 2,
            )
            if dy > 0:
                door.rotation_euler[2] = 0   # +Y
            else:
                door.rotation_euler[2] = -math.pi  # -Y

        door.data.materials.append(self.materials.get_door_material())

    def create_stairs(self, x: int, y: int, z: int) -> None:
        """Create stairs in a room."""
        ox, oy, base_z = self.pos_calc.get_room_position(x, y, z)
        floor_top = self.pos_calc.get_floor_top(base_z)
        # Position stairs center so bottom sits exactly on floor
        oz = floor_top + self.config.wall_height / 2
        
        bpy.ops.mesh.primitive_cube_add(size=1, location=(ox, oy, oz))
        stairs = bpy.context.active_object
        stairs.scale = (0.5, 0.5, self.config.wall_height / 2)
        stairs.name = f"Stairs_{x}_{y}_{z}"
        stairs.data.materials.append(self.materials.get_stairs_material())
    
    def create_key(self, x: int, y: int, z: int) -> None:
        """Create a key object in a room."""
        ox, oy, base_z = self.pos_calc.get_room_position(x, y, z)
        floor_top = self.pos_calc.get_floor_top(base_z)
        # Position key floating slightly above the floor surface
        oz = floor_top + 0.5
        
        bpy.ops.mesh.primitive_uv_sphere_add(radius=0.3, location=(ox, oy, oz))
        key = bpy.context.active_object
        key.name = f"Key_{x}_{y}_{z}"
        key.data.materials.append(self.materials.get_key_material())


class DungeonGenerator:
    """Main class that orchestrates dungeon generation."""
    
    def __init__(self, config: Optional[DungeonConfig] = None):
        self.config = config or DungeonConfig()
        self.position_calc = PositionCalculator(self.config)
        self.graph_gen = GraphGenerator(self.config)
        self.mesh_creator = MeshCreator(self.config, self.position_calc)
        
        # Set random seed
        random.seed(self.config.seed)
    
    def clear_scene(self) -> None:
        """Clear all objects from the Blender scene."""
        bpy.ops.object.select_all(action='SELECT')
        bpy.ops.object.delete(use_global=False)
    
    def generate_dungeon(self) -> Dict[int, List[Tuple[int, int]]]:
        """Generate the complete dungeon."""
        self.clear_scene()
        floor_data = {}
        
        for floor in range(self.config.floors):
            floor_rooms = self._generate_floor(floor)
            floor_data[floor] = floor_rooms
            
            # Add stairs to connect floors
            if floor < self.config.floors - 1 and floor_rooms:
                self._add_stairs_connection(floor, floor_data)
        
        # Add key on top floor
        self._add_key_on_top_floor(floor_data)
        
        return floor_data
    
    def _generate_floor(self, floor: int) -> List[Tuple[int, int]]:
        """Generate a single floor of the dungeon."""
        G, room_list = self.graph_gen.generate_floor_graph()
        
        # Create rooms
        for (x, y) in room_list:
            connected_dirs = self._get_connected_directions(x, y, room_list)
            self.mesh_creator.create_room(x, y, z=floor, connected_dirs=connected_dirs)
        
        # Create corridors and barriers
        for (n1, n2) in G.edges:
            x1, y1 = n1
            x2, y2 = n2
            self.mesh_creator.create_corridor(x1, y1, floor, x2, y2)
            
            # Randomly add barriers/doors
            if random.random() < self.config.barrier_probability:
                # Position door slightly inside (x1, y1) room
                offset = 0.4  # how far to push toward the room
                mx = x2 - offset * (x2 - x1)
                my = y2 - offset * (y2 - y1)
                dx = x1 - x2  # direction pointing TOWARD room
                dy = y1 - y2
                self.mesh_creator.create_barrier(mx, my, floor, dx=dx, dy=dy)
        
        return room_list
    
    def _get_connected_directions(self, x: int, y: int, 
                                 room_list: List[Tuple[int, int]]) -> Set[str]:
        """Get the directions in which a room has connections."""
        connections = set()
        neighbors = {
            "E": (x + 1, y),
            "W": (x - 1, y),
            "N": (x, y + 1),
            "S": (x, y - 1),
        }
        
        for dir_name, (nx_, ny_) in neighbors.items():
            if (nx_, ny_) in room_list:
                connections.add(dir_name)
        
        return connections
    
    def _add_stairs_connection(self, floor: int, floor_data: Dict[int, List[Tuple[int, int]]]) -> None:
        """Add stairs to connect floors."""
        current_floor_rooms = floor_data[floor]
        if not current_floor_rooms:
            return
        
        stair_room = random.choice(current_floor_rooms)
        self.mesh_creator.create_stairs(*stair_room, z=floor)
        
        # Ensure the stair room exists on the next floor
        if floor + 1 not in floor_data:
            floor_data[floor + 1] = []
        
        if stair_room not in floor_data[floor + 1]:
            floor_data[floor + 1].append(stair_room)
            self.mesh_creator.create_room(*stair_room, z=floor + 1)
    
    def _add_key_on_top_floor(self, floor_data: Dict[int, List[Tuple[int, int]]]) -> None:
        """Add a key on the top floor."""
        top_floor = self.config.floors - 1
        if top_floor in floor_data and floor_data[top_floor]:
            key_room = random.choice(floor_data[top_floor])
            self.mesh_creator.create_key(*key_room, z=top_floor)


def main():
    """Main function to generate the dungeon."""
    # Create custom configuration if needed
    config = DungeonConfig()
    
    # Generate dungeon
    generator = DungeonGenerator(config)
    floor_data = generator.generate_dungeon()
    
    print(f"Dungeon generated with {len(floor_data)} floors")
    for floor, rooms in floor_data.items():
        print(f"Floor {floor}: {len(rooms)} rooms")


# Run the script
if __name__ == "__main__":
    main()
