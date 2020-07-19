package engine

import (
	"github.com/go-gl/mathgl/mgl32"
)
import "github.com/go-gl/gl/v4.1-core/gl"
import "github.com/udhos/gwob"

type Mesh struct {
	vao uint32

	vertexTextureNormalBufferId uint32
	//vertexCoordsBufferId  uint32
	//textureCoordsBufferId uint32
	//normalsBufferId       uint32
	facesBufferId           uint32
	tangentBufferId         uint32

	faceCount int32
}

const strideSize = 8 * 4 // (px,py,pz),(tu,tv),(nx,ny,nz) = 8 x 4-byte floats = 32 bytes max
const strideOffsetPosition = 0
const strideOffsetTexture = 3 * 4 // 3 x 4-byte floats
const strideOffsetNormal = 5 * 4 // 5 x 4-byte floats

func LoadMesh(filepath string) (*Mesh, error) {
	options := &gwob.ObjParserOptions{}
	obj, err := gwob.NewObjFromFile(filepath, options)
	if err != nil {
		return &Mesh{}, err
	}

	// The gwob obj reader produces indices that are `int` instead of the `int32` that OpenGL expects.
	var convertedIndices []int32
	for _, v := range obj.Indices {
		convertedIndices = append(convertedIndices, int32(v))
	}

	mesh := rawToMesh(obj.Coord, obj.NumberOfElements(), convertedIndices)
	return mesh, nil
}

func rawToMesh(allInOne []float32, numberOfElements int, indices []int32) *Mesh {
	mesh := &Mesh{}

	// Create the VAO on the GPU, then use it.
	gl.GenVertexArrays(1, &mesh.vao)
	gl.BindVertexArray(mesh.vao)

	// The convention by most of the 3D tools seems to be to inverse the Y axis.
	for row := 0; row < numberOfElements; row++ {
		allInOne[row * 8 + 4] = 1 - allInOne[row * 8 + 4]
	}

	// All-in-one.
	gl.GenBuffers(1, &mesh.vertexTextureNormalBufferId)
	gl.BindBuffer(gl.ARRAY_BUFFER, mesh.vertexTextureNormalBufferId)
	gl.BufferData(gl.ARRAY_BUFFER, len(allInOne) * 4, gl.Ptr(allInOne), gl.STATIC_DRAW)

	// Vertex Buffer Object (VBO).
	//gl.GenBuffers(1, &mesh.vertexCoordsBufferId)
	//gl.BindBuffer(gl.ARRAY_BUFFER, mesh.vertexCoordsBufferId)
	//gl.BufferData(gl.ARRAY_BUFFER, len(allInOne) * 4, gl.Ptr(allInOne), gl.STATIC_DRAW)
	gl.EnableVertexAttribArray(ShaderAttributeVertexCoords)
	gl.VertexAttribPointer(ShaderAttributeVertexCoords, 3, gl.FLOAT, false, strideSize, gl.PtrOffset(strideOffsetPosition))

	// Texture UVs.
	//gl.GenBuffers(1, &mesh.textureCoordsBufferId)
	//gl.BindBuffer(gl.ARRAY_BUFFER, mesh.textureCoordsBufferId)
	//gl.BufferData(gl.ARRAY_BUFFER, len(allInOne) * 4, gl.Ptr(allInOne), gl.STATIC_DRAW)
	gl.EnableVertexAttribArray(ShaderAttributeTextureCoords)
	gl.VertexAttribPointer(ShaderAttributeTextureCoords, 2, gl.FLOAT, false, strideSize, gl.PtrOffset(strideOffsetTexture))

	// Normals.
	//gl.GenBuffers(1, &mesh.normalsBufferId)
	//gl.BindBuffer(gl.ARRAY_BUFFER, mesh.normalsBufferId)
	//gl.BufferData(gl.ARRAY_BUFFER, len(allInOne) * 4, gl.Ptr(allInOne), gl.STATIC_DRAW)
	gl.EnableVertexAttribArray(ShaderAttributeNormals)
	gl.VertexAttribPointer(ShaderAttributeNormals, 3, gl.FLOAT, false, strideSize, gl.PtrOffset(strideOffsetNormal))

	// Faces (EBO).
	gl.GenBuffers(1, &mesh.facesBufferId)
	gl.BindBuffer(gl.ELEMENT_ARRAY_BUFFER, mesh.facesBufferId)
	gl.BufferData(gl.ELEMENT_ARRAY_BUFFER, len(indices) * 4, gl.Ptr(indices), gl.STATIC_DRAW)
	mesh.faceCount = int32(len(indices))

	// Tangent/Bitangent.
	tangents := generateTangents(numberOfElements, indices, allInOne)
	gl.GenBuffers(1, &mesh.tangentBufferId)
	gl.BindBuffer(gl.ARRAY_BUFFER, mesh.tangentBufferId)
	gl.BufferData(gl.ARRAY_BUFFER, len(tangents) * 4, gl.Ptr(tangents), gl.STATIC_DRAW)
	gl.EnableVertexAttribArray(ShaderAttributeTangents)
	gl.VertexAttribPointer(ShaderAttributeTangents, 3, gl.FLOAT, false, int32(3 * 4), gl.PtrOffset(0))

	return mesh
}

func generateTangents(numberOfElements int, indices []int32, allInOne []float32) []float32 {
	tangents := make([]float32, numberOfElements * 3)

	for i := 0; i < len(indices); i += 3 {
		v1i := indices[i]
		v1x := allInOne[v1i*8 + 0]
		v1y := allInOne[v1i*8 + 1]
		v1z := allInOne[v1i*8 + 2]
		v1u := allInOne[v1i*8 + 3]
		v1v := allInOne[v1i*8 + 4]
		v1 := mgl32.Vec3{v1x, v1y, v1z}
		vt1 := mgl32.Vec2{v1u, v1v}

		v2i := indices[i+1]
		v2x := allInOne[v2i*8 + 0]
		v2y := allInOne[v2i*8 + 1]
		v2z := allInOne[v2i*8 + 2]
		v2u := allInOne[v2i*8 + 3]
		v2v := allInOne[v2i*8 + 4]
		v2 := mgl32.Vec3{v2x, v2y, v2z}
		vt2 := mgl32.Vec2{v2u, v2v}

		v3i := indices[i+2]
		v3x := allInOne[v3i*8 + 0]
		v3y := allInOne[v3i*8 + 1]
		v3z := allInOne[v3i*8 + 2]
		v3u := allInOne[v3i*8 + 3]
		v3v := allInOne[v3i*8 + 4]
		v3 := mgl32.Vec3{v3x, v3y, v3z}
		vt3 := mgl32.Vec2{v3u, v3v}

		deltaPos1 := v2.Sub(v1)
		deltaPos2 := v3.Sub(v1)
		deltaUV1 := vt2.Sub(vt1)
		deltaUV2 := vt3.Sub(vt1)

		f := 1.0 / (deltaUV1.X() * deltaUV2.Y() - deltaUV2.X() * deltaUV1.Y())
		tangent := (deltaPos1.Mul(deltaUV2.Y()).Sub(deltaPos2.Mul(deltaUV1.Y()))).Mul(f)

		tangents[v1i*3 + 0] = tangent.X()
		tangents[v1i*3 + 1] = tangent.Y()
		tangents[v1i*3 + 2] = tangent.Z()
		tangents[v2i*3 + 0] = tangent.X()
		tangents[v2i*3 + 1] = tangent.Y()
		tangents[v2i*3 + 2] = tangent.Z()
		tangents[v3i*3 + 0] = tangent.X()
		tangents[v3i*3 + 1] = tangent.Y()
		tangents[v3i*3 + 2] = tangent.Z()
	}

	return tangents
}

func (m *Mesh) Use() {
	gl.BindVertexArray(m.vao)

	// Vertices
	//gl.BindBuffer(gl.ARRAY_BUFFER, m.vertexTextureNormalBufferId)

	// Texture uvs
	//gl.BindBuffer(gl.ARRAY_BUFFER, m.vertexTextureNormalBufferId)

	// Normals
	//gl.BindBuffer(gl.ARRAY_BUFFER, m.vertexTextureNormalBufferId)

	// Faces
	//gl.BindBuffer(gl.ELEMENT_ARRAY_BUFFER, m.facesBufferId)

	// Tangents
	//gl.BindBuffer(gl.ARRAY_BUFFER, m.tangentBufferId)
}

func (m *Mesh) Unuse() {
	gl.BindVertexArray(0)
}
