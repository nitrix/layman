package engine

import (
	"github.com/go-gl/gl/v4.1-core/gl"
	"github.com/go-gl/mathgl/mgl32"
)

type Renderer struct {
	projection mgl32.Mat4
}

func NewRenderer(w *Window) (*Renderer, error) {
	renderer := &Renderer{}

	width, height := w.Dimensions()
	renderer.projection = mgl32.Perspective(mgl32.DegToRad(45.0), float32(width) / float32(height), 0.1, 50.0)

	gl.ClearColor(0.0, 0.0, 0.0, 1.0)

	gl.Enable(gl.DEPTH_TEST)
	gl.DepthFunc(gl.LESS)

	gl.Enable(gl.CULL_FACE)
	gl.CullFace(gl.BACK)

	gl.Enable(gl.MULTISAMPLE)

	return renderer, nil
}

func (r *Renderer) Wireframe(enabled bool) {
	if enabled {
		gl.PolygonMode(gl.FRONT_AND_BACK, gl.LINE)
	} else {
		gl.PolygonMode(gl.FRONT_AND_BACK, gl.FILL)
	}
}

// TODO: Render a Scene instead. This lets us sort by entity models, to
// avoid switching between them too often. Aka, batch rendering.
func (r *Renderer) Render(scene *Scene) {
	gl.Clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT)

	// Iterate through group of entities (grouped by model)
	for model, entities := range scene.entities {
		model.Use()

		model.shader.BindUniformProjection(r.projection)
		model.shader.BindUniformCamera(scene.activeCamera)
		model.shader.BindUniformLight(scene.activeLight)
		model.shader.BindUniformTextureSamplers()

		for _, entity := range entities {
			model.shader.BindUniformMaterial(entity.model.material)
			model.shader.BindUniformEntity(entity)

			gl.DrawElements(gl.TRIANGLES, entity.model.mesh.faceCount, gl.UNSIGNED_INT, gl.PtrOffset(0))
		}

		model.Unuse()
	}
}