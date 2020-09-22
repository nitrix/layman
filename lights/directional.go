package lights

import "github.com/go-gl/mathgl/mgl32"

type Directional struct {
	Direction  mgl32.Vec3
	Irradiance mgl32.Vec3
}

func NewDirectional() Directional {
	return Directional{
		Direction:  mgl32.Vec3{0, 0, -1},
		Irradiance: mgl32.Vec3{2, 2, 2},
	}
}

func (n *Directional) isLight() {}