# M3 - Procedural Dungeon Generation

This project demonstrates procedural dungeon generation techniques using Blender Python API. The implementation includes two different approaches for generating 3D dungeon layouts with rooms, corridors, walls, doors, and interactive elements.

## 📁 Project Structure

```txt
m3/
├── README.md                                    # This file
├── dungeon_procedural_generation.blend         # Blender project file
├── dungeon_procedural_generation.blend1        # Blender backup file
├── procedural_gen_cellular.py                  # Simple cellular automata approach
├── procedural_gen_graph_with_pathfinding.py    # Advanced graph-based approach
└── Procedural Dungeon Generation: A Survey.pdf # Reference documentation
```

## 🎯 Features

### Common Features (Both Scripts)

- **Procedural Generation**: Automatically creates dungeon layouts
- **3D Visualization**: Full 3D geometry with rooms, walls, and corridors
- **Material System**: Different materials for floors, walls, doors, and objects
- **Randomization**: Configurable seed-based generation
- **Interactive Elements**: Keys and doors for gameplay mechanics

### Script-Specific Features

#### `procedural_gen_cellular.py` - Simple Approach

- Grid-based room placement (4x4 grid)
- 70% probability for room generation
- Simple corridor connections between adjacent rooms
- Random door placement with 30% probability
- Single key placement

#### `procedural_gen_graph_with_pathfinding.py` - Advanced Approach

- **Object-Oriented Design**: Modular, extensible architecture
- **Graph-Based Generation**: Uses NetworkX for room connectivity
- **Multi-Floor Support**: Generate dungeons with multiple levels
- **Advanced Positioning**: Precise 3D positioning system
- **Material Management**: Centralized material creation and assignment
- **Configurable Parameters**: Extensive customization options
- **Pathfinding Ready**: Graph structure suitable for AI pathfinding

## 🔧 Requirements

### Software

- **Blender 3.0+** with Python scripting enabled
- **Python 3.9+** (included with Blender)

### Python Dependencies (for advanced script)

- `networkx` - Graph data structures and algorithms
- `random` - Random number generation
- `math` - Mathematical operations
- `typing` - Type hints support

To install NetworkX in Blender's Python environment:

```python
# From Blender's Python console or script
import subprocess
import os
import sys

# Define target modules path explicitly
modules_path = os.path.expanduser('~/.config/blender/4.4/scripts/modules')

# Create folder if it doesn't exist
os.makedirs(modules_path, exist_ok=True)

# Install networkx using Blender's own Python executable
subprocess.check_call([
    sys.executable, '-m', 'pip', 'install', '--target=' + modules_path, 'networkx'
])

print(f"NetworkX successfully installed to: {modules_path}")
```

REMEMBER TO RESTART BLENDER AFTER INSTALLING NETWORKX OR ANY PACKAGES TO REFLECT CHANGES.

## 🚀 Usage

### Method 1: Run in Blender Script Editor

1. Open Blender
2. Switch to **Scripting** workspace
3. Create a new text file or open one of the Python scripts
4. Click **Run Script** button

### Method 2: Run from Blender Command Line

```bash
blender --python procedural_gen_cellular.py
# or
blender --python procedural_gen_graph_with_pathfinding.py
```

### Method 3: Load Existing Project

1. Open `dungeon_procedural_generation.blend` in Blender
2. The generated dungeon should already be visible
3. Re-run scripts to generate new layouts

## ⚙️ Configuration

### Simple Script (`procedural_gen_cellular.py`)

Edit these variables at the top of the script:

```python
ROOM_SIZE = 6        # Size of each room
WALL_HEIGHT = 3      # Height of walls
GRID_SIZE = 4        # Grid dimensions (4x4)
SEED = 42           # Random seed for reproducibility
```

### Advanced Script (`procedural_gen_graph_with_pathfinding.py`)

Modify the `DungeonConfig` class:

```python
class DungeonConfig:
    def __init__(self):
        # Grid and probability settings
        self.grid_size = 4                    # Dungeon grid size
        self.room_probability = 0.7           # Chance of room generation
        self.floors = 1                       # Number of floors
        self.barrier_probability = 0.3        # Door placement probability

        # Dimensional settings
        self.room_size = 6                    # Room dimensions
        self.wall_height = 5                  # Wall height
        self.corridor_width = 2               # Corridor width

        # Random seed
        self.seed = 42                        # For reproducible generation
```

## 🏗️ Architecture (Advanced Script)

### Core Classes

1. **`DungeonConfig`**: Configuration management
2. **`MaterialManager`**: Blender material creation and management
3. **`PositionCalculator`**: 3D position calculations for precise placement
4. **`GraphGenerator`**: Graph-based room connectivity using NetworkX
5. **`MeshCreator`**: 3D mesh creation for all dungeon elements
6. **`DungeonGenerator`**: Main orchestrator class

### Generation Process

1. **Initialization**: Set up configuration and clear scene
2. **Graph Generation**: Create room connectivity graph
3. **Room Creation**: Generate 3D rooms with floors and walls
4. **Corridor Creation**: Connect rooms with walkable corridors
5. **Barrier Placement**: Add doors and obstacles
6. **Special Elements**: Place stairs (multi-floor) and keys

## 🎮 Generated Elements

### Geometry

- **Rooms**: Cubed spaces with floors and surrounding walls
- **Corridors**: Connecting pathways between rooms
- **Walls**: Boundaries and barriers
- **Doors**: Blocking elements with distinct materials
- **Stairs**: Multi-floor connections (advanced script only)

### Interactive Objects

- **Keys**: Collectible spheres with distinct coloring
- **Doors**: Obstacles that could require keys in gameplay

### Materials

- Floor material (gray)
- Wall material (brown)
- Door material (red)
- Key material (yellow/gold)
- Corridor material (light gray)
- Stairs material (blue - advanced script only)

## 📚 Educational Context

This project is part of a Computer Graphics course (M3 module) and demonstrates:

- **Procedural Content Generation**: Algorithmic creation of game content
- **3D Modeling with Code**: Programmatic mesh creation
- **Graph Theory Applications**: Using graphs for spatial relationships
- **Material and Rendering**: Blender material system
- **Object-Oriented Programming**: Clean, maintainable code architecture
- **Random Generation**: Controlled randomness for varied outputs

## 🔍 Technical References

- The included PDF "Procedural Dungeon Generation: A Survey.pdf" provides theoretical background
- NetworkX documentation for graph algorithms
- Blender Python API documentation for 3D operations

## 🎨 Customization Ideas

- Add different room shapes (circular, L-shaped)
- Implement themed dungeons (medieval, sci-fi, horror)
- Add more complex door mechanics
- Integrate lighting systems
- Create animated elements
- Export to game engines (Unity, Godot)
- Add procedural textures
- Implement BSP (Binary Space Partitioning) algorithms

## 🐛 Troubleshooting

### Common Issues

1. **NetworkX not found**: Install using Blender's pip as shown above
2. **Script doesn't run**: Ensure Blender scripting is enabled
3. **Objects not visible**: Check viewport shading and camera position
4. **Materials not applied**: Verify material nodes are enabled in Blender

### Performance Tips

- Reduce `grid_size` for faster generation
- Lower `wall_height` for simpler geometry
- Disable `floors > 1` for single-level testing

---
