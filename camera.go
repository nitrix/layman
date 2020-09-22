package laygl

import "github.com/go-gl/mathgl/mgl32"

type Camera struct {
	position   mgl32.Vec3
	direction  mgl32.Vec3
	viewMatrix mgl32.Mat4
}

func NewCamera() *Camera {
	camera := &Camera{}
	camera.updateViewMatrix()
	return camera
}

func (c *Camera) MoveAt(x, y, z float32) {
	c.position = mgl32.Vec3{x, y, z}
	c.updateViewMatrix()
}

func (c *Camera) LookAt(x, y, z float32) {
	c.direction = mgl32.Vec3{x, y, z}
	c.updateViewMatrix()
}

func (c *Camera) RotateX(radians float32) {
	c.viewMatrix = mgl32.Translate3D(c.position.X(), c.position.Y(), c.position.Z()).Mul4(c.viewMatrix)
	c.viewMatrix = mgl32.Rotate3DX(radians).Mat4().Mul4(c.viewMatrix)
	c.viewMatrix = mgl32.Translate3D(-c.position.X(), -c.position.Y(), -c.position.Z()).Mul4(c.viewMatrix)
}

func (c *Camera) RotateY(radians float32) {
	c.viewMatrix = mgl32.Translate3D(c.position.X(), c.position.Y(), c.position.Z()).Mul4(c.viewMatrix)
	c.viewMatrix = mgl32.Rotate3DY(radians).Mat4().Mul4(c.viewMatrix)
	c.viewMatrix = mgl32.Translate3D(-c.position.X(), -c.position.Y(), -c.position.Z()).Mul4(c.viewMatrix)
}

func (c *Camera) RotateZ(radians float32) {
	c.viewMatrix = mgl32.Translate3D(c.position.X(), c.position.Y(), c.position.Z()).Mul4(c.viewMatrix)
	c.viewMatrix = mgl32.Rotate3DZ(radians).Mat4().Mul4(c.viewMatrix)
	c.viewMatrix = mgl32.Translate3D(-c.position.X(), -c.position.Y(), -c.position.Z()).Mul4(c.viewMatrix)
}

func (c *Camera) updateViewMatrix() {
	c.viewMatrix = mgl32.LookAtV(
		c.position,
		c.direction,
		mgl32.Vec3{0, 1, 0},
	)
}
