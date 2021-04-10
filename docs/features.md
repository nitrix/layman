# Features

## Client features

### Currently

- Physically Based Rendering (metallic/roughness and specular/glossiness workflows).
- Tone mapping (Linear, Uncharted, Hejl Richard, ACES).
- Image-Based Lighting (IBL).
- Analytical lights (punctual, directional, spot).
- Mipmapping and anisotropic filtering.
- Multisample anti-aliasing (MSAA).
- Supports glTF 2.0 file format.
- Supports the KHR_materials_unlit extension.
- Orbital/3rd person/free camera.
- Skybox.

### Planned

- Support the KHR_lights_punctual extension.
- Support assets streaming/lazy loading.
- Cutscenes.
- Self updates.

## Server features

### Planned

- Multiplayer support (authentication, playing, saving).
- Distributed infrastructure, self-healing.
- Multiple characters and concurrent connections per account.
- Seamless infinite worlds.

## Editor features

### Currently

- Mouse picking.
- Translation/rotation/scale gizmo.
- Debugging (inspecting entities, textures, wireframe, etc).

### Planned

- World editor.
- Shader editor (node-based system).
- Script editor (event-based system).
- Management of assets (models, sounds, etc).
- Management of player-generated data.
- ACL access list, with permissions.
- Version control of changes and real-time in-game deployments.

## Other features

See [ideas](docs/ideas.md) for a bucket list of ideas to look into.

### References

- [Real Shading in Unreal Engine 4](http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf) by Epic Games.
- [Physically Based Shading](http://blog.selfshadow.com/publications/s2012-shading-course/burley/s2012_pbs_disney_brdf_notes_v3.pdf) by Disney
- [Environment Maps](https://github.com/KhronosGroup/glTF-WebGL-PBR/#environment-maps) by Kronos.
- [An Inexpensive BRDF Model for Physically based Rendering](https://www.cs.virginia.edu/~jdl/bib/appearance/analytic%20models/schlick94b.pdf) by Christophe Schlick.