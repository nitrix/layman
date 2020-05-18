package engine

import "github.com/go-gl/mathgl/mgl32"

type Material struct {
	Ambient mgl32.Vec3
	Diffuse mgl32.Vec3
	Specular mgl32.Vec3
	Shininess float32
}
