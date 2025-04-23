# 🐘 3D OBJ Loader - Elephant with Lighting & Transformations

This project loads and renders a 3D model from a `.obj` file using OpenGL (in C++). It supports **lighting**, **normals**, and real-time **keyboard interaction** for transformations using OpenGL's built-in functions (`glTranslate`, `glRotate`, `glScale`).

This project allows to render complex 3D geometry with shading and interaction.

---

## 📦 Features

- ✅ OBJ file loading with:
  - Vertices (`v`)
  - Normals (`vn`)
  - Texture coordinates (`vt`)
  - Faces (`f`)
- ✅ Rendered as filled **triangles** (not just wireframe)
- ✅ **Phong lighting model** with 3 configurable light sources
- ✅ Interactive **rotation**, **scaling**, and **translation**
- ✅ Lights can be toggled on/off individually

---

## 💡 Lighting

- 3 static point light sources in the scene
- Each light has:
  - Ambient
  - Diffuse
  - Specular components
- Toggle lights with keys `1`, `2`, and `3`

---

## 🎮 Controls

### 🔁 Rotation

- `W / S`: Rotate up/down (X-axis)
- `A / D`: Rotate left/right (Y-axis)

### ✋ Translation

- `I / K`: Move up/down
- `J / L`: Move left/right

### 🔍 Scaling

- `+`: Scale up
- `-`: Scale down

### 💡 Light Toggle

- `1`: Toggle Light 1
- `2`: Toggle Light 2
- `3`: Toggle Light 3

### ⏹ Other

- `ESC`: Exit the program

---

## 🚀 How to Compile and Run

Make sure you have OpenGL and GLUT installed. Then, compile the code with:

```bash
g++ main.cpp -o obj_viewer -lGL -lGLU -lglut
```
