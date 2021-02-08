# Layman Game Engine (layman)

A simple 3D engine for the layman.

![Screenshot](docs/screenshot.png)

## Features

- C11, portable on Windows, Linux and Mac.
- High resolution clock for elapsed-time calculation.
- Physically Based Rendering (metallic/roughness and specular/glossiness workflows).
- Analytical lights and Image-Based Lighting (IBL).
- Supports glTF 2.0 file format.
- Level of detail (mipmapping and anisotropic filtering).
- Multisample Anti-aliasing (MSAA).
- Optimizations (face culling).

## Planned

- Flat terrain.
- Wireframe mode.
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
- Bloom effect
- Multiple render targets
- Geometry shaders
- Cube map reflections
- Lens flare
- Occlusion queries
- Water
- Runtime keybinding system to map actions to GLFW key constants.

## References

### Shading

- [Real Shading in Unreal Engine 4](http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf) by Epic Games.
- [Physically Based Shading](http://blog.selfshadow.com/publications/s2012-shading-course/burley/s2012_pbs_disney_brdf_notes_v3.pdf) by Disney
- [Environment Maps](https://github.com/KhronosGroup/glTF-WebGL-PBR/#environment-maps) by Kronos.
- [An Inexpensive BRDF Model for Physically based Rendering](https://www.cs.virginia.edu/~jdl/bib/appearance/analytic%20models/schlick94b.pdf) by Christophe Schlick.

### glTF extensions

- [KHR_materials_clearcoat](https://github.com/ux3d/glTF/tree/KHR_materials_pbrClearcoat/extensions/2.0/Khronos/KHR_materials_clearcoat) by Kronos.
- [KHR_materials_specular](https://github.com/ux3d/glTF/tree/KHR_materials_pbrClearcoat/extensions/2.0/Khronos/KHR_materials_specular) by Kronos.
- [KHR_materials_subsurface](https://github.com/KhronosGroup/glTF/pull/1766) by Kronos.
- [KHR_materials_thinfilm](https://github.com/ux3d/glTF/tree/extensions/KHR_materials_thinfilm/extensions/2.0/Khronos/KHR_materials_thinfilm) by Kronos.

## License

This is free and unencumbered software released into the public domain. See the [UNLICENSE](UNLICENSE) file for more details.
