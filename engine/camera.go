package engine

import "github.com/go-gl/mathgl/mgl32"

type Camera struct {
	position mgl32.Vec3
	direction mgl32.Vec3
	view mgl32.Mat4
}

func NewCamera() *Camera {
	camera := &Camera{}
	camera.updateView()
	return camera
}

func (c *Camera) MoveAt(position mgl32.Vec3) {
	c.position = position
	c.updateView()
}

func (c *Camera) LookAt(direction mgl32.Vec3) {
	c.direction = direction
	c.updateView()
}

func (c *Camera) RotateX(angle float32) {
	c.view = c.view.Mul4(mgl32.Translate3D(c.position.X(), c.position.Y(), c.position.Z()))
	c.view = c.view.Mul4(mgl32.Rotate3DX(angle).Mat4())
	c.view = c.view.Mul4(mgl32.Translate3D(-c.position.X(), -c.position.Y(), -c.position.Z()))
}

func (c *Camera) RotateY(angle float32) {
	c.view = c.view.Mul4(mgl32.Translate3D(c.position.X(), c.position.Y(), c.position.Z()))
	c.view = c.view.Mul4(mgl32.Rotate3DY(angle).Mat4())
	c.view = c.view.Mul4(mgl32.Translate3D(-c.position.X(), -c.position.Y(), -c.position.Z()))
}

func (c *Camera) RotateZ(angle float32) {
	c.view = c.view.Mul4(mgl32.Translate3D(c.position.X(), c.position.Y(), c.position.Z()))
	c.view = c.view.Mul4(mgl32.Rotate3DZ(angle).Mat4())
	c.view = c.view.Mul4(mgl32.Translate3D(-c.position.X(), -c.position.Y(), -c.position.Z()))
}

func (c *Camera) updateView() {
	c.view = mgl32.LookAtV(
		c.position,
		c.direction,
		mgl32.Vec3{0, 1, 0},
	)
}