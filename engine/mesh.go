package engine

import (
	"github.com/go-gl/mathgl/mgl32"
)
import "github.com/go-gl/gl/v4.1-core/gl"
import "github.com/udhos/gwob"

type Mesh struct {
	verticesVao uint32

	vertexTextureNormalBufferId uint32
	//vertexCoordsBufferId  uint32
	//textureCoordsBufferId uint32
	//normalsBufferId       uint32
	facesBufferId           uint32
	tangentBufferId         uint32

	faceCount int32
}

func LoadMesh(filepath string) (*Mesh, error) {
	mesh := &Mesh{}

	options := &gwob.ObjParserOptions{}
	obj, err := gwob.NewObjFromFile(filepath, options)
	if err != nil {
		return mesh, err
	}

	// Create the VAO on the GPU, then use it.
	gl.GenVertexArrays(1, &mesh.verticesVao)
	gl.BindVertexArray(mesh.verticesVao)

	// The convention by most of the 3D tools seems to be to inverse the Y axis.
	for row := 0; row < obj.NumberOfElements(); row++ {
		obj.Coord[row * 8 + 4] = 1 - obj.Coord[row * 8 + 4]
	}

	// All-in-one.
	gl.GenBuffers(1, &mesh.vertexTextureNormalBufferId)
	gl.BindBuffer(gl.ARRAY_BUFFER, mesh.vertexTextureNormalBufferId)
	gl.BufferData(gl.ARRAY_BUFFER, len(obj.Coord) * 4, gl.Ptr(obj.Coord), gl.STATIC_DRAW)

	// Vertex Buffer Object (VBO).
	//gl.GenBuffers(1, &mesh.vertexCoordsBufferId)
	//gl.BindBuffer(gl.ARRAY_BUFFER, mesh.vertexCoordsBufferId)
	//gl.BufferData(gl.ARRAY_BUFFER, len(mesh.obj.Coord) * 4, gl.Ptr(mesh.obj.Coord), gl.STATIC_DRAW)
	gl.EnableVertexAttribArray(ShaderAttributeVertexCoords)
	gl.VertexAttribPointer(ShaderAttributeVertexCoords, 3, gl.FLOAT, false, int32(obj.StrideSize), gl.PtrOffset(obj.StrideOffsetPosition))

	// Texture UVs.
	//gl.GenBuffers(1, &mesh.textureCoordsBufferId)
	//gl.BindBuffer(gl.ARRAY_BUFFER, mesh.textureCoordsBufferId)
	//gl.BufferData(gl.ARRAY_BUFFER, len(mesh.obj.Coord) * 4, gl.Ptr(mesh.obj.Coord), gl.STATIC_DRAW)
	gl.EnableVertexAttribArray(ShaderAttributeTextureCoords)
	gl.VertexAttribPointer(ShaderAttributeTextureCoords, 2, gl.FLOAT, false, int32(obj.StrideSize), gl.PtrOffset(obj.StrideOffsetTexture))

	// Normals.
	//gl.GenBuffers(1, &mesh.normalsBufferId)
	//gl.BindBuffer(gl.ARRAY_BUFFER, mesh.normalsBufferId)
	//gl.BufferData(gl.ARRAY_BUFFER, len(mesh.obj.Coord) * 4, gl.Ptr(mesh.obj.Coord), gl.STATIC_DRAW)
	gl.EnableVertexAttribArray(ShaderAttributeNormals)
	gl.VertexAttribPointer(ShaderAttributeNormals, 3, gl.FLOAT, false, int32(obj.StrideSize), gl.PtrOffset(obj.StrideOffsetNormal))

	// FIXME: Write my own .obj parser.
	// The obj reader produces indices that are `int` instead of the `int32` that OpenGL expects.
	// Don't get fooled by the usage being `gl.UNSIGNED_INT` thinking it'll be whatever `int` is native on the machine,
	// OpenGL seems to always understand this as 32-bit; which break the FFI with Go.
	var indices []int32
	for _, v := range obj.Indices {
		indices = append(indices, int32(v))
	}

	// Faces (EBO).
	gl.GenBuffers(1, &mesh.facesBufferId)
	gl.BindBuffer(gl.ELEMENT_ARRAY_BUFFER, mesh.facesBufferId)
	gl.BufferData(gl.ELEMENT_ARRAY_BUFFER, len(indices) * 4, gl.Ptr(indices), gl.STATIC_DRAW)

	// Tangent/Bitangent.
	tangents := make([]float32, obj.NumberOfElements() * 3)
	for i := 0; i < len(indices); i += 3 {
		v1i := indices[i]
		v1x := obj.Coord[v1i*8 + 0]
		v1y := obj.Coord[v1i*8 + 1]
		v1z := obj.Coord[v1i*8 + 2]
		v1u := obj.Coord[v1i*8 + 3]
		v1v := obj.Coord[v1i*8 + 4]
		v1 := mgl32.Vec3{v1x, v1y, v1z}
		vt1 := mgl32.Vec2{v1u, v1v}

		v2i := indices[i+1]
		v2x := obj.Coord[v2i*8 + 0]
		v2y := obj.Coord[v2i*8 + 1]
		v2z := obj.Coord[v2i*8 + 2]
		v2u := obj.Coord[v2i*8 + 3]
		v2v := obj.Coord[v2i*8 + 4]
		v2 := mgl32.Vec3{v2x, v2y, v2z}
		vt2 := mgl32.Vec2{v2u, v2v}

		v3i := indices[i+2]
		v3x := obj.Coord[v3i*8 + 0]
		v3y := obj.Coord[v3i*8 + 1]
		v3z := obj.Coord[v3i*8 + 2]
		v3u := obj.Coord[v3i*8 + 3]
		v3v := obj.Coord[v3i*8 + 4]
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
	gl.GenBuffers(1, &mesh.tangentBufferId)
	gl.BindBuffer(gl.ARRAY_BUFFER, mesh.tangentBufferId)
	gl.BufferData(gl.ARRAY_BUFFER, len(tangents) * 4, gl.Ptr(tangents), gl.STATIC_DRAW)
	gl.EnableVertexAttribArray(ShaderAttributeTangents)
	gl.VertexAttribPointer(ShaderAttributeTangents, 3, gl.FLOAT, false, int32(3 * 4), gl.PtrOffset(0))

	mesh.faceCount = int32(len(indices))

	return mesh, nil
}

func (m *Mesh) Use() {
	gl.BindVertexArray(m.verticesVao)

	// Vertices
	//gl.BindBuffer(gl.ARRAY_BUFFER, m.vertexTextureNormalBufferId)

	// Texture uvs
	//gl.BindBuffer(gl.ARRAY_BUFFER, m.vertexTextureNormalBufferId)

	// Normals
	//gl.BindBuffer(gl.ARRAY_BUFFER, m.vertexTextureNormalBufferId)

	// Faces
	//gl.BindBuffer(gl.ELEMENT_ARRAY_BUFFER, m.facesBufferId)

	// Tangents
	// gl.BindBuffer(gl.ARRAY_BUFFER, m.tangentBufferId)
}

func (m *Mesh) Unuse() {
	gl.BindVertexArray(0)
}
