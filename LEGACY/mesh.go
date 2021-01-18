package laygl

import (
	"github.com/go-gl/gl/v4.6-core/gl"
)

type mesh struct {
	name string

	vao uint32

	verticesBufferId uint32
	uvsBufferId      uint32
	normalsBufferId  uint32
	indicesBufferId  uint32
	tangentBufferId  uint32

	indiceCount uint32

	albedoTexture               *Texture
	normalMapTexture            *Texture
	emissiveMapTexture          *Texture
	metallicRoughnessMapTexture *Texture
	ambientOcclusionMapTexture  *Texture

	vertices []float32
	normals  []float32
	uvs      []float32
	tangents []float32
	indices  []uint16

	shader   *Shader
}

func (m *mesh) Use() {
	gl.BindVertexArray(m.vao)

	// TODO: Shouldn't be necessary to bind buffers here, it's stored in the VAO.

	// Vertices
	gl.BindBuffer(gl.ARRAY_BUFFER, m.verticesBufferId)

	// Texture uvs
	gl.BindBuffer(gl.ARRAY_BUFFER, m.uvsBufferId)

	// Normals
	gl.BindBuffer(gl.ARRAY_BUFFER, m.normalsBufferId)

	// Faces
	gl.BindBuffer(gl.ELEMENT_ARRAY_BUFFER, m.indicesBufferId)

	// Tangents
	gl.BindBuffer(gl.ARRAY_BUFFER, m.tangentBufferId)

	if m.albedoTexture != nil {
		m.albedoTexture.Use()
	}

	if m.normalMapTexture != nil {
		m.normalMapTexture.Use()
	}

	if m.emissiveMapTexture != nil {
		m.emissiveMapTexture.Use()
	}

	if m.metallicRoughnessMapTexture != nil {
		m.metallicRoughnessMapTexture.Use()
	}

	if m.ambientOcclusionMapTexture != nil {
		m.ambientOcclusionMapTexture.Use()
	}

	m.shader.Use()
}

func (m *mesh) Unuse() {
	gl.BindVertexArray(0)

	if m.albedoTexture != nil {
		m.albedoTexture.Unuse()
	}

	if m.normalMapTexture != nil {
		m.normalMapTexture.Unuse()
	}

	if m.emissiveMapTexture != nil {
		m.emissiveMapTexture.Unuse()
	}

	if m.metallicRoughnessMapTexture != nil {
		m.metallicRoughnessMapTexture.Unuse()
	}

	if m.ambientOcclusionMapTexture != nil {
		m.ambientOcclusionMapTexture.Unuse()
	}

	m.shader.Unuse()
}
