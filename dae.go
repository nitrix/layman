package laygl

import (
	"errors"
	"github.com/go-gl/mathgl/mgl32"
	"github.com/nitrix/collada"
	"math"
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

	// Take into account the up axis.
	switch c.UpAxis {
	case collada.X_Up:
		model.initialTransform = mgl32.HomogRotate3DZ(math.Pi/2).Mul4(model.initialTransform)
	case collada.Y_Up:
		// That's the same coordinate system that we're using.
	case collada.Z_Up:
		model.initialTransform = mgl32.HomogRotate3DX(-math.Pi/2).Mul4(model.initialTransform)
	}

	return model, nil
}
