package laygl

import "github.com/go-gl/mathgl/mgl32"

// TODO: Rename this to directional light.
type Light struct {
	Direction  mgl32.Vec3
	Irradiance mgl32.Vec3
}

func DefaultLight() Light {
	return Light{
		Direction:  mgl32.Vec3{0, 0, -1.0},
		Irradiance: mgl32.Vec3{2.0, 2.0, 2.0},
	}
}
