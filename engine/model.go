package engine

import (
	"fmt"
	"github.com/go-gl/mathgl/mgl32"
)

type Model struct {
	name                string
	mesh                *Mesh
	albedoTexture       *Texture
	normalMapTexture    *Texture
	roughnessMapTexture *Texture
	glowMapTexture      *Texture
	shader              *Shader
	material            *Material
	initialTransform    mgl32.Mat4
}

type ModelParams struct {
	Name                    string
	MeshPath                string
	AlbedoTexturePath       string
	NormalMapTexturePath    string
	RoughnessMapTexturePath string
	VertexShaderPath        string
	FragmentShaderPath      string
	InitialScale            float32
	InitialRotation         mgl32.Vec3
	Material                Material
}

func (m Model) Use() {
	m.albedoTexture.Use()
	m.normalMapTexture.Use()
	m.roughnessMapTexture.Use()
	m.mesh.Use()
	m.shader.Use()
}

func (m Model) Unuse() {
	m.albedoTexture.Unuse()
	m.normalMapTexture.Unuse()
	m.roughnessMapTexture.Unuse()
	m.mesh.Unuse()
	m.shader.Unuse()
}

func LoadModel(params ModelParams) (*Model, error) {
	var err error

	model := &Model{
		initialTransform: mgl32.Ident4(),
	}

	model.mesh, err = LoadMesh(params.MeshPath)
	if err != nil {
		return nil, err
	}

	// Albedo texture
	if params.AlbedoTexturePath != "" {
		model.albedoTexture, err = LoadTexture(TextureAlbedo, params.AlbedoTexturePath)
		if err != nil {
			return nil, err
		}
	}

	// Normal texture map
	if params.NormalMapTexturePath != "" {
		model.normalMapTexture, err = LoadTexture(TextureNormalMap, params.NormalMapTexturePath)
		if err != nil {
			return nil, err
		}
	}

	// Roughness texture map
	if params.RoughnessMapTexturePath != "" {
		model.roughnessMapTexture, err = LoadTexture(TextureRoughnessMap, params.RoughnessMapTexturePath)
		if err != nil {
			return nil, err
		}
	}

	// TODO: Load glow map texture
	model.glowMapTexture = &Texture{}

	// Load shader program.
	model.shader, err = LoadShader(params.VertexShaderPath, params.FragmentShaderPath)
	if err != nil {
		return nil, fmt.Errorf("unable to load shader: %w", err)
	}

	// Apply scaling
	if params.InitialScale != 0 {
		model.initialTransform = model.initialTransform.Mul4(
			mgl32.Scale3D(params.InitialScale, params.InitialScale, params.InitialScale),
		)
	}

	// Apply rotation
	if params.InitialRotation.X() != 0 || params.InitialRotation.Y() != 0 || params.InitialRotation.Z() != 0 {
		model.initialTransform = model.initialTransform.Mul4(
			mgl32.HomogRotate3DX(params.InitialRotation.X()),
		)
		model.initialTransform = model.initialTransform.Mul4(
			mgl32.HomogRotate3DY(params.InitialRotation.Y()),
		)
		model.initialTransform = model.initialTransform.Mul4(
			mgl32.HomogRotate3DZ(params.InitialRotation.Z()),
		)
	}

	// Apply material
	model.material = &params.Material

	return model, nil
}