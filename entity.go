package laygl

import (
	"github.com/go-gl/mathgl/mgl32"
)

type Entity struct {
	model *Model

	translation mgl32.Vec3
	rotation    mgl32.Vec3
	scale       float32

	transform mgl32.Mat4
}

func NewEntityFromModel(model *Model) *Entity {
	return &Entity{
		model:       model,
		scale:       1,
		translation: mgl32.Vec3{0, 0, 0},
		rotation:    mgl32.Vec3{0, 0, 0},
		transform:   model.initialTransform,
	}
}

func (e *Entity) Translate(dx, dy, dz float32) {
	e.translation = e.translation.Add(mgl32.Vec3{dx, dy, dz})
	e.updateTransform()
}

// Using radians, so a complete rotation is 2*Pi.

func (e *Entity) Rotate(rx, ry, rz float32) {
	e.rotation = e.rotation.Add(mgl32.Vec3{rx, ry, rz})
	e.updateTransform()
}

func (e *Entity) Scale(s float32) {
	e.scale *= s
	e.updateTransform()
}

func (e *Entity) updateTransform() {
	e.transform = mgl32.Ident4()
	e.transform = e.transform.Mul4(mgl32.Translate3D(e.translation.X(), e.translation.Y(), e.translation.Z()))
	e.transform = e.transform.Mul4(mgl32.HomogRotate3D(e.rotation.Z(), mgl32.Vec3{0, 0, 1}))
	e.transform = e.transform.Mul4(mgl32.HomogRotate3D(e.rotation.Y(), mgl32.Vec3{0, 1, 0}))
	e.transform = e.transform.Mul4(mgl32.HomogRotate3D(e.rotation.X(), mgl32.Vec3{1, 0, 0}))
	e.transform = e.transform.Mul4(mgl32.Scale3D(e.scale, e.scale, e.scale))
	e.transform = e.transform.Mul4(e.model.initialTransform)
}
