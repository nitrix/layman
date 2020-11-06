package laygl

import (
	"errors"
	"github.com/go-gl/mathgl/mgl32"
	"path/filepath"
	"runtime/debug"
)

type Model struct {
	name             string
	meshes           []*Mesh
	initialTransform mgl32.Mat4
}

func NewModel() *Model {
	return &Model{
		meshes:           make([]*Mesh, 0),
		initialTransform: mgl32.Ident4(),
	}
}

func LoadModel(name, path string) (*Model, error) {
	if filepath.Ext(path) != ".gltf" && filepath.Ext(path) != ".glb" {
		return nil, errors.New("only glTF is currently supported")
	}

	gltf := Gltf{}
	model, err := gltf.loadModel(path)
	if err != nil {
		return nil, err
	}

	model.name = name

	// Reduce the memory residency after loading a model.
	debug.FreeOSMemory()

	return model, nil
}
