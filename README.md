## Layman OpenGL (laygl)

A simple 3D engine for the layman.

![Screenshot](docs/screenshot.png)

### Currently implemented

- Creating a window and renderer with a modern OpenGL context.
- High resolution clock for elapsed-time calculation.
- Support .obj file format.
- Phong lighting (ambient + diffuse + specular).
- Materials (ambient + diffuse + specular + shininess).
- Back-face culling optimization.
- Anti-aliasing (MSAA 4x multi-sampling).
- Normal maps.
- Roughness/specular maps.
- Emission maps.
- Wireframe mode.
- Mipmapping / anisotropic filtering.
- Flat terrain.

### Short term

- Multiple lights
- Point lights

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
