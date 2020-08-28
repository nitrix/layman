package laygl

import (
	"github.com/go-gl/gl/v4.6-core/gl"
	"github.com/go-gl/mathgl/mgl32"
)

type Mesh struct {
	vao uint32

	verticesBufferId uint32
	uvsBufferId      uint32
	normalsBufferId  uint32
	indicesBufferId  uint32
	tangentBufferId  uint32

	triangleCount int32
}

func rawToMesh(triangleCount int, vertices, uvs, normals []float32, indices []int32) *Mesh {
	mesh := &Mesh{}

	// Create the VAO on the GPU, then use it.
	gl.GenVertexArrays(1, &mesh.vao)
	gl.BindVertexArray(mesh.vao)

	// Vertex Buffer Object (VBO).
	gl.GenBuffers(1, &mesh.verticesBufferId)
	gl.BindBuffer(gl.ARRAY_BUFFER, mesh.verticesBufferId)
	gl.BufferData(gl.ARRAY_BUFFER, len(vertices) * 4, gl.Ptr(vertices), gl.STATIC_DRAW)
	gl.EnableVertexAttribArray(ShaderAttributeVertexCoords)
	gl.VertexAttribPointer(ShaderAttributeVertexCoords, 3, gl.FLOAT, false, 3 * 4, gl.PtrOffset(0))

	// Texture UVs.
	gl.GenBuffers(1, &mesh.uvsBufferId)
	gl.BindBuffer(gl.ARRAY_BUFFER, mesh.uvsBufferId)
	gl.BufferData(gl.ARRAY_BUFFER, len(uvs) * 4, gl.Ptr(uvs), gl.STATIC_DRAW)
	gl.EnableVertexAttribArray(ShaderAttributeTextureCoords)
	gl.VertexAttribPointer(ShaderAttributeTextureCoords, 2, gl.FLOAT, false, 2 * 4, gl.PtrOffset(0))

	// Normals.
	gl.GenBuffers(1, &mesh.normalsBufferId)
	gl.BindBuffer(gl.ARRAY_BUFFER, mesh.normalsBufferId)
	gl.BufferData(gl.ARRAY_BUFFER, len(normals) * 4, gl.Ptr(normals), gl.STATIC_DRAW)
	gl.EnableVertexAttribArray(ShaderAttributeNormals)
	gl.VertexAttribPointer(ShaderAttributeNormals, 3, gl.FLOAT, false, 3 * 4, gl.PtrOffset(0))

	// Faces (EBO).
	gl.GenBuffers(1, &mesh.indicesBufferId)
	gl.BindBuffer(gl.ELEMENT_ARRAY_BUFFER, mesh.indicesBufferId)
	gl.BufferData(gl.ELEMENT_ARRAY_BUFFER, len(indices) * 4, gl.Ptr(indices), gl.STATIC_DRAW)
	mesh.triangleCount = int32(len(indices))

	// Tangent/Bitangent.
	tangents := generateTangents(triangleCount, indices, vertices, uvs, normals)
	gl.GenBuffers(1, &mesh.tangentBufferId)
	gl.BindBuffer(gl.ARRAY_BUFFER, mesh.tangentBufferId)
	gl.BufferData(gl.ARRAY_BUFFER, len(tangents) * 4, gl.Ptr(tangents), gl.STATIC_DRAW)
	gl.EnableVertexAttribArray(ShaderAttributeTangents)
	gl.VertexAttribPointer(ShaderAttributeTangents, 3, gl.FLOAT, false, int32(3 * 4), gl.PtrOffset(0))

	return mesh
}

func generateTangents(numberOfElements int, indices []int32, vertices, uvs, normals []float32) []float32 {
	tangents := make([]float32, numberOfElements * 3)
	bitangents := make([]float32, numberOfElements * 3)

	for i := 0; i < len(indices); i += 3 {
		v1i := indices[i]
		v1x := vertices[v1i*3 + 0]
		v1y := vertices[v1i*3 + 1]
		v1z := vertices[v1i*3 + 2]
		v1u := uvs[v1i*2 + 0]
		v1v := uvs[v1i*2 + 1]
		v1 := mgl32.Vec3{v1x, v1y, v1z}
		vt1 := mgl32.Vec2{v1u, v1v}

		v2i := indices[i+1]
		v2x := vertices[v2i*3 + 0]
		v2y := vertices[v2i*3 + 1]
		v2z := vertices[v2i*3 + 2]
		v2u := uvs[v2i*2 + 0]
		v2v := uvs[v2i*2 + 1]
		v2 := mgl32.Vec3{v2x, v2y, v2z}
		vt2 := mgl32.Vec2{v2u, v2v}

		v3i := indices[i+2]
		v3x := vertices[v3i*3 + 0]
		v3y := vertices[v3i*3 + 1]
		v3z := vertices[v3i*3 + 2]
		v3u := uvs[v3i*2 + 0]
		v3v := uvs[v3i*2 + 1]
		v3 := mgl32.Vec3{v3x, v3y, v3z}
		vt3 := mgl32.Vec2{v3u, v3v}

		// Part 1
		deltaPos1 := v2.Sub(v1)
		deltaPos2 := v3.Sub(v1)
		deltaUV1 := vt2.Sub(vt1)
		deltaUV2 := vt3.Sub(vt1)

		f := 1.0 / (deltaUV1.X() * deltaUV2.Y() - deltaUV1.Y() * deltaUV2.X())

		tangent := mgl32.Vec3{
			(deltaPos1.X() * deltaUV2.Y() - deltaPos2.X() * deltaUV1.Y()) * f,
			(deltaPos1.Y() * deltaUV2.Y() - deltaPos2.Y() * deltaUV1.Y()) * f,
			(deltaPos1.Z() * deltaUV2.Y() - deltaPos2.Z() * deltaUV1.Y()) * f,
		}

		bitangent := mgl32.Vec3{
			(deltaPos1.X() * deltaUV2.X() - deltaPos2.X() * deltaUV1.X()) * f,
			(deltaPos1.Y() * deltaUV2.X() - deltaPos2.Y() * deltaUV1.X()) * f,
			(deltaPos1.Z() * deltaUV2.X() - deltaPos2.Z() * deltaUV1.X()) * f,
		}

		tangents[v1i*3 + 0] += tangent.X()
		tangents[v1i*3 + 1] += tangent.Y()
		tangents[v1i*3 + 2] += tangent.Z()
		tangents[v2i*3 + 0] += tangent.X()
		tangents[v2i*3 + 1] += tangent.Y()
		tangents[v2i*3 + 2] += tangent.Z()
		tangents[v3i*3 + 0] += tangent.X()
		tangents[v3i*3 + 1] += tangent.Y()
		tangents[v3i*3 + 2] += tangent.Z()

		bitangents[v1i*3 + 0] += bitangent.X()
		bitangents[v1i*3 + 1] += bitangent.Y()
		bitangents[v1i*3 + 2] += bitangent.Z()
		bitangents[v2i*3 + 0] += bitangent.X()
		bitangents[v2i*3 + 1] += bitangent.Y()
		bitangents[v2i*3 + 2] += bitangent.Z()
		bitangents[v3i*3 + 0] += bitangent.X()
		bitangents[v3i*3 + 1] += bitangent.Y()
		bitangents[v3i*3 + 2] += bitangent.Z()
	}

	// Part 2
	for i := 0; i < numberOfElements; i++ {
		n := mgl32.Vec3{
			normals[i*3+0],
			normals[i*3+1],
			normals[i*3+2],
		}

		t0 := mgl32.Vec3{
			tangents[i*3+0],
			tangents[i*3+1],
			tangents[i*3+2],
		}

		t := t0.Sub(n.Mul(n.Dot(t0)))
		t = t.Normalize()

		tangents[i*3+0] = t.X()
		tangents[i*3+1] = t.Y()
		tangents[i*3+2] = t.Z()
	}

	return tangents
}

func (m *Mesh) Use() {
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
	//gl.BindBuffer(gl.ARRAY_BUFFER, m.tangentBufferId)
}

func (m *Mesh) Unuse() {
	gl.BindVertexArray(0)
}
