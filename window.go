package laygl

import (
	"fmt"
	"github.com/go-gl/gl/v4.6-core/gl"
	"github.com/go-gl/glfw/v3.3/glfw"
	"runtime"
	"sync/atomic"
)

type Window struct {
	glfwWindow      *glfw.Window
	resizeCallbacks []func()
	renderCallbacks []func()
	tasks chan func()
}

var refCount int32

func NewFullScreenWindow(title string) (*Window, error) {
	monitor := glfw.GetPrimaryMonitor()
	videoMode := monitor.GetVideoMode()

	return newWindow(videoMode.Width, videoMode.Height, title, monitor)
}

func NewWindow(width, height int, title string) (*Window, error) {
	window, err := newWindow(width, height, title, nil)
	if err != nil {
		return nil, err
	}

	// Refresh every frame rendered. This is to please the window compositor on Windows, otherwise it stutters.
	// The FPS should still remain that of the VSync.
	glfw.SwapInterval(1)

	return window, nil
}

func newWindow(width, height int, title string, monitor *glfw.Monitor) (*Window, error) {
	// GLFW event handling must happen on the main thread.
	runtime.LockOSThread()

	err := incrementGlfw()
	if err != nil {
		return nil, fmt.Errorf("unable to initialize glfw: %w", err)
	}

	glfw.WindowHint(glfw.Resizable, glfw.True)
	glfw.WindowHint(glfw.ContextVersionMajor, 4)
	glfw.WindowHint(glfw.ContextVersionMinor, 6)
	glfw.WindowHint(glfw.OpenGLProfile, glfw.OpenGLCoreProfile)
	glfw.WindowHint(glfw.Samples, AntiAliasingSamples)

	glfwWindow, err := glfw.CreateWindow(width, height, title, monitor, nil)
	if err != nil {
		decrementGlfw()
		return nil, fmt.Errorf("unable to create glfw window: %w", err)
	}

	window := &Window{}
	window.glfwWindow = glfwWindow

	// Switch context.
	savedContext := glfw.GetCurrentContext()
	glfwWindow.MakeContextCurrent()

	// Initialize OpenGL for each new window context.
	if err := gl.Init(); err != nil {
		decrementGlfw()
		return nil, fmt.Errorf("unable to initialize opengl: %w", err)
	}

	// Restore context.
	if savedContext != nil {
		savedContext.MakeContextCurrent()
	}

	// Tasks.
	window.tasks = make(chan func())

	// Setup callbacks.
	glfwWindow.SetFramebufferSizeCallback(func(w *glfw.Window, width int, height int) {
		window.tasks <- func() {
			for _, callback := range window.resizeCallbacks {
				callback()
			}
		}
	})

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

func (w Window) Destroy() {
	w.glfwWindow.Destroy()
	decrementGlfw()
}

func (w *Window) Run() {
	glfw.DetachCurrentContext()

	go func() {
		// This rendering goroutine must stay on whatever thread it's on because we're about to
		// see the current context and that's a "per thread" mechanism.
		runtime.LockOSThread()
		w.glfwWindow.MakeContextCurrent()

		// Main loop.
		for !w.glfwWindow.ShouldClose() {
			select {
			case task := <- w.tasks:
				task()
			default:
			}

			for _, callback := range w.renderCallbacks {
				callback()
			}

			w.glfwWindow.SwapBuffers()
		}
	}()

	for !w.glfwWindow.ShouldClose() {
		glfw.PollEvents()
	}
}

func (w *Window) OnResize(callback func()) {
	w.resizeCallbacks = append(w.resizeCallbacks, callback)
}

func (w *Window) OnRender(callback func()) {
	w.renderCallbacks = append(w.renderCallbacks, callback)
}

func incrementGlfw() error {
	newCount := atomic.AddInt32(&refCount, 1)

	if newCount == 1 {
		if err := glfw.Init(); err != nil {
			atomic.AddInt32(&refCount, -1)
			return err
		}
	}

	return nil
}

func decrementGlfw() {
	newCount := atomic.AddInt32(&refCount, -1)
	if newCount == 0 {
		glfw.Terminate()
	}
}
