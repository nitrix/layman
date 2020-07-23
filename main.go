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
		VertexShaderPath:        "shaders/model/vertex.glsl",
		FragmentShaderPath:      "shaders/model/fragment.glsl",
		Material:                engine.DefaultMaterial(),
		InitialScale:            0.05,
	})
	if err != nil {
		log.Fatalln("Unable to load helmet model")
	}

	// Create light
	light := engine.DefaultLight()
	light.Position = mgl32.Vec3{0.0, 3, 10.0}

	// Demo entity
	demoEntity := engine.NewEntityFromModel(helmetModel)

	// Camera
	camera := engine.NewCamera()
	camera.MoveAt(mgl32.Vec3{0, 3, 15})
	camera.LookAt(mgl32.Vec3{0, 3, 0})

	// Terrains
	groundTerrainParams := engine.TerrainParams{
		AlbedoTexturePath: "assets/textures/ground/ground_albedo.png",
		NormalMapTexturePath: "assets/textures/ground/ground_normal.png",
		RoughnessMapTexturePath: "assets/textures/ground/ground_roughness.png",
	}

	terrain1, err := engine.NewTerrain(groundTerrainParams)
	if err != nil {
		log.Fatalln(err)
	}
	terrain2, err := engine.NewTerrain(groundTerrainParams)
	if err != nil {
		log.Fatalln(err)
	}
	terrain3, err := engine.NewTerrain(groundTerrainParams)
	if err != nil {
		log.Fatalln(err)
	}
	terrain4, err := engine.NewTerrain(groundTerrainParams)
	if err != nil {
		log.Fatalln(err)
	}

	terrain1.Translate(mgl32.Vec3{0, 0, 0})
	terrain2.Translate(mgl32.Vec3{-engine.TerrainSize, 0, 0})
	terrain3.Translate(mgl32.Vec3{0, 0, -engine.TerrainSize})
	terrain4.Translate(mgl32.Vec3{-engine.TerrainSize, 0, -engine.TerrainSize})

	// Scene
	scene := engine.NewScene()
	scene.AddCamera(camera)
	scene.AddEntity(demoEntity)
	scene.AddTerrain(terrain1)
	scene.AddTerrain(terrain2)
	scene.AddTerrain(terrain3)
	scene.AddTerrain(terrain4)
	scene.AddLight(&light)

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