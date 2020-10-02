package laygl

import "C"
import (
	"github.com/go-gl/gl/v4.6-core/gl"
	"github.com/go-gl/mathgl/mgl32"
	"unsafe"
)

const AntiAliasingSamples = 8
const IBLSamples = 32

/*
// Number of vertices in skybox_vertices.
const numSkyboxVertices = 36
// Vertex positions of a skybox centered at the origin and extending -1 units in each direction.
var skybox_vertices = []float32{
	-1.0,  1.0, -1.0, -1.0, -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0,
	-1.0, -1.0, 1.0, -1.0, -1.0, -1.0, 1.0, -1.0, -1.0, -1.0, -1.0, 1.0, -1.0, -1.0,
	1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, -1.0, 1.0, 1.0,
	1.0, 1.0, 1.0, 1.0,  1.0, 1.0, 1.0, -1.0, 1.0, -1.0, -1.0, -1.0, -1.0, 1.0, -1.0,
	1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, -1.0, 1.0, -1.0, -1.0, 1.0, -1.0,
	1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, -1.0, 1.0, 1.0, -1.0,
	1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, -1.0, -1.0,
	-1.0, -1.0, 1.0, 1.0, -1.0, 1.0,
}
*/

type Renderer struct {
	projection mgl32.Mat4
	environment *Environment
}

func NewRenderer(window *Window) (*Renderer, error) {
	renderer := &Renderer{}

	renderer.Resize(window.Dimensions())
	window.OnResize(func() {
		renderer.Resize(window.Dimensions())
	})

	gl.ClearColor(0, 0, 0, 1.0)

	gl.Enable(gl.DEPTH_TEST)
	gl.DepthFunc(gl.LESS)

	gl.Enable(gl.CULL_FACE)
	gl.CullFace(gl.BACK)

	gl.Enable(gl.MULTISAMPLE)

	/*
	// Create the VAO on the GPU, then use it.
	gl.GenVertexArrays(1, &renderer.skyboxMesh.vao)
	gl.BindVertexArray(renderer.skyboxMesh.vao)
	gl.GenBuffers(1, &renderer.skyboxMesh.verticesBufferId)
	gl.BindBuffer(gl.ARRAY_BUFFER, renderer.skyboxMesh.verticesBufferId)
	gl.BufferData(gl.ARRAY_BUFFER, numSkyboxVertices * 4, gl.Ptr(&skybox_vertices[0]), gl.STATIC_DRAW)
	gl.EnableVertexAttribArray(ShaderAttributeVertexCoords)
	gl.VertexAttribPointer(ShaderAttributeVertexCoords, 3, gl.FLOAT, false, 3 * 4, gl.PtrOffset(0))

	renderer.skyboxShader, _ = LoadShader("shaders/skybox.vert", "shaders/skybox.frag")
	*/

	environment, err := NewEnvironment()
	if err != nil {
		return nil, err
	}

	renderer.environment = environment

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
	r.projection = mgl32.Perspective(mgl32.DegToRad(45.0), float32(width)/float32(height), 0.1, 1000.0)
	gl.Viewport(0, 0, int32(width), int32(height))
}

func (r *Renderer) Render(scene *Scene) {
	gl.Clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT)

	r.renderEntities(scene)
}

/*
func (r *Renderer) renderSkybox(camera *Camera) {
	// Setup the program and uniforms.
	gl.UseProgram(r.skyboxShader.programId)
	gl.DepthMask(false)

	view_location := r.skyboxShader.findUniformByName("view_transform")
	gl.UniformMatrix4fv(view_location, 1, false, &camera.viewMatrix[0])

	projection_location := r.skyboxShader.findUniformByName("projection_transform")
	gl.UniformMatrix4fv(projection_location, 1, false, &r.projection[0])

	blur_location := r.skyboxShader.findUniformByName("skybox_blur")
	gl.Uniform1f(blur_location, 0.0)

	// Setup the texture.
	gl.BindTexture(gl.TEXTURE_2D, environment->environment_handle)
	environment_location := r.skyboxShader.findUniformByName("environment_map")
	gl.Uniform1i(environment_location, 0)

	// Draw the box.
	gl.BindVertexArray(r.skyboxMesh.vao)
	gl.DrawArrays(gl.TRIANGLES, 0, numSkyboxVertices / 3)
	gl.BindVertexArray(0)
	gl.DepthMask(true)
}
*/

func (r *Renderer) renderEntities(scene *Scene) {
	for model, entities := range scene.entities {
		// Render each meshes of the model.
		for _, mesh := range model.meshes {
			mesh.Use()

			// Prepare the shader.
			mesh.shader.BindUniformProjection(r.projection)
			mesh.shader.BindUniformCamera(scene.activeCamera)
			mesh.shader.BindUniformLights(scene.lights)
			mesh.shader.BindUniformMaterial(mesh.material)
			mesh.shader.BindUniformEnvironment(r.environment)
			mesh.shader.BindUniformTextureSamplers(
				mesh.albedoTexture,
				mesh.normalMapTexture,
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
