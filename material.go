package laygl

import "github.com/go-gl/mathgl/mgl32"

type Material struct {
	Ambient mgl32.Vec3
	Diffuse mgl32.Vec3
	Specular mgl32.Vec3
	Shininess float32
}

func DefaultMaterial() Material {
	return Material{
		Ambient: mgl32.Vec3{0.0, 0.0, 0.0},
		Diffuse: mgl32.Vec3{1.0, 1.0, 1.0},
		Specular: mgl32.Vec3{0.65, 0.65, 0.65},
		Shininess: 20,
	}
}