package laygl

import "github.com/go-gl/mathgl/mgl32"

type Camera struct {
	position  mgl32.Vec3
	direction mgl32.Vec3
	view      mgl32.Mat4
}

func NewCamera() *Camera {
	camera := &Camera{}
	camera.updateView()
	return camera
}

func (c *Camera) MoveAt(x, y, z float32) {
	c.position = mgl32.Vec3{x, y, z}
	c.updateView()
}

func (c *Camera) LookAt(x, y, z float32) {
	c.direction = mgl32.Vec3{x, y, z}
	c.updateView()
}

func (c *Camera) RotateX(radians float32) {
	c.view = mgl32.Translate3D(c.position.X(), c.position.Y(), c.position.Z()).Mul4(c.view)
	c.view = mgl32.Rotate3DX(radians).Mat4().Mul4(c.view)
	c.view = mgl32.Translate3D(-c.position.X(), -c.position.Y(), -c.position.Z()).Mul4(c.view)
}

func (c *Camera) RotateY(radians float32) {
	c.view = mgl32.Translate3D(c.position.X(), c.position.Y(), c.position.Z()).Mul4(c.view)
	c.view = mgl32.Rotate3DY(radians).Mat4().Mul4(c.view)
	c.view = mgl32.Translate3D(-c.position.X(), -c.position.Y(), -c.position.Z()).Mul4(c.view)
}

func (c *Camera) RotateZ(radians float32) {
	c.view = mgl32.Translate3D(c.position.X(), c.position.Y(), c.position.Z()).Mul4(c.view)
	c.view = mgl32.Rotate3DZ(radians).Mat4().Mul4(c.view)
	c.view = mgl32.Translate3D(-c.position.X(), -c.position.Y(), -c.position.Z()).Mul4(c.view)
}

func (c *Camera) updateView() {
	c.view = mgl32.LookAtV(
		c.position,
		c.direction,
		mgl32.Vec3{0, 1, 0},
	)
}
