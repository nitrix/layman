package engine

import (
	"github.com/go-gl/mathgl/mgl32"
)
import "github.com/go-gl/gl/v4.1-core/gl"
import "github.com/udhos/gwob"

type Model struct {
	transform mgl32.Mat4

	verticesVao uint32

	vertexTextureNormalBufferId uint32
	//vertexCoordsBufferId  uint32
	//textureCoordsBufferId uint32
	//normalsBufferId       uint32
	facesBufferId         uint32

	obj *gwob.Obj
	indices []int32
}

func LoadModel(filepath string) (*Model, error) {
	model := &Model{}

	var err error
	options := &gwob.ObjParserOptions{}
	model.obj, err = gwob.NewObjFromFile(filepath, options)
	if err != nil {
		return model, err
	}

	model.transform = mgl32.Ident4() // TODO: Should be in Entity

	// Create the VAO on the GPU, then use it.
	gl.GenVertexArrays(1, &model.verticesVao)
	gl.BindVertexArray(model.verticesVao)

	// The convention by most of the 3D tools seems to be to inverse the Y axis.
	for row := 0; row < model.obj.NumberOfElements(); row++ {
		model.obj.Coord[row * 8 + 4] = 1 - model.obj.Coord[row * 8 + 4]
	}

	// All-in-one.
	gl.GenBuffers(1, &model.vertexTextureNormalBufferId)
	gl.BindBuffer(gl.ARRAY_BUFFER, model.vertexTextureNormalBufferId)
	gl.BufferData(gl.ARRAY_BUFFER, len(model.obj.Coord) * 4, gl.Ptr(model.obj.Coord), gl.STATIC_DRAW)

	// Vertex Buffer Object (VBO).
	//gl.GenBuffers(1, &model.vertexCoordsBufferId)
	//gl.BindBuffer(gl.ARRAY_BUFFER, model.vertexCoordsBufferId)
	//gl.BufferData(gl.ARRAY_BUFFER, len(model.obj.Coord) * 4, gl.Ptr(model.obj.Coord), gl.STATIC_DRAW)
	gl.EnableVertexAttribArray(ShaderAttributeVertexCoords)
	gl.VertexAttribPointer(ShaderAttributeVertexCoords, 3, gl.FLOAT, false, int32(model.obj.StrideSize), gl.PtrOffset(model.obj.StrideOffsetPosition))

	// Texture UVs.
	//gl.GenBuffers(1, &model.textureCoordsBufferId)
	//gl.BindBuffer(gl.ARRAY_BUFFER, model.textureCoordsBufferId)
	//gl.BufferData(gl.ARRAY_BUFFER, len(model.obj.Coord) * 4, gl.Ptr(model.obj.Coord), gl.STATIC_DRAW)
	gl.EnableVertexAttribArray(ShaderAttributeTextureCoords)
	gl.VertexAttribPointer(ShaderAttributeTextureCoords, 2, gl.FLOAT, false, int32(model.obj.StrideSize), gl.PtrOffset(model.obj.StrideOffsetTexture))

	// Normals.
	//gl.GenBuffers(1, &model.normalsBufferId)
	//gl.BindBuffer(gl.ARRAY_BUFFER, model.normalsBufferId)
	//gl.BufferData(gl.ARRAY_BUFFER, len(model.obj.Coord) * 4, gl.Ptr(model.obj.Coord), gl.STATIC_DRAW)
	gl.EnableVertexAttribArray(ShaderAttributeNormals)
	gl.VertexAttribPointer(ShaderAttributeNormals, 3, gl.FLOAT, false, int32(model.obj.StrideSize), gl.PtrOffset(model.obj.StrideOffsetNormal))

	// FIXME: Write my own .obj parser.
	// The obj reader produces indices that are `int` instead of the `int32` that OpenGL expects.
	// Don't get fooled by the usage being `gl.UNSIGNED_INT` thinking it'll be whatever `int` is native on the machine,
	// OpenGL seems to always understand this as 32-bit; which break the FFI with Go.
	for _, v := range model.obj.Indices {
		model.indices = append(model.indices, int32(v))
	}

	// Faces (EBO).
	gl.GenBuffers(1, &model.facesBufferId)
	gl.BindBuffer(gl.ELEMENT_ARRAY_BUFFER, model.facesBufferId)
	gl.BufferData(gl.ELEMENT_ARRAY_BUFFER, len(model.obj.Indices) * 4, gl.Ptr(model.indices), gl.STATIC_DRAW)

	return model, nil
}

func (m *Model) Use() {
	gl.BindVertexArray(m.verticesVao)

	// Vertices
	//gl.BindBuffer(gl.ARRAY_BUFFER, m.vertexTextureNormalBufferId)

	// Texture uvs
	//gl.BindBuffer(gl.ARRAY_BUFFER, m.vertexTextureNormalBufferId)

	// Normals
	//gl.BindBuffer(gl.ARRAY_BUFFER, m.vertexTextureNormalBufferId)

	// Faces
	//gl.BindBuffer(gl.ELEMENT_ARRAY_BUFFER, m.facesBufferId)
}

func (m *Model) Unuse() {
	gl.BindVertexArray(0)
}

// Using radians, so a complete rotation is 2*Pi.
func (m *Model) RotateX(angle float32) {
	m.transform = m.transform.Mul4(mgl32.HomogRotate3DX(angle))
}

func (m *Model) RotateY(angle float32) {
	m.transform = m.transform.Mul4(mgl32.HomogRotate3DY(angle))
}

func (m *Model) RotateZ(angle float32) {
	m.transform = m.transform.Mul4(mgl32.HomogRotate3DZ(angle))
}