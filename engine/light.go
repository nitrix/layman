package engine

import "github.com/go-gl/mathgl/mgl32"

type Light struct {
	Position mgl32.Vec3
	Ambient mgl32.Vec3
	Diffuse mgl32.Vec3
	Specular mgl32.Vec3
}
