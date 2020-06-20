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

	return renderer, nil
}

func (r *Renderer) Wireframe(enabled bool) {
	if enabled {
		gl.PolygonMode(gl.FRONT_AND_BACK, gl.LINE)
	} else {
		gl.PolygonMode(gl.FRONT_AND_BACK, gl.FILL)
	}
}

func (r *Renderer) Render(shader *Shader, textureAlbedo *Texture, textureNormalMap *Texture, camera *Camera, light *Light, material *Material, model *Model) {
	gl.Clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT)

	shader.Use()
	textureAlbedo.Use()
	textureNormalMap.Use()
	model.Use()

	shader.BindUniformProjection(r.projection)
	shader.BindUniformCamera(camera)
	shader.BindUniformModel(model)
	shader.BindUniformLight(light)
	shader.BindUniformMaterial(material)
	shader.BindUniformTextureSamplers()

	gl.DrawElements(gl.TRIANGLES, model.faceCount, gl.UNSIGNED_INT, gl.PtrOffset(0))

	shader.Unuse()
	textureAlbedo.Unuse()
	textureNormalMap.Unuse()
	model.Unuse()
}