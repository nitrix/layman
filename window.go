package laygl

import (
	"fmt"
	"github.com/go-gl/gl/v4.6-core/gl"
	"github.com/go-gl/glfw/v3.3/glfw"
	"runtime"
)

type Window struct {
	glfwWindow      *glfw.Window
	resizeCallbacks []func()
	elapsed float32
}

func NewWindow(width, height int, title string, fullscreen bool) (*Window, error) {
	// GLFW event handling must happen on the main thread.
	runtime.LockOSThread()

	if err := glfw.Init(); err != nil {
		return nil, fmt.Errorf("unable to initialize glfw: %w", err)
	}

	monitor := glfw.GetPrimaryMonitor()
	videoMode := monitor.GetVideoMode()

	if width <= 0 {
		width = videoMode.Width
	}

	if height <= 0 {
		height = videoMode.Height
	}

	if !fullscreen {
		monitor = nil
	}

	glfw.WindowHint(glfw.Resizable, glfw.True)
	glfw.WindowHint(glfw.ContextVersionMajor, 4)
	glfw.WindowHint(glfw.ContextVersionMinor, 6)
	glfw.WindowHint(glfw.OpenGLProfile, glfw.OpenGLCoreProfile)
	//glfw.WindowHint(glfw.Samples, AntiAliasingSamples)

	glfwWindow, err := glfw.CreateWindow(width, height, title, monitor, nil)
	if err != nil {
		return nil, fmt.Errorf("unable to create glfw window: %w", err)
	}

	window := &Window{}
	window.glfwWindow = glfwWindow

	// Switch context.
	glfwWindow.MakeContextCurrent()

	// Initialize OpenGL for each new window context.
	if err := gl.Init(); err != nil {
		return nil, fmt.Errorf("unable to initialize opengl: %w", err)
	}

	// Setup callbacks.
	glfwWindow.SetFramebufferSizeCallback(func(w *glfw.Window, width int, height int) {
		for _, callback := range window.resizeCallbacks {
			callback()
		}
	})

	// Refresh every frame rendered. This is to please the window compositor on Windows, otherwise it stutters.
	// The FPS should still remain that of the VSync.
	glfw.SwapInterval(1)

	return window, nil
}

func (w *Window) Fullscreen() {
	monitor := glfw.GetPrimaryMonitor()
	videoMode := monitor.GetVideoMode()

	w.glfwWindow.SetMonitor(monitor, 0, 0, videoMode.Width, videoMode.Height, videoMode.RefreshRate)
}

func (w Window) Version() string {
	return gl.GoStr(gl.GetString(gl.VERSION))
}

func (w Window) Dimensions() (int, int) {
	return w.glfwWindow.GetFramebufferSize()
}

func (w Window) Close() {
	w.glfwWindow.SetShouldClose(true)
}

func (w *Window) Run(renderFunc func()) {
	previousTime := glfw.GetTime()

	for !w.glfwWindow.ShouldClose() {
		// Handle events.
		glfw.PollEvents()

		// Update time.
		t := glfw.GetTime()
		w.elapsed = float32(t - previousTime)
		previousTime = t

		// Render.
		renderFunc()

		// Present.
		w.glfwWindow.SwapBuffers()
	}
}

func (w Window) Elapsed() float32 {
	return w.elapsed
}

func (w *Window) OnResize(callback func()) {
	w.resizeCallbacks = append(w.resizeCallbacks, callback)
}