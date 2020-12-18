package laygl

import (
	"fmt"
	"github.com/go-gl/gl/v4.6-core/gl"
	"github.com/go-gl/glfw/v3.3/glfw"
	"runtime"
)

type window struct {
	glfwWindow      *glfw.Window
	resizeCallbacks []func()
	elapsed float32
}

func NewWindow(width, height int, title string, fullscreen bool) (*window, error) {
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

	glfwWindow, err := glfw.CreateWindow(width, height, title, monitor, nil)
	if err != nil {
		return nil, fmt.Errorf("unable to create glfw window: %w", err)
	}

	w := &window{}
	w.glfwWindow = glfwWindow

	// Switch context.
	glfwWindow.MakeContextCurrent()

	// Initialize OpenGL for each new window context.
	if err := gl.Init(); err != nil {
		return nil, fmt.Errorf("unable to initialize opengl: %w", err)
	}

	// Setup callbacks.
	glfwWindow.SetFramebufferSizeCallback(func(_ *glfw.Window, _ int, _ int) {
		for _, callback := range w.resizeCallbacks {
			callback()
		}
	})

	// 1 = Swap every 1 frame rendered (defaults to V-Sync's FPS).
	// 0 = Disable V-Sync.
	glfw.SwapInterval(0)

	return w, nil
}

func (w *window) Fullscreen() {
	monitor := glfw.GetPrimaryMonitor()
	videoMode := monitor.GetVideoMode()

	w.glfwWindow.SetMonitor(monitor, 0, 0, videoMode.Width, videoMode.Height, videoMode.RefreshRate)
}

func (w *window) SetTitle(title string) {
	w.glfwWindow.SetTitle(title)
}

func (w *window) SetSize(width, height int) {
	w.glfwWindow.SetSize(width, height)
}

func (w window) Version() string {
	return gl.GoStr(gl.GetString(gl.VERSION))
}

func (w window) Dimensions() (int, int) {
	return w.glfwWindow.GetFramebufferSize()
}

func (w window) Close() {
	w.glfwWindow.SetShouldClose(true)
}

func (w *window) Run(renderFunc func()) {
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

func (w window) Elapsed() float32 {
	return w.elapsed
}

func (w *window) OnResize(callback func()) {
	w.resizeCallbacks = append(w.resizeCallbacks, callback)
}