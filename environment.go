package laygl

import (
	"fmt"
	"github.com/go-gl/gl/v4.6-core/gl"
	"github.com/mdouchement/hdr"
	_ "github.com/mdouchement/hdr/codec/rgbe"
	"github.com/mdouchement/hdr/tmo"
	"image"
	"image/draw"
	"os"
)

type Environment struct{
	texture Texture
	hammersleyPoints []float32
}

func NewEnvironment() (*Environment, error) {
	environment := &Environment{}

	environment.generateHammersleyPoints()

	err := environment.loadHDR()
	if err != nil {
		return nil, err
	}

	return environment, nil
}

func (e *Environment) loadHDR() error {
	//fi, err := os.Open("assets/pisa.hdr")
	fi, err := os.Open("assets/field.hdr")
	if err != nil {
		return err
	}

	defer func() {
		_ = fi.Close()
	}()

	m, _, err := image.Decode(fi)
	if err != nil {
		return err
	}

	if hdrm, ok := m.(hdr.Image); ok {
		t := tmo.NewLinear(hdrm)
		// t := tmo.NewLogarithmic(hdrm)
		//t := tmo.NewDefaultReinhard05(hdrm)
		m = t.Perform()
	}

	e.texture.kind = TextureEnvironmentMap

	gl.GenTextures(1, &e.texture.id)
	gl.BindTexture(gl.TEXTURE_2D, e.texture.id)
	gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_LINEAR)
	gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR_MIPMAP_LINEAR)
	gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE)
	gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE)

	rgba := image.NewRGBA(m.Bounds())
	if rgba.Stride != 4*rgba.Rect.Size().X {
		return fmt.Errorf("unsupported stride")
	}

	draw.Draw(rgba, rgba.Bounds(), m, image.Point{}, draw.Src)
	gl.TexImage2D(gl.TEXTURE_2D, 0, gl.RGBA, int32(rgba.Rect.Size().X), int32(rgba.Rect.Size().Y), 0, gl.RGBA, gl.UNSIGNED_BYTE, gl.Ptr(rgba.Pix))
	gl.GenerateMipmap(gl.TEXTURE_2D)
	gl.BindTexture(gl.TEXTURE_2D, 0)

	return nil
}

func (e *Environment) generateHammersleyPoints() {
	for i := 0; i < IBLSamples; i++ {
		u := float32(0.0)
		p := float32(0.5)
		k := i

		for k > 0 {
			if k & 1 > 0 {
				u += p
			}
			p *= 0.5
			k = k >> 1
		}

		v := (float32(i) + 0.5) / float32(IBLSamples)
		//location := gl.GetUniformLocation(program, ("hammersley_points[" + std::to_string(i) + "]").c_str());
		//gl.Uniform2f(location, u, v)
		e.hammersleyPoints = append(e.hammersleyPoints, u)
		e.hammersleyPoints = append(e.hammersleyPoints, v)
	}
}