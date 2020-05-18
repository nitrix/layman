package engine

import (
	"github.com/go-gl/gl/v4.1-core/gl"
	"github.com/go-gl/glfw/v3.3/glfw"
	"github.com/go-gl/mathgl/mgl32"
)

type Renderer struct {
	projection mgl32.Mat4

	// TODO: Temporary
	angle float64
	previousTime float64
}

func NewRenderer(w *Window) (*Renderer, error) {
	renderer := &Renderer{}

	width, height := w.Dimensions()
	renderer.projection = mgl32.Perspective(mgl32.DegToRad(45.0), float32(width) / float32(height), 0.1, 10.0)

	gl.Enable(gl.DEPTH_TEST)
	gl.DepthFunc(gl.LESS)
	gl.ClearColor(0.0, 0.0, 0.0, 1.0)

	gl.Enable(gl.CULL_FACE)
	gl.CullFace(gl.BACK)

	renderer.previousTime = glfw.GetTime()

	return renderer, nil
}

func (r *Renderer) Wireframe(enabled bool) {
	if enabled {
		gl.PolygonMode(gl.FRONT_AND_BACK, gl.LINE)
		// gl.PolygonMode(gl.FRONT_AND_BACK, gl.POINT)
	} else {
		gl.PolygonMode(gl.FRONT_AND_BACK, gl.FILL)
	}
}

func (r *Renderer) Render(shader *Shader, texture *Texture, camera *Camera, light *Light, material *Material, model *Model) {
	gl.Clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT)

	shader.Use()
	texture.Use()
	model.Use()

	// TODO: These should be more strongly typed.
	shader.BindUniformProjection(r.projection)
	shader.BindUniformView(camera.view)
	shader.BindUniformTransform(model.transform)
	shader.BindUniformTextureSampler()
	shader.BindUniformLight(light)
	shader.BindUniformMaterial(material)

	// TODO: Cleanup below?!

	// Update
	t := glfw.GetTime()
	elapsed := t - r.previousTime
	r.previousTime = t

	r.angle += elapsed
	// r.angle = 4
	model.transform = mgl32.HomogRotate3D(float32(r.angle), mgl32.Vec3{0, 1, 0})
	shader.BindUniformTransform(model.transform)

	// Render
	gl.DrawElements(gl.TRIANGLES, int32(len(model.indices)), gl.UNSIGNED_INT, gl.PtrOffset(0))
}