package laygl

import "github.com/go-gl/mathgl/mgl32"

type Light struct {
	Position mgl32.Vec3
	Ambient mgl32.Vec3
	Diffuse mgl32.Vec3
	Specular mgl32.Vec3
}

func DefaultLight() Light {
	return Light{
		Ambient: mgl32.Vec3{0.2, 0.2, 0.2},
		Diffuse: mgl32.Vec3{1.0, 1.0, 1.0},
		Specular: mgl32.Vec3{0.85, 0.85, 0.85},
	}
}