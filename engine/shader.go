package engine

import (
	"fmt"
	"github.com/go-gl/gl/v4.1-core/gl"
	"github.com/go-gl/mathgl/mgl32"
	"io/ioutil"
	"strings"
)

type Shader struct {
	programId                uint32

	uniformProjection        int32
	uniformView              int32
	uniformTransform         int32

	uniformTextureAlbedoSampler    int32
	uniformTextureNormalMapSampler int32

	uniformLightPosition     int32
	uniformLightAmbient      int32
	uniformLightDiffuse      int32
	uniformLightSpecular     int32

	uniformMaterialAmbient   int32
	uniformMaterialDiffuse   int32
	uniformMaterialSpecular  int32
	uniformMaterialShininess int32
}


const (
	ShaderAttributeVertexCoords = iota
	ShaderAttributeTextureCoords
	ShaderAttributeNormals
	ShaderAttributeTangents
)

func (s *Shader) Use() {
	gl.UseProgram(s.programId)
	s.bindAttributes()
}

func (s *Shader) Unuse() {
	gl.UseProgram(0)
}

func LoadShader(vertexShaderFilepath, fragmentShaderFilepath string) (*Shader, error) {
	shader := &Shader{}

	buf, err := ioutil.ReadFile(vertexShaderFilepath)
	if err != nil {
		return shader, err
	}
	vertexShaderSource := string(buf) + "\x00"

	buf, err = ioutil.ReadFile(fragmentShaderFilepath)
	if err != nil {
		return shader, err
	}
	fragmentShaderSource := string(buf) + "\x00"

	vertexShader, err := compileShader(vertexShaderSource, gl.VERTEX_SHADER)
	if err != nil {
		return shader, err
	}

	fragmentShader, err := compileShader(fragmentShaderSource, gl.FRAGMENT_SHADER)
	if err != nil {
		return shader, err
	}

	shader.programId = gl.CreateProgram()

	gl.AttachShader(shader.programId, vertexShader)
	gl.AttachShader(shader.programId, fragmentShader)
	gl.LinkProgram(shader.programId)

	var status int32
	gl.GetProgramiv(shader.programId, gl.LINK_STATUS, &status)
	if status == gl.FALSE {
		var logLength int32
		gl.GetProgramiv(shader.programId, gl.INFO_LOG_LENGTH, &logLength)

		log := strings.Repeat("\x00", int(logLength+1))
		gl.GetProgramInfoLog(shader.programId, logLength, nil, gl.Str(log))

		return shader, fmt.Errorf("failed to link program: %v", log)
	}

	gl.DeleteShader(vertexShader)
	gl.DeleteShader(fragmentShader)

	shader.findUniforms()

	return shader, nil
}

func (s *Shader) bindAttributes() {
	s.bindAttribute(ShaderAttributeVertexCoords, "position")
	s.bindAttribute(ShaderAttributeTextureCoords, "texture_coords")
	s.bindAttribute(ShaderAttributeNormals, "normal")
	s.bindAttribute(ShaderAttributeTangents, "tangent")
}

func (s *Shader) findUniforms() {
	s.uniformProjection = s.findUniformByName("projection")
	s.uniformView = s.findUniformByName("view")
	s.uniformTransform = s.findUniformByName("transform")
	s.uniformTextureAlbedoSampler = s.findUniformByName("texture_albedo_sampler")
	s.uniformTextureNormalMapSampler = s.findUniformByName("texture_normal_map_sampler")
	s.uniformLightPosition = s.findUniformByName("light_position")
	s.uniformMaterialAmbient = s.findUniformByName("material_ambient")
	s.uniformMaterialDiffuse = s.findUniformByName("material_diffuse")
	s.uniformMaterialSpecular = s.findUniformByName("material_specular")
	s.uniformMaterialShininess = s.findUniformByName("material_shininess")
	s.uniformLightAmbient = s.findUniformByName("light_ambient")
	s.uniformLightDiffuse = s.findUniformByName("light_diffuse")
	s.uniformLightSpecular = s.findUniformByName("light_specular")
}

func (s *Shader) bindAttribute(attribute uint32, name string) {
	gl.BindAttribLocation(s.programId, attribute, gl.Str(name + "\x00"))
}

func (s *Shader) findUniformByName(name string) int32 {
	return gl.GetUniformLocation(s.programId, gl.Str(name + "\x00"))
}

func (s *Shader) BindUniformProjection(projection mgl32.Mat4) {
	gl.UniformMatrix4fv(s.uniformProjection, 1, false, &projection[0])
}

func (s *Shader) BindUniformCamera(camera *Camera) {
	gl.UniformMatrix4fv(s.uniformView, 1, false, &camera.view[0])
}

func (s *Shader) BindUniformModel(model *Model) {
	gl.UniformMatrix4fv(s.uniformTransform, 1, false, &model.transform[0])
}

func (s *Shader) BindUniformLight(light *Light) {
	gl.Uniform3f(s.uniformLightPosition, light.Position.X(), light.Position.Y(), light.Position.Z())

	gl.Uniform3f(s.uniformLightAmbient, light.Ambient.X(), light.Ambient.Y(), light.Ambient.Z())
	gl.Uniform3f(s.uniformLightDiffuse, light.Diffuse.X(), light.Diffuse.Y(), light.Diffuse.Z())
	gl.Uniform3f(s.uniformLightSpecular, light.Specular.X(), light.Specular.Y(), light.Specular.Z())
}

func (s *Shader) BindUniformMaterial(material *Material) {
	gl.Uniform3f(s.uniformMaterialAmbient, material.Ambient.X(), material.Ambient.Y(), material.Ambient.Z())
	gl.Uniform3f(s.uniformMaterialDiffuse, material.Diffuse.X(), material.Diffuse.Y(), material.Diffuse.Z())
	gl.Uniform3f(s.uniformMaterialSpecular, material.Specular.X(), material.Specular.Y(), material.Specular.Z())
	gl.Uniform1f(s.uniformMaterialShininess, material.Shininess)
}

func (s *Shader) BindUniformTextureSamplers() {
	gl.Uniform1i(s.uniformTextureAlbedoSampler, int32(TextureAlbedo))
	gl.Uniform1i(s.uniformTextureNormalMapSampler, int32(TextureNormalMap))
}

func compileShader(source string, shaderType uint32) (uint32, error) {
	shader := gl.CreateShader(shaderType)

	cString, free := gl.Strs(source)
	gl.ShaderSource(shader, 1, cString, nil)
	free()

	gl.CompileShader(shader)

	var status int32
	gl.GetShaderiv(shader, gl.COMPILE_STATUS, &status)

	if status == gl.FALSE {
		var logLength int32
		gl.GetShaderiv(shader, gl.INFO_LOG_LENGTH, &logLength)

		log := strings.Repeat("\x00", int(logLength+1))
		gl.GetShaderInfoLog(shader, logLength, nil, gl.Str(log))

		return 0, fmt.Errorf("failed to compile %v: %v", source, log)
	}

	return shader, nil
}