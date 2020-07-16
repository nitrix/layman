package engine

import (
	"github.com/go-gl/gl/v4.1-core/gl"
	"github.com/go-gl/glfw/v3.3/glfw"
	"log"
	"runtime"
	"sync/atomic"
)

type Window struct {
	glfwWindow *glfw.Window
}

var glfwReferenceCount int32

func init() {
	// GLFW event handling must happen on the main thread.
	runtime.LockOSThread()
}

func NewFullScreenWindow(title string) (*Window, error) {
	incrementGlfw()

	w := &Window{}

	var err error
	monitor := glfw.GetPrimaryMonitor()
	videoMode := monitor.GetVideoMode()
	w.glfwWindow, err = glfw.CreateWindow(videoMode.Width, videoMode.Height, title, monitor, nil)
	if err != nil {
		log.Fatalln("Unable to create glfw w:", err)
	}

	w.glfwWindow.MakeContextCurrent()

	// Initialize OpenGL.
	// Must always be done within an OpenGL context (aka after calling MakeContextCurrent and before any OpenGL calls).
	if err := gl.Init(); err != nil {
		log.Fatalln("Unable to initialize OpenGL:", err)
	}

	return w, nil
}

func NewWindow(width, height int, title string) (*Window, error) {
	incrementGlfw()

	w := &Window{}

	var err error
	w.glfwWindow, err = glfw.CreateWindow(width, height, title, nil, nil)
	if err != nil {
		log.Fatalln("Unable to create glfw w:", err)
	}

	w.glfwWindow.MakeContextCurrent()

	// Initialize OpenGL.
	// Must always be done within an OpenGL context (aka after calling MakeContextCurrent and before any OpenGL calls).
	if err := gl.Init(); err != nil {
		log.Fatalln("Unable to initialize OpenGL:", err)
	}

	return w, nil
}

func (w *Window) Fullscreen() {
	monitor := glfw.GetPrimaryMonitor()
	videoMode := monitor.GetVideoMode()

	w.glfwWindow.SetMonitor(monitor, 0, 0, videoMode.Width, videoMode.Height, videoMode.RefreshRate)
}

func (w *Window) Version() string {
	return gl.GoStr(gl.GetString(gl.VERSION))
}

func (w *Window) Dimensions() (int, int) {
	return w.glfwWindow.GetFramebufferSize()
}

func (w *Window) ShouldClose() bool {
	return w.glfwWindow.ShouldClose()
}

func (w *Window) Destroy() {
	w.glfwWindow.Destroy()
	decrementGlfw()
}

func (w *Window) PollEvents() {
	glfw.PollEvents()
}

func (w *Window) Refresh() {
	w.glfwWindow.SwapBuffers()
}

func incrementGlfw() {
	newCount := atomic.AddInt32(&glfwReferenceCount, 1)
	if newCount != 1 {
		return
	}

	if err := glfw.Init(); err != nil {
		log.Fatalln("failed to initialize glfw:", err)
	}

	glfw.WindowHint(glfw.Resizable, glfw.False)
	glfw.WindowHint(glfw.ContextVersionMajor, 4)
	glfw.WindowHint(glfw.ContextVersionMinor, 1)
	glfw.WindowHint(glfw.OpenGLProfile, glfw.OpenGLCoreProfile)
	glfw.WindowHint(glfw.OpenGLForwardCompatible, glfw.True)
	glfw.WindowHint(glfw.Samples, 4)
}

func decrementGlfw() {
	newCount := atomic.AddInt32(&glfwReferenceCount, -1)
	if newCount != 0 {
		return
	}

	glfw.Terminate()
}