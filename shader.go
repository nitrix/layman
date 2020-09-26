package laygl

import (
	"fmt"
	"github.com/go-gl/gl/v4.6-core/gl"
	"github.com/go-gl/mathgl/mgl32"
	"github.com/nitrix/laygl/lights"
	"io/ioutil"
	"strings"
)

const LightCount = 4

type Shader struct {
	programId uint32

	uniformProjection      int32
	uniformView            int32
	uniformTransform       int32
	uniformNormalTransform int32
	uniformCameraPosition  int32

	albedoMap    UniformMapInfo
	normalMap    UniformMapInfo
	roughnessMap UniformMapInfo
	metallicMap  UniformMapInfo
	aoMap        UniformMapInfo
	emissionMap  UniformMapInfo

	directionalLight DirectionalLightInfo
	pointLights [LightCount]PointLightInfo

	uniformMaterialAmbient   int32
	uniformMaterialDiffuse   int32
	uniformMaterialSpecular  int32
	uniformMaterialShininess int32
}

type UniformMapInfo struct {
	enabledUniformId      int32
	defaultValueUniformId int32
	samplerUniformId      int32
}

type DirectionalLightInfo struct {
	enabledUniformId    int32
	directionUniformId  int32
	irradianceUniformId int32
}

type PointLightInfo struct {
	enabledUniformedId int32
	positionUniformedId int32
	irradianceUniformedId int32
	constantAttenuationUniformedId int32
	linearAttenuationUniformedId int32
	quadraticAttenuationUniformedId int32
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
	s.bindAttribute(ShaderAttributeTextureCoords, "uv")
	s.bindAttribute(ShaderAttributeNormals, "normal")
	s.bindAttribute(ShaderAttributeTangents, "tangent")
}

func (s *Shader) findUniforms() {
	s.uniformProjection = s.findUniformByName("projection_transform")
	s.uniformView = s.findUniformByName("view_transform")
	s.uniformTransform = s.findUniformByName("model_transform")
	s.uniformCameraPosition = s.findUniformByName("camera_position")

	s.albedoMap.samplerUniformId = s.findUniformByName("albedo_map.map")
	s.albedoMap.defaultValueUniformId = s.findUniformByName("albedo_map.default_value")
	s.albedoMap.enabledUniformId = s.findUniformByName("albedo_map.enabled")

	s.normalMap.samplerUniformId = s.findUniformByName("normal_map.map")
	s.normalMap.defaultValueUniformId = s.findUniformByName("normal_map.default_value")
	s.normalMap.enabledUniformId = s.findUniformByName("normal_map.enabled")

	s.roughnessMap.samplerUniformId = s.findUniformByName("roughness_map.map")
	s.roughnessMap.defaultValueUniformId = s.findUniformByName("roughness_map.default_value")
	s.roughnessMap.enabledUniformId = s.findUniformByName("roughness_map.enabled")

	s.metallicMap.samplerUniformId = s.findUniformByName("metallic_map.map")
	s.metallicMap.defaultValueUniformId = s.findUniformByName("metallic_map.default_value")
	s.metallicMap.enabledUniformId = s.findUniformByName("metallic_map.enabled")

	s.aoMap.samplerUniformId = s.findUniformByName("ao_map.map")
	s.aoMap.defaultValueUniformId = s.findUniformByName("ao_map.default_value")
	s.aoMap.enabledUniformId = s.findUniformByName("ao_map.enabled")

	s.uniformMaterialAmbient = s.findUniformByName("material_ambient")
	s.uniformMaterialDiffuse = s.findUniformByName("material_diffuse")
	s.uniformMaterialSpecular = s.findUniformByName("material_specular")
	s.uniformMaterialShininess = s.findUniformByName("material_shininess")

	s.directionalLight.enabledUniformId = s.findUniformByName("directional_light.enabled")
	s.directionalLight.directionUniformId = s.findUniformByName("directional_light.direction")
	s.directionalLight.irradianceUniformId = s.findUniformByName("directional_light.irradiance")

	for i := 0; i < LightCount; i++ {
		light := fmt.Sprintf("point_lights[%d]", i)
		s.pointLights[i].enabledUniformedId = s.findUniformByName(light + ".enabled")
		s.pointLights[i].positionUniformedId = s.findUniformByName(light + ".position")
		s.pointLights[i].irradianceUniformedId = s.findUniformByName(light + ".irradiance")
		s.pointLights[i].constantAttenuationUniformedId = s.findUniformByName(light + ".const_atten")
		s.pointLights[i].linearAttenuationUniformedId = s.findUniformByName(light + ".linear_atten")
		s.pointLights[i].quadraticAttenuationUniformedId = s.findUniformByName(light + ".quad_atten")
	}
}

func (s *Shader) bindAttribute(attribute uint32, name string) {
	gl.BindAttribLocation(s.programId, attribute, gl.Str(name+"\x00"))
}

func (s *Shader) findUniformByName(name string) int32 {
	return gl.GetUniformLocation(s.programId, gl.Str(name+"\x00"))
}

func (s *Shader) BindUniformProjection(projection mgl32.Mat4) {
	gl.UniformMatrix4fv(s.uniformProjection, 1, false, &projection[0])
}

func (s *Shader) BindUniformCamera(camera *Camera) {
	gl.UniformMatrix4fv(s.uniformView, 1, false, &camera.viewMatrix[0])
	gl.UniformMatrix3fv(s.uniformCameraPosition, 1, false, &camera.position[0])
}

func (s *Shader) BindUniformTransform(transform *mgl32.Mat4) {
	gl.UniformMatrix4fv(s.uniformTransform, 1, false, &transform[0])
}

func (s *Shader) BindUniformLights(allLights []lights.Light) {
	gl.Uniform1i(s.directionalLight.enabledUniformId, 0)

	for i, light := range allLights {
		switch l := light.(type) {
		case *lights.Directional:
			gl.Uniform1i(s.directionalLight.enabledUniformId, 1)
			gl.Uniform3f(s.directionalLight.directionUniformId, l.Direction.X(), l.Direction.Y(), l.Direction.Z())
			gl.Uniform3f(s.directionalLight.irradianceUniformId, l.Irradiance.X(), l.Irradiance.Y(), l.Irradiance.Z())
		case *lights.Point:
			if i < LightCount {
				gl.Uniform1i(s.pointLights[i].enabledUniformedId, 1)
				gl.Uniform3f(s.pointLights[i].positionUniformedId, l.Position.X(), l.Position.Y(), l.Position.Z())
				gl.Uniform3f(s.pointLights[i].irradianceUniformedId, l.Irradiance.X(), l.Irradiance.Y(), l.Irradiance.Z())
				gl.Uniform1f(s.pointLights[i].constantAttenuationUniformedId, l.ConstantAttenuation)
				gl.Uniform1f(s.pointLights[i].linearAttenuationUniformedId, l.LinearAttenuation)
				gl.Uniform1f(s.pointLights[i].quadraticAttenuationUniformedId, l.QuadraticAttenuation)
			}
		}
	}
}

func (s *Shader) BindUniformMaterial(material *Material) {
	gl.Uniform3f(s.uniformMaterialAmbient, material.Ambient.X(), material.Ambient.Y(), material.Ambient.Z())
	gl.Uniform3f(s.uniformMaterialDiffuse, material.Diffuse.X(), material.Diffuse.Y(), material.Diffuse.Z())
	gl.Uniform3f(s.uniformMaterialSpecular, material.Specular.X(), material.Specular.Y(), material.Specular.Z())
	gl.Uniform1f(s.uniformMaterialShininess, material.Shininess)
}

func (s *Shader) BindUniformTextureSamplers(albedo, normalMap, metallicRoughnessMap, aoMap *Texture) {
	// TODO: Rename to map?
	// Albedo map
	if albedo != nil {
		gl.Uniform1i(s.albedoMap.enabledUniformId, 1)
	} else {
		gl.Uniform1i(s.albedoMap.enabledUniformId, 0)
	}
	gl.Uniform1i(s.albedoMap.samplerUniformId, int32(TextureAlbedo))
	gl.Uniform3f(s.albedoMap.defaultValueUniformId, 1.0, 1.0, 1.0)

	// Normal map
	if normalMap != nil {
		gl.Uniform1i(s.normalMap.enabledUniformId, 1)
	} else {
		gl.Uniform1i(s.normalMap.enabledUniformId, 0)
	}
	gl.Uniform1i(s.normalMap.samplerUniformId, int32(TextureNormalMap))
	gl.Uniform3f(s.normalMap.defaultValueUniformId, 0.5, 0.5, 1.0)

	// Metallic/roughness map
	if metallicRoughnessMap != nil {
		gl.Uniform1i(s.metallicMap.enabledUniformId, 1)
		gl.Uniform1i(s.roughnessMap.enabledUniformId, 1)
	} else {
		gl.Uniform1i(s.metallicMap.enabledUniformId, 0)
		gl.Uniform1i(s.roughnessMap.enabledUniformId, 0)
	}
	gl.Uniform1i(s.metallicMap.samplerUniformId, int32(TextureMetallicRoughnessMap))
	gl.Uniform1i(s.roughnessMap.samplerUniformId, int32(TextureMetallicRoughnessMap))
	gl.Uniform3f(s.metallicMap.defaultValueUniformId, 0.0, 0.0, 0.0)
	gl.Uniform3f(s.roughnessMap.defaultValueUniformId, 1, 1, 1)

	// Ambient occlusion map
	if aoMap != nil {
		gl.Uniform1i(s.aoMap.enabledUniformId, 1)
	} else {
		gl.Uniform1i(s.aoMap.enabledUniformId, 0)
	}
	gl.Uniform1i(s.aoMap.samplerUniformId, int32(TextureAmbientOcclusionMap))
	gl.Uniform3f(s.aoMap.defaultValueUniformId, 1.0, 1.0, 1.0)
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

		return 0, fmt.Errorf("failed to compile shader:\n%v", log)
	}

	return shader, nil
}
