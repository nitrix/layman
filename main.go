package main

import (
	"github.com/go-gl/glfw/v3.3/glfw"
	"github.com/go-gl/mathgl/mgl32"
	"learngl/engine"
	"log"
	"runtime/debug"
)

func main() {
	// Create a window.
	window, err := engine.NewFullScreenWindow("Learn OpenGL")
	// window, err := engine.NewWindow(1280, 720, "Learn OpenGL")
	if err != nil {
		log.Fatalln("Unable to create fullscreen window:", err)
	}
	defer window.Destroy()

	// Create the renderer.
	renderer, err := engine.NewRenderer(window)
	if err != nil {
		log.Fatalln("Unable to create renderer:", err)
	}

	// Load the helmet model.
	helmetModel, err := engine.LoadModel(engine.ModelParams{
		Name:                    "helmet",
		MeshPath:                "assets/models/helmet.obj",
		AlbedoTexturePath:       "assets/textures/helmet/helmet_albedo.png",
		NormalMapTexturePath:    "assets/textures/helmet/helmet_normal.png",
		RoughnessMapTexturePath: "assets/textures/helmet/helmet_roughness.png",
		VertexShaderPath:        "shaders/vertex.glsl",
		FragmentShaderPath:      "shaders/fragment.glsl",
		Material:                engine.DefaultMaterial,
		InitialScale:            0.05,
	})

	// Create light
	light := &engine.Light{
		Position: mgl32.Vec3{0.0, -0.3, 10.0},
		Ambient: mgl32.Vec3{0.2, 0.2, 0.2},
		Diffuse: mgl32.Vec3{1.0, 1.0, 1.0},
		Specular: mgl32.Vec3{1.0, 1.0, 1.0},
	}

	// Demo entity
	demoEntity := engine.NewEntityFromModel(helmetModel)

	// Camera
	camera := engine.NewCamera()
	camera.MoveAt(mgl32.Vec3{0, 3, 15})
	camera.LookAt(mgl32.Vec3{0, 3, 0})

	// Scene
	scene := engine.NewScene()
	scene.AddCamera(camera)
	scene.AddEntity(demoEntity)
	scene.AddLight(light)

	// Reduce the memory residency after loading assets into GPU memory.
	debug.FreeOSMemory()

	// Main loop
	previousTime := glfw.GetTime()
	for !window.ShouldClose() {
		window.PollEvents()

		// Update
		t := glfw.GetTime()
		elapsed := t - previousTime
		previousTime = t
		demoEntity.RotateY(float32(elapsed) * 0.25)

		// Render
		renderer.Render(scene)
		window.Refresh()
	}
}