## Layman OpenGL (laygl)

A simple 3D engine for the layman.

![Screenshot](docs/screenshot.png)

### Currently implemented

- Creating a window and renderer with a modern OpenGL context.
- High resolution clock for elapsed-time calculation.
- Physically Based Rendering (metallic/roughness workflow).
- Support glTF 2.0 file format.
- Materials.
- Normal maps.
- Anti-aliasing (MSAA 8x multi-sampling).
- Back-face culling optimization.
- Mipmapping / anisotropic filtering.
- Wireframe mode.
- Directional light.

### Short term

- Image Based Lighting (IBL).
- Point lights.
- Flat terrain.

### Long term

- Transparency
- Fog
- Third person camera with tilt-pan-zoom offsets.
- Skeletal animation
- Multi-texturing
- Terrain collision
- Texture atlases
- GUI
- Day/night cycle
- Cell shading
- Font rendering
- Distance field text rendering
- Particle effects
- Instanced rendering
- Procedural terrain
- Shadow mapping
- Percentage closer filtering
- Post-processing effects
- Gaussian Blur
- Multisampled FBOs
- Bloom effect
- Multiple render targets
- Geometry shaders
- Cube map reflections
- Render to cube map texture
- Lens flare
- Occlusion queries
- Water

### Not clear yet

- Runtime keybinding system to map actions to GLFW key constants.
- Associating different materials to different mesh parts of a model.
- Loading materials from files.

## License

This is free and unencumbered software released into the public domain. See the [UNLICENSE](UNLICENSE) file for more details.
