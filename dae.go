package laygl

import (
	"errors"
	"github.com/nitrix/collada"
)

// TODO: Interface for model loaders? Own packages even?

func LoadDae(filename string) (*Model, error) {
	c, err := collada.Load(filename)
	if err != nil {
		return nil, err
	}

	// TODO: Models currently only support one mesh. This should change.
	if len(c.Meshes) > 1 {
		return nil, errors.New("too many meshes")
	}

	// The convention by most of the 3D tools seems to be to invert the Y axis.
	for i := 0; i < len(c.Meshes[0].TextureCoords) / 2; i++ {
		c.Meshes[0].TextureCoords[i*2 + 1] = 1 - c.Meshes[0].TextureCoords[i*2 + 1]
	}

	mesh := rawToMesh(
		c.Meshes[0].TriangleCount,
		c.Meshes[0].Vertices,
		c.Meshes[0].TextureCoords,
		c.Meshes[0].Normals,
		c.Meshes[0].Indices,
	)

	model := NewModel()
	model.mesh = mesh

	// Blender models are Z axis aligned up for some reason. Gotta rotate it.
	// model.initialTransform = model.initialTransform.Mul4(mgl32.HomogRotate3DX(90))

	return model, nil
}
