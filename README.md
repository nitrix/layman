## Learn OpenGL (learngl)

Just a simple project while I'm learning OpenGL.

![Wakfu example character](docs/screenshot.png)

### Currently implemented

- Creating a window and renderer with a modern OpenGL context.
- Entities have a model, texture, position, rotation and scale.
- Matrix and vector math utilities.
- Support .obj file format.
- Pan-tilt-zoom camera.
- Wireframe toggleable mode.
- Diffuse lighting.
- Specular highlights.

### Short term

- Numerous code refactors
    - Window handling events with camera/renderer imports
    - Toolkit collection/dynarray
    - Renderer pluggable/default perspective
- Drop SOIL, use libpng directly
- Optimizations
    - Turn obj module into loader module
    - Meshes back-face culling
    - Batch rendering

### Long term

- Terrain
- Transparency
- Fog
- Multi-texturing
- Player movement
- 3rd person camera
- Mipmapping
- Terrain collision
- Texture atlases
- GUI
- Multiple lights
- Point lights
- Day/night cycle
- Cell shading
- Normal mapping
- Font rendering
- Distance field text rendering
- Particle effects
- Instanced rendering
- Procedural terrain
- Shadow mapping
- Percentage closer filtering
- Antialiasing and anisotropic filtering
- Post-processing effects
- Gaussian Blur
- Multisampled FBOs
- Specular maps
- Bloom effect
- Multiple render targets
- Geometry shaders
- Cube map reflections
- Render to cube map texture
- Skeletal animation
- Lens flare
- Occlusion queries
- Water