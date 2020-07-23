package engine

import "C"
import (
	"github.com/go-gl/gl/v4.1-core/gl"
	"github.com/go-gl/mathgl/mgl32"
	"unsafe"
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

func (r *Renderer) Render(scene *Scene) {
	gl.Clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT)

	r.renderEntities(scene)
	r.renderTerrains(scene)
}

func (r *Renderer) renderEntities(scene *Scene) {
	for model, entities := range scene.entities {
		model.Use()

		// Prepare the shader.
		model.shader.BindUniformProjection(r.projection)
		model.shader.BindUniformCamera(scene.activeCamera)
		model.shader.BindUniformLight(scene.activeLight)
		model.shader.BindUniformMaterial(model.material)
		model.shader.BindUniformTextureSamplers(model.albedoTexture, model.normalMapTexture, model.roughnessMapTexture, model.emissionMapTexture)

		for _, entity := range entities {
			// The entity transform is the only thing that we should be updating for each entity.
			model.shader.BindUniformTransform(&entity.transform)

			gl.DrawElements(gl.TRIANGLES, entity.model.mesh.faceCount, gl.UNSIGNED_INT, gl.PtrOffset(0))
		}

		model.Unuse()
	}
}

func (r *Renderer) renderTerrains(scene *Scene) {
	for _, terrain := range scene.terrains {
		terrain.Use()

		// Prepare the shader.
		terrain.shader.BindUniformProjection(r.projection)
		terrain.shader.BindUniformCamera(scene.activeCamera)
		terrain.shader.BindUniformLight(scene.activeLight)
		terrain.shader.BindUniformMaterial(&terrain.material)
		terrain.shader.BindUniformTransform(&terrain.transform)
		terrain.shader.BindUniformTextureSamplers(terrain.albedoTexture, terrain.normalMapTexture, terrain.roughnessMapTexture, terrain.emissionMapTexture)

		gl.DrawElements(gl.TRIANGLES, terrain.mesh.faceCount, gl.UNSIGNED_INT, gl.PtrOffset(0))

		terrain.Unuse()
	}
}

func IsExtensionSupported(name string) bool {
	nb := int32(0)
	gl.GetIntegerv(gl.NUM_EXTENSIONS, &nb)

	for i := uint32(0); i < uint32(nb); i++ {
		str := gl.GetStringi(gl.EXTENSIONS, i)
		str2 := C.GoString((*C.char)(unsafe.Pointer(str)))
		if str2 == name {
			return true
		}
	}

	return false
}