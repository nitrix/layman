package laygl

import (
	"github.com/go-gl/mathgl/mgl32"
	"math"
)

type camera struct {
	position   mgl32.Vec3
	rotation   mgl32.Vec3
	viewMatrix mgl32.Mat4
}

func NewCamera() *camera {
	camera := &camera{}
	camera.updateViewMatrix()
	return camera
}

func (c *camera) Move(dx, dy, dz float32) {
	c.position = c.position.Add(mgl32.Vec3{dx, dy, dz})
	c.updateViewMatrix()
}

func (c *camera) MoveForward(distance float32) {
	deltaX := distance * float32(math.Sin(float64(-c.rotation.Y())))
	deltaZ := distance * float32(math.Cos(float64(-c.rotation.Y())))
	c.Move(-deltaX, 0, -deltaZ)
}

func (c *camera) MoveAt(x, y, z float32) {
	c.position = mgl32.Vec3{x, y, z}
	c.updateViewMatrix()
}

func (c *camera) Rotate(x, y, z float32) {
	c.rotation = c.rotation.Add(mgl32.Vec3{x, y, z})
	c.updateViewMatrix()
}

func (c *camera) updateViewMatrix() {
	translation := mgl32.Translate3D(-c.position.X(), -c.position.Y(), -c.position.Z())
	rotationY := mgl32.HomogRotate3DY(c.rotation.Y())
	rotationZ := mgl32.HomogRotate3DZ(c.rotation.Z())

	c.viewMatrix = mgl32.Ident4()
	c.viewMatrix = c.viewMatrix.Mul4(rotationY)
	c.viewMatrix = c.viewMatrix.Mul4(rotationZ)
	c.viewMatrix = c.viewMatrix.Mul4(translation)
}
