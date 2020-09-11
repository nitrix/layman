package laygl

import "C"
import (
	"github.com/go-gl/gl/v4.6-core/gl"
	"github.com/go-gl/mathgl/mgl32"
	"unsafe"
)

type Renderer struct {
	projection mgl32.Mat4
}

func NewRenderer(window *Window) (*Renderer, error) {
	renderer := &Renderer{}

	renderer.Resize(window.Dimensions())
	window.OnResize(func(w *Window) {
		renderer.Resize(window.Dimensions())
	})

	gl.ClearColor(0, 0, 0, 1.0)

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

func (r *Renderer) Resize(width, height int) {
	r.projection = mgl32.Perspective(mgl32.DegToRad(45.0), float32(width) / float32(height), 0.1, 50.0)
	gl.Viewport(0, 0, int32(width), int32(height))
}

func (r *Renderer) Render(scene *Scene) {
	gl.Clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT)

	r.renderEntities(scene)
}

func (r *Renderer) renderEntities(scene *Scene) {
	for model, entities := range scene.entities {
		// Render each meshes of the model.
		for _, mesh := range model.meshes {
			mesh.Use()

			// Prepare the shader.
			mesh.shader.BindUniformProjection(r.projection)
			mesh.shader.BindUniformCamera(scene.activeCamera)
			mesh.shader.BindUniformLight(scene.activeLight)
			mesh.shader.BindUniformMaterial(mesh.material)
			mesh.shader.BindUniformTextureSamplers(
				mesh.albedoTexture,
				mesh.normalMapTexture,
				mesh.metallicRoughnessMapTexture,
				mesh.ambientOcclusionMapTexture,
			)

			for _, entity := range entities {
				// The entity transform is the only thing that we should be updating for each entity.
				mesh.shader.BindUniformTransform(&entity.transform)
				mesh.shader.BindUniformNormalTransform()

				// FIXME: UNSIGNED_SHORT?
				gl.DrawElements(gl.TRIANGLES, int32(mesh.indiceCount), gl.UNSIGNED_SHORT, gl.PtrOffset(0))
			}

			mesh.Unuse()
		}
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