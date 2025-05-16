# 🐘 3D OBJ Loader - Elephant with Lighting & Transformations

This project loads and renders a 3D model from a `.obj` file using OpenGL (in C++). It supports **lighting**, **normals**, and real-time **keyboard interaction** for transformations using OpenGL's built-in functions (`glTranslate`, `glRotate`, `glScale`).

This project allows to render complex 3D geometry with shading and interaction.

---

## 📦 Features

- ✅ Loads `.obj` files:
  - Vertices (`v`)
  - Normals (`vn`)
  - Texture coordinates (`vt`)
  - Faces (`f`) (supports triangulation from polygons)
- ✅ Renders as filled **triangles**
- ✅ **3-point lighting** (Phong model: ambient + diffuse + specular)
- ✅ Mouse & keyboard interaction:
  - Rotate, translate, scale, zoom
  - Reset transformations
- ✅ Toggle individual lights (RGB)
- ✅ Lighting mode switch: **fixed** or **follows the model**

---

## 💡 Lighting

- Three configurable light sources:
  - Red (front), Green (left), Blue (top)
- Each light supports:
  - Ambient
  - Diffuse
  - Specular components
- Toggle with keys: `1`, `2`, `3`
- Lighting modes:
  - `f`: Fixed in world space
  - `m`: Attached to model (follows rotation/position)

---

## 🎮 Controls

### 🧭 Rotation

- `W / S` — Rotate model up/down (X-axis)
- `A / D` — Rotate model left/right (Y-axis)
- `U / O` — Rotate model counter-/clockwise (Z-axis)

### ✋ Translation

- `I / K` — Move model up/down
- `J / L` — Move model left/right
- `Z / X` — Move model closer/further (Z-axis)

### 🔍 Zoom / Scale

- `+` — Zoom in
- `-` — Zoom out
- Mouse scroll — Zoom in/out

### 💡 Lighting

- `1`, `2`, `3` — Toggle Red, Green, and Blue lights
- `F` — Lighting fixed in world space
- `M` — Lighting follows model

### 🖱️ Mouse Interactions

- **Left-drag** — Rotate model
- **Right-drag** — Translate model
- **Scroll wheel** — Zoom in/out

### ⏹ Other

- `SPACE` — Reset all transformations (position, rotation, zoom)
- `ESC` — Exit the program

---

## 🚀 How to Compile and Run

Make sure you have OpenGL and GLUT installed. Then, compile the code with:

```bash
g++ main.cpp -o obj_viewer -lGL -lGLU -lglut
```

## Observations

Only the following models have the vt, for texture loading:

- radar.obj and radar-fixed-center-point.obj
- mba1.obj
- tie-fighter.obj
