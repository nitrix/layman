package laygl

import (
	"github.com/go-gl/gl/v4.6-core/gl"
)

type Texture struct {
	id   uint32
	kind TextureKind
}

type TextureKind int

const (
	TextureAlbedo TextureKind = iota
	TextureNormalMap
	TextureMetallicRoughnessMap
	TextureAmbientOcclusionMap
)

func (t *Texture) Use() {
	// This is actually the recommended way to enumerate that constant.
	// You use the texture unit 0 and add your offset to it.
	gl.ActiveTexture(gl.TEXTURE0 + uint32(t.kind))
	gl.BindTexture(gl.TEXTURE_2D, t.id)
}

func (t *Texture) Unuse() {
	gl.ActiveTexture(gl.TEXTURE0 + uint32(t.kind))
	gl.BindTexture(gl.TEXTURE_2D, 0)
}
