package main

import (
	"github.com/go-gl/mathgl/mgl32"
	"learngl/engine"
	"log"
)

func main() {
	// Create a window.
	// window, err := engine.NewFullScreenWindow("Learn OpenGL")
	window, err := engine.NewWindow(1280, 720, "Learn OpenGL")
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

	// Load shader
	shader, err := engine.LoadShader("shaders/vertex.glsl", "shaders/fragment.glsl")
	if err != nil {
		log.Fatalln("Unable to load shader:", err)
	}

	// Load texture
	texture, err := engine.LoadTexture("assets/textures/wakfu.png")
	if err != nil {
		log.Fatalln(err)
	}

	// Load model
	model, err := engine.LoadModel("assets/models/wakfu.obj")
	if err != nil {
		log.Fatalln(err)
	}

	// Create light
	light := &engine.Light{
		Position: mgl32.Vec3{5.0, -0.3, 5.0},
		Ambient: mgl32.Vec3{0.2, 0.2, 0.2},
		Diffuse: mgl32.Vec3{1.0, 1.0, 1.0},
		Specular: mgl32.Vec3{1.0, 1.0, 1.0},
	}

	// Create material
	material := &engine.Material{
		Ambient: mgl32.Vec3{1.0, 1.0, 1.0},
		Diffuse: mgl32.Vec3{1.0, 1.0, 1.0},
		Specular: mgl32.Vec3{1.0, 1.0, 1.0},
		Shininess: 100,
	}

	// Render!
	for !window.ShouldClose() {
		window.PollEvents()

		renderer.Render(shader, texture, camera, light, material, model)
		window.Refresh()
	}
}