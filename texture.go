package laygl

import (
	"github.com/go-gl/gl/v4.6-core/gl"
	_ "image/jpeg"
	_ "image/png"
)

type Texture struct {
	textureId uint32
	kind      TextureKind
}

type TextureKind int
const (
	TextureAlbedo TextureKind = iota
	TextureNormalMap
	TextureMetallicRoughnessMap
	TextureAmbientOcclusionMap
)

func (t *Texture) Use() {
	gl.ActiveTexture(gl.TEXTURE0 + uint32(t.kind))
	gl.BindTexture(gl.TEXTURE_2D, t.textureId)
}

func (t *Texture) Unuse() {
	gl.ActiveTexture(gl.TEXTURE0 + uint32(t.kind))
	gl.BindTexture(gl.TEXTURE_2D, 0)
}
