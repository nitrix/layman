package engine

import (
	"github.com/go-gl/gl/v4.1-core/gl"
	"github.com/go-gl/mathgl/mgl32"
)

const TerrainSize = 200
const terrainVertexCount = 128

type Terrain struct {
	mesh *Mesh
	albedoTexture *Texture
	normalMapTexture *Texture
	roughnessMapTexture *Texture
	glowMapTexture *Texture
	shader *Shader
	material Material
	transform mgl32.Mat4
}

type TerrainParams struct {
	AlbedoTexturePath string
	NormalMapTexturePath string
	RoughnessMapTexturePath string
	GlowMapTexturePath string
}

func NewTerrain(params TerrainParams) (*Terrain, error) {
	terrain := &Terrain{}

	shader, err := LoadShader("shaders/terrain/vertex.glsl", "shaders/terrain/fragment.glsl")
	if err != nil {
		return nil, err
	}

	if params.AlbedoTexturePath != "" {
		terrain.albedoTexture, err = LoadTexture(TextureAlbedo, params.AlbedoTexturePath)
		if err != nil {
			return nil, err
		}
	}

	terrain.albedoTexture.Use()
	gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.REPEAT)
	gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.REPEAT)
	terrain.albedoTexture.Unuse()

	if params.NormalMapTexturePath != "" {
		terrain.normalMapTexture, err = LoadTexture(TextureNormalMap, params.NormalMapTexturePath)
		if err != nil {
			return nil, err
		}
	}

	if params.RoughnessMapTexturePath != "" {
		terrain.roughnessMapTexture, err = LoadTexture(TextureRoughnessMap, params.RoughnessMapTexturePath)
		if err != nil {
			return nil, err
		}
	}

	if params.GlowMapTexturePath != "" {
		terrain.glowMapTexture, err = LoadTexture(TextureGlowMap, params.GlowMapTexturePath)
		if err != nil {
			return nil, err
		}
	}

	terrain.mesh = generateTerrainMesh()
	terrain.material = DefaultMaterial()
	terrain.shader = shader
	terrain.transform = mgl32.Ident4()

	return terrain, nil
}


func (t Terrain) Use() {
	t.mesh.Use()
	t.shader.Use()

	if t.albedoTexture != nil {
		t.albedoTexture.Use()
	}

	if t.normalMapTexture != nil {
		t.normalMapTexture.Use()
	}

	if t.roughnessMapTexture != nil {
		t.roughnessMapTexture.Use()
	}

	if t.glowMapTexture != nil {
		t.glowMapTexture.Use()
	}
}

func (t Terrain) Unuse() {
	t.mesh.Unuse()
	t.shader.Unuse()

	if t.albedoTexture != nil {
		t.albedoTexture.Unuse()
	}

	if t.normalMapTexture != nil {
		t.normalMapTexture.Unuse()
	}

	if t.roughnessMapTexture != nil {
		t.roughnessMapTexture.Unuse()
	}

	if t.glowMapTexture != nil {
		t.glowMapTexture.Unuse()
	}
}

func (t *Terrain) Translate(position mgl32.Vec3) {
	t.transform = t.transform.Mul4(mgl32.Translate3D(position[0], position[1], position[2]))
}

func generateTerrainMesh() *Mesh {
	numberOfElements := terrainVertexCount * terrainVertexCount
	allInOne := make([]float32, 0, numberOfElements * 8)
	indices := make([]int32, 0, 6*(terrainVertexCount-1)*(terrainVertexCount-1))

	for i := 0; i < terrainVertexCount; i++ {
		for j := 0; j < terrainVertexCount; j++ {
			vx := float32(j) / (terrainVertexCount-1) * TerrainSize
			vy := float32(0)
			vz := float32(i) / (terrainVertexCount-1) * TerrainSize
			tu := float32(j) / float32(terrainVertexCount-1)
			tv := float32(i) / float32(terrainVertexCount-1)
			nx := float32(0)
			ny := float32(1)
			nz := float32(0)

			// Tiling
			tu *= 40
			tv *= 40

			allInOne = append(allInOne, vx)
			allInOne = append(allInOne, vy)
			allInOne = append(allInOne, vz)
			allInOne = append(allInOne, tu)
			allInOne = append(allInOne, tv)
			allInOne = append(allInOne, nx)
			allInOne = append(allInOne, ny)
			allInOne = append(allInOne, nz)
		}
	}

	for z := int32(0); z < terrainVertexCount-1; z++ {
		for x := int32(0); x < terrainVertexCount-1; x++ {
			topLeft := z * terrainVertexCount + x
			topRight := topLeft + 1
			bottomLeft := (z+1) * terrainVertexCount + x
			bottomRight := bottomLeft + 1

			indices = append(indices, topLeft)
			indices = append(indices, bottomLeft)
			indices = append(indices, topRight)
			indices = append(indices, topRight)
			indices = append(indices, bottomLeft)
			indices = append(indices, bottomRight)
		}
	}

	mesh := rawToMesh(allInOne, numberOfElements, indices)
	return mesh
}