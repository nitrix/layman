package main

import (
	"fmt"
	"github.com/go-gl/glfw/v3.3/glfw"
	"github.com/go-gl/mathgl/mgl32"
	"github.com/nitrix/laygl"
	"log"
	"math"
	"runtime/debug"
	"time"
)

func main() {
	// Create a window.
	window, err := laygl.NewFullScreenWindow("Learn OpenGL")
	// window, err := laygl.NewWindow(1280, 720, "Learn OpenGL")
	if err != nil {
		log.Fatalln("Unable to create fullscreen window:", err)
	}
	defer window.Destroy()

	// Create the renderer.
	renderer, err := laygl.NewRenderer(window)
	if err != nil {
		log.Fatalln("Unable to create renderer:", err)
	}

	// Load the helmet model.
	model, err := laygl.LoadModel(laygl.ModelParams{
		Name:                    "helmet",
		ModelPath:               "assets/models/helmet.obj",
		AlbedoTexturePath:       "assets/textures/helmet/helmet_albedo.png",
		NormalMapTexturePath:    "assets/textures/helmet/helmet_normal.png",
		RoughnessMapTexturePath: "assets/textures/helmet/helmet_roughness.png",
		VertexShaderPath:        "shaders/model/vertex.glsl",
		FragmentShaderPath:      "shaders/model/fragment.glsl",
		Material:                laygl.DefaultMaterial(),
		InitialScale:            0.05,
		InitialRotation:         mgl32.Vec3{0, math.Pi, 0},
	})
	if err != nil {
		log.Fatalln("Unable to load helmet model:", err)
	}

	// Create light
	light := laygl.DefaultLight()
	light.Position = mgl32.Vec3{0, 3, -15}

	// Entity
	entity := laygl.NewEntityFromModel(model)

	// Camera
	camera := laygl.NewCamera()
	camera.MoveAt(mgl32.Vec3{0, 3, -15})
	camera.LookAt(mgl32.Vec3{0, 3, 0})

	// Scene
	scene := laygl.NewScene()
	scene.AddCamera(camera)
	scene.AddEntity(entity)
	scene.AddLight(&light)

	// Reduce the memory residency after loading assets into GPU memory.
	debug.FreeOSMemory()

	fps := 0
	lastTime := time.Now()

	// Main loop
	previousTime := glfw.GetTime()
	for !window.ShouldClose() {
		window.PollEvents()

		// Update
		t := glfw.GetTime()
		elapsed := t - previousTime
		previousTime = t
		entity.RotateY(float32(elapsed) * 0.25)

		// Render
		renderer.Render(scene)
		window.Refresh()

		// Print FPS
		fps++
		if time.Since(lastTime) > time.Second {
			fmt.Println("FPS:", fps)
			fps = 0
			lastTime = time.Now()
		}
	}
}