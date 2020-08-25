package laygl

import "github.com/go-gl/mathgl/mgl32"

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

func (e *Entity) RotateX(angle float32) {
	e.transform = e.transform.Mul4(mgl32.HomogRotate3DX(angle))
}

func (e *Entity) RotateY(angle float32) {
	e.transform = e.transform.Mul4(mgl32.HomogRotate3DY(angle))
}

func (e *Entity) RotateZ(angle float32) {
	e.transform = e.transform.Mul4(mgl32.HomogRotate3DZ(angle))
}

func (e *Entity) Scale(factor float32) {
	e.transform = e.transform.Mul4(mgl32.Scale3D(factor, factor, factor))
}