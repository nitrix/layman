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

	// Create the camera.
	camera := engine.NewCamera()
	camera.Move(mgl32.Vec3{0, 3, 15})
	camera.LookAt(mgl32.Vec3{0, 3, 0})

	// Load shader
	shader, err := engine.LoadShader("shaders/vertex.glsl", "shaders/fragment.glsl")
	if err != nil {
		log.Fatalln("Unable to load shader:", err)
	}

	// Load albedo texture
	textureAlbedo, err := engine.LoadTexture(engine.TextureAlbedo, "assets/textures/helmet/helmet_albedo.png")
	if err != nil {
		log.Fatalln(err)
	}

	// Load normal map texture
	textureNormalMap, err := engine.LoadTexture(engine.TextureNormalMap, "assets/textures/helmet/helmet_normal.png")
	if err != nil {
		log.Fatalln(err)
	}

	// Load roughness map texture
	textureRoughnessMap, err := engine.LoadTexture(engine.TextureRoughnessMap, "assets/textures/helmet/helmet_roughness.png")
	if err != nil {
		log.Fatalln(err)
	}

	// Load glow map texture
	/*
	textureGlowMap, err := engine.LoadTexture(engine.TextureGlowMap, "assets/textures/helmet/helmet_glow.jpg")
	if err != nil {
		log.Fatalln(err)
	}
	*/
	textureGlowMap := &engine.Texture{}

	// Load model
	model, err := engine.LoadModel("assets/models/helmet.obj")
	if err != nil {
		log.Fatalln(err)
	}
	model.Scale(0.05)

	// Create light
	light := &engine.Light{
		Position: mgl32.Vec3{0.0, -0.3, 10.0},
		Ambient: mgl32.Vec3{0.2, 0.2, 0.2},
		Diffuse: mgl32.Vec3{1.0, 1.0, 1.0},
		Specular: mgl32.Vec3{1.0, 1.0, 1.0},
	}

	// Create material
	material := &engine.Material{
		Ambient: mgl32.Vec3{1.0, 1.0, 1.0},
		Diffuse: mgl32.Vec3{1.0, 1.0, 1.0},
		Specular: mgl32.Vec3{1.0, 1.0, 1.0},
		Shininess: 10,
	}

	previousTime := glfw.GetTime()

	// Reduce the memory residency after loading assets into GPU memory.
	debug.FreeOSMemory()

	// Main loop
	for !window.ShouldClose() {
		window.PollEvents()

		// Update
		t := glfw.GetTime()
		elapsed := t - previousTime
		previousTime = t
		model.RotateY(float32(elapsed) * 0.25)

		// Render
		renderer.Render(shader, textureAlbedo, textureNormalMap, textureRoughnessMap, textureGlowMap, camera, light, material, model)
		window.Refresh()
	}
}