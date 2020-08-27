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
	//window, err := laygl.NewWindow(1280, 720, "Learn OpenGL")
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
	helmetModel, err := laygl.LoadModel(laygl.ModelParams{
		Name:                    "helmet",
		MeshPath:                "assets/models/helmet.obj",
		AlbedoTexturePath:       "assets/textures/helmet/helmet_albedo.png",
		NormalMapTexturePath:    "assets/textures/helmet/helmet_normal.png",
		RoughnessMapTexturePath: "assets/textures/helmet/helmet_roughness.png",
		VertexShaderPath:        "shaders/model/vertex.glsl",
		FragmentShaderPath:      "shaders/model/fragment.glsl",
		Material:                laygl.DefaultMaterial(),
		InitialScale:            0.05,
	})
	if err != nil {
		log.Fatalln("Unable to load helmet model")
	}

	// TODO: Light is directional on the Z axis. Therefore it has to be put far away in the negative space.

	// Create light
	light := laygl.DefaultLight()
	light.Position = mgl32.Vec3{0, 0, -laygl.TerrainSize}

	// Demo entity
	demoEntity := laygl.NewEntityFromModel(helmetModel)
	demoEntity.RotateY(math.Pi)

	// Camera
	camera := laygl.NewCamera()
	camera.MoveAt(mgl32.Vec3{0, 3, -15})
	camera.LookAt(mgl32.Vec3{0, 3, 0})

	// Terrains
	groundTerrainParams := laygl.TerrainParams{
		AlbedoTexturePath: "assets/textures/ground/ground_albedo.png",
	}

	terrain1, err := laygl.NewTerrain(groundTerrainParams)
	if err != nil {
		log.Fatalln(err)
	}
	terrain2, err := laygl.NewTerrain(groundTerrainParams)
	if err != nil {
		log.Fatalln(err)
	}
	terrain3, err := laygl.NewTerrain(groundTerrainParams)
	if err != nil {
		log.Fatalln(err)
	}
	terrain4, err := laygl.NewTerrain(groundTerrainParams)
	if err != nil {
		log.Fatalln(err)
	}

	terrain1.Translate(mgl32.Vec3{0, 0, 0})
	terrain2.Translate(mgl32.Vec3{-laygl.TerrainSize, 0, 0})
	terrain3.Translate(mgl32.Vec3{0, 0, -laygl.TerrainSize})
	terrain4.Translate(mgl32.Vec3{-laygl.TerrainSize, 0, -laygl.TerrainSize})

	// Scene
	scene := laygl.NewScene()
	scene.AddCamera(camera)
	scene.AddEntity(demoEntity)
	//scene.AddTerrain(terrain1)
	//scene.AddTerrain(terrain2)
	//scene.AddTerrain(terrain3)
	//scene.AddTerrain(terrain4)
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
		demoEntity.RotateY(float32(elapsed) * 0.25)
		// camera.RotateY(float32(elapsed) * 0.25)

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