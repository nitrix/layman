package engine

import (
	"fmt"
	"github.com/go-gl/gl/v4.1-core/gl"
	"image"
	"image/draw"
	_ "image/jpeg"
	"os"
)

type Texture struct {
	textureId uint32
}

func (t *Texture) Use() {
	gl.ActiveTexture(gl.TEXTURE0)
	gl.BindTexture(gl.TEXTURE_2D, t.textureId)
}

func (t *Texture) Unuse() {
	gl.ActiveTexture(gl.TEXTURE0)
	gl.BindTexture(gl.TEXTURE_2D, 0)
}

func LoadTexture(filepath string) (*Texture, error) {
	texture := &Texture{}

	imgFile, err := os.Open(filepath)
	if err != nil {
		return texture, fmt.Errorf("unable to open texture file: %w", err)
	}

	img, _, err := image.Decode(imgFile)
	if err != nil {
		return texture, fmt.Errorf("unable to decode texture file: %w", err)
	}

	rgba := image.NewRGBA(img.Bounds())
	if rgba.Stride != 4 * rgba.Rect.Size().X {
		return texture, fmt.Errorf("unsupported stride")
	}

	draw.Draw(rgba, rgba.Bounds(), img, image.Point{}, draw.Src)

	gl.GenTextures(1, &texture.textureId)
	gl.BindTexture(gl.TEXTURE_2D, texture.textureId)
	gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR)
	gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR)
	gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE)
	gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE)

	gl.TexImage2D(gl.TEXTURE_2D, 0, gl.RGBA, int32(rgba.Rect.Size().X), int32(rgba.Rect.Size().Y), 0, gl.RGBA, gl.UNSIGNED_BYTE, gl.Ptr(rgba.Pix))

	return texture, nil
}