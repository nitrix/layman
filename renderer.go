package laygl

import "C"
import (
	_ "embed"
	"github.com/go-gl/gl/v3.3-core/gl"
	"github.com/go-gl/mathgl/mgl32"
	"unsafe"
)

//go:embed shaders/hdr.vert
var embeddedShaderHdrVert string

//go:embed shaders/hdr.frag
var embeddedShaderHdrFrag string

const AntiAliasingSamples = 8
const IBLSamples = 32

type renderer struct {
	projection mgl32.Mat4
	environment *Environment
	framebuffer *Framebuffer
	hdrShader *Shader
	ditherTexture *Texture
	width, height int
	wireframe bool

	dummyVao uint32
}

var bayerMatrix = []uint8 {
	10, 32, 8, 40, 2, 34, 10, 42,
	48, 16, 56, 24, 50, 18, 58, 26,
	12, 44, 4, 36, 14, 46, 6, 38,
	60, 28, 52, 20, 62, 30, 54, 22,
	3, 35, 11, 43, 1, 33, 9, 41,
	51, 19, 59, 27, 49, 17, 57, 25,
	15, 47, 7, 39, 13, 45, 5, 37,
	63, 31, 55, 23, 61, 29, 53, 21,
}

func NewRenderer(window *window) (*renderer, error) {
	renderer := &renderer{}

	gl.ClearColor(0, 0, 0, 1.0)

	// Culling back faces.
	gl.Enable(gl.CULL_FACE)
	gl.CullFace(gl.BACK)

	gl.Enable(gl.DEPTH_TEST)

	environment, err := NewEnvironment()
	if err != nil {
		return nil, err
	}

	renderer.environment = environment
	renderer.framebuffer = NewFramebuffer(window.Dimensions())
	gl.GenVertexArrays(1, &renderer.dummyVao)

	renderer.hdrShader, err = LoadShaderFromMemory(embeddedShaderHdrVert, embeddedShaderHdrFrag)
	if err != nil {
		return nil, err
	}

	renderer.ditherTexture = &Texture{}

	gl.GenTextures(1, &renderer.ditherTexture.id)
	gl.BindTexture(gl.TEXTURE_2D, renderer.ditherTexture.id)
	gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.REPEAT)
	gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.REPEAT)
	gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST)
	gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST)
	gl.TexImage2D(gl.TEXTURE_2D, 0, gl.R8UI, 8, 8, 0, gl.RED_INTEGER, gl.UNSIGNED_BYTE, unsafe.Pointer(&bayerMatrix[0]))

	renderer.Resize(window.Dimensions())
	window.OnResize(func() {
		renderer.Resize(window.Dimensions())
	})

	return renderer, nil
}

func (r *renderer) Wireframe(enabled bool) {
	r.wireframe = enabled
}

func (r *renderer) Resize(width, height int) {
	r.width, r.height = width, height
	r.projection = mgl32.Perspective(mgl32.DegToRad(45.0), float32(width)/float32(height), 0.1, 1000.0)
	gl.Viewport(0, 0, int32(width), int32(height))

	r.framebuffer.Destroy()
	r.framebuffer = NewFramebuffer(width, height)
}

func (r *renderer) Render(scene *scene) {
	r.renderEntities(scene)
	r.postProcessing()
}

func (r *renderer) postProcessing() {
	gl.PolygonMode(gl.FRONT_AND_BACK, gl.FILL)

	r.hdrShader.Use()
	gl.Clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT)

	gl.ActiveTexture(gl.TEXTURE0)
	gl.BindTexture(gl.TEXTURE_2D_MULTISAMPLE, r.framebuffer.colorBuffer)
	gl.ActiveTexture(gl.TEXTURE1)
	gl.BindTexture(gl.TEXTURE_2D, r.ditherTexture.id)

	gl.Uniform2ui(r.hdrShader.findUniformByName("dimensions"), uint32(r.width), uint32(r.height))
	gl.Uniform1i(r.hdrShader.findUniformByName("hdrBuffer"), 0)
	gl.Uniform1i(r.hdrShader.findUniformByName("bayer_matrix"), 1)

	// Render a quad that covers the entire screen so we can actually use the texture produced by the main shader.
	gl.BindVertexArray(r.dummyVao)
	gl.DrawArrays(gl.TRIANGLES, 0, 3)
	r.hdrShader.Unuse()
}

func (r *renderer) renderEntities(scene *scene) {
	if r.wireframe {
		gl.PolygonMode(gl.FRONT_AND_BACK, gl.LINE)
	}

	r.framebuffer.Use()
	gl.Clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT)

	for model, entities := range scene.entities {
		// Render each meshes of the model.
		for _, mesh := range model.meshes {
			mesh.Use()

			// Prepare the shader.
			mesh.shader.BindUniformProjection(r.projection)
			mesh.shader.BindUniformCamera(scene.camera)
			mesh.shader.BindUniformLights(scene.lights)
			mesh.shader.BindUniformEnvironment(r.environment)
			mesh.shader.BindUniformTextureSamplers(
				mesh.albedoTexture,
				mesh.normalMapTexture,
				mesh.emissiveMapTexture,
				mesh.metallicRoughnessMapTexture,
				mesh.ambientOcclusionMapTexture,
			)

			for _, entity := range entities {
				// The entity transformMatrix is the only thing that we should be updating for each entity.
				mesh.shader.BindUniformTransform(&entity.transformMatrix)

				// FIXME: UNSIGNED_SHORT?
				gl.DrawElements(gl.TRIANGLES, int32(mesh.indiceCount), gl.UNSIGNED_SHORT, gl.PtrOffset(0))
			}

			mesh.Unuse()
		}
	}

	r.framebuffer.Unuse()
}