package laygl

import (
	"github.com/udhos/gwob"
)

func LoadObj(modelPath string) (*Model, error) {
	model := NewModel()

	options := &gwob.ObjParserOptions{}
	obj, err := gwob.NewObjFromFile(modelPath, options)
	if err != nil {
		return model, err
	}

	// The gwob obj reader produces indices that are `int` instead of the `uint32` that OpenGL expects.
	var convertedIndices []uint32
	for _, v := range obj.Indices {
		convertedIndices = append(convertedIndices, uint32(v))
	}

	vertices := make([]float32, 0)
	uvs := make([]float32, 0)
	normals := make([]float32, 0)

	for i := 0; i < obj.NumberOfElements(); i++ {
		vertices = append(vertices, obj.Coord[i*8 + 0]) // X
		vertices = append(vertices, obj.Coord[i*8 + 1]) // Y
		vertices = append(vertices, obj.Coord[i*8 + 2]) // Z
		uvs = append(uvs, obj.Coord[i*8 + 3]) // X
		uvs = append(uvs, obj.Coord[i*8 + 4]) // Y
		normals = append(normals, obj.Coord[i*8 + 5]) // X
		normals = append(normals, obj.Coord[i*8 + 6]) // Y
		normals = append(normals, obj.Coord[i*8 + 7]) // Z
	}

	// The convention by most of the 3D tools seems to be to invert the Y axis.
	for i := 0; i < obj.NumberOfElements(); i++ {
		uvs[i*2 + 1] = 1 - uvs[i*2 + 1]
	}

	model.mesh = rawToMesh(int32(obj.NumberOfElements()), vertices, uvs, normals, convertedIndices)

	return model, nil
}
