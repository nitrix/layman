package laygl

import (
	"github.com/go-gl/gl/v3.3-core/gl"
)

type Framebuffer struct{
	fbo uint32
	colorBuffer uint32
	depthBuffer uint32
}

func NewFramebuffer(width, height int) *Framebuffer {
	fb := &Framebuffer{}

	gl.GenFramebuffers(1, &fb.fbo)
	gl.BindFramebuffer(gl.FRAMEBUFFER, fb.fbo)

	// Color attachment.
	gl.GenTextures(1, &fb.colorBuffer)
	gl.BindTexture(gl.TEXTURE_2D_MULTISAMPLE, fb.colorBuffer)
	gl.TexImage2DMultisample(gl.TEXTURE_2D_MULTISAMPLE, AntiAliasingSamples, gl.RGBA16F, int32(width), int32(height), true)
	gl.FramebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D_MULTISAMPLE, fb.colorBuffer, 0)

	// Depth attachment.
	gl.GenRenderbuffers(1, &fb.depthBuffer)
	gl.BindRenderbuffer(gl.RENDERBUFFER, fb.depthBuffer)
	gl.RenderbufferStorageMultisample(gl.RENDERBUFFER, AntiAliasingSamples, gl.DEPTH_COMPONENT, int32(width), int32(height))
	gl.FramebufferRenderbuffer(gl.FRAMEBUFFER, gl.DEPTH_ATTACHMENT, gl.RENDERBUFFER, fb.depthBuffer)

	// Unbind.
	gl.BindFramebuffer(gl.FRAMEBUFFER, 0)

	return fb
}

func (fb *Framebuffer) Destroy() {
	gl.DeleteTextures(1, &fb.colorBuffer)
	gl.DeleteRenderbuffers(1, &fb.depthBuffer)
	gl.DeleteFramebuffers(1, &fb.fbo)
}

func (fb *Framebuffer) Use() {
	gl.BindFramebuffer(gl.FRAMEBUFFER, fb.fbo)
}

func (fb *Framebuffer) Unuse() {
	gl.BindFramebuffer(gl.FRAMEBUFFER, 0)
}