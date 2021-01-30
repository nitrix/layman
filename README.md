# Layman Game Engine (layman)

A simple 3D engine for the layman.

![Screenshot](docs/screenshot.png)

## Features

- MSAA multisampling (4x).
- Anisotropic filtering (16x).

## Planned

- Physically Based Rendering (metallic/roughness workflow).
- Modern deferred OpenGL pipeline.
- Support glTF 2.0 file format.
- Multiple window support.

- Image Based Lighting (IBL).
- Flat terrain.
- Creating a window and renderer with a modern OpenGL context.
- High resolution clock for elapsed-time calculation.
- Physically Based Rendering (metallic/roughness workflow).
- Support glTF 2.0 file format.
- Materials.
- Normal maps.
- Anti-aliasing (MSAA 8x multi-sampling).
- Back-face culling optimization.
- Mipmapping.
- Wireframe mode.
- Directional and point lights.
- High resolution clock for elapsed-time calculation.
- Materials.
- Samplers (wrapping? clamp/linear, etc).
- Normal maps.
- Anti-aliasing (MSAA 8x multi-sampling).
- Back-face culling optimization.
- Mipmapping.
- Wireframe mode.
- Directional and point lights.
- Image Based Lighting (IBL).
- Flat terrain.

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
- Runtime keybinding system to map actions to GLFW key constants.
- Associating different materials to different mesh parts of a model.
- Loading materials from files.


## References

- [Real Shading in Unreal Engine 4](http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf) by Epic Games.
- [Physically Based Shading at Disney](http://blog.selfshadow.com/publications/s2012-shading-course/burley/s2012_pbs_disney_brdf_notes_v3.pdf)
- [README.md - Environment Maps](https://github.com/KhronosGroup/glTF-WebGL-PBR/#environment-maps)
- ["An Inexpensive BRDF Model for Physically based Rendering](https://www.cs.virginia.edu/~jdl/bib/appearance/analytic%20models/schlick94b.pdf) by Christophe Schlick
- ["KHR_materials_clearcoat"](https://github.com/ux3d/glTF/tree/KHR_materials_pbrClearcoat/extensions/2.0/Khronos/KHR_materials_clearcoat)
- ["KHR_materials_specular"](https://github.com/ux3d/glTF/tree/KHR_materials_pbrClearcoat/extensions/2.0/Khronos/KHR_materials_specular)
- ["KHR_materials_subsurface"](https://github.com/KhronosGroup/glTF/pull/1766)
- ["KHR_materials_thinfilm"](https://github.com/ux3d/glTF/tree/extensions/KHR_materials_thinfilm/extensions/2.0/Khronos/KHR_materials_thinfilm)

## License

This is free and unencumbered software released into the public domain. See the [UNLICENSE](UNLICENSE) file for more details.
