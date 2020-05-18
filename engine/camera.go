package engine

import "github.com/go-gl/mathgl/mgl32"

type Camera struct {
	view mgl32.Mat4
}

func NewCamera() *Camera {
	camera := &Camera{}

	camera.view = mgl32.LookAtV(
		mgl32.Vec3{5, -0.3, 5},
		mgl32.Vec3{0, -0.3, 0},
		mgl32.Vec3{0, 1, 0},
	)

	return camera
}