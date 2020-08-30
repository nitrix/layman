package laygl

import (
	"github.com/go-gl/mathgl/mgl32"
)

type Entity struct {
	model *Model
	position mgl32.Vec3
	transform mgl32.Mat4
}

func NewEntityFromModel(model *Model) *Entity {
	return &Entity{
		model: model,
		transform: model.initialTransform,
	}
}

// Using radians, so a complete rotation is 2*Pi.

func (e *Entity) Translate(x, y, z float32) {
	e.position = e.position.Add(mgl32.Vec3{x, y, z})
	e.transform = mgl32.Translate3D(-x, -y, -z).Mul4(e.transform)
}

func (e *Entity) RotateX(radians float32) {
	e.transform = mgl32.Translate3D(e.position.X(), e.position.Y(), e.position.Z()).Mul4(e.transform)
	e.transform = mgl32.HomogRotate3DX(radians).Mul4(e.transform)
	e.transform = mgl32.Translate3D(-e.position.X(), -e.position.Y(), -e.position.Z()).Mul4(e.transform)
}

func (e *Entity) RotateY(radians float32) {
	e.transform = mgl32.Translate3D(e.position.X(), e.position.Y(), e.position.Z()).Mul4(e.transform)
	e.transform = mgl32.HomogRotate3DY(radians).Mul4(e.transform)
	e.transform = mgl32.Translate3D(-e.position.X(), -e.position.Y(), -e.position.Z()).Mul4(e.transform)
}

func (e *Entity) RotateZ(radians float32) {
	e.transform = mgl32.Translate3D(e.position.X(), e.position.Y(), e.position.Z()).Mul4(e.transform)
	e.transform = mgl32.HomogRotate3DZ(radians).Mul4(e.transform)
	e.transform = mgl32.Translate3D(-e.position.X(), -e.position.Y(), -e.position.Z()).Mul4(e.transform)
}

func (e *Entity) Scale(factor float32) {
	e.transform = e.transform.Mul4(mgl32.Scale3D(factor, factor, factor))
}