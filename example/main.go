package main

import (
	"fmt"
	"github.com/nitrix/laygl"
	"github.com/nitrix/laygl/lights"
	"log"
	"time"
)

func main() {
	// Create a window.
	t := time.Now()
	window, err := laygl.NewWindow(1280, 720, "Demo", false)
	if err != nil {
		log.Fatalln("Unable to create fullscreen window:", err)
	}
	log.Println("Time to create the window:", time.Now().Sub(t))

	// Create the renderer.
	t = time.Now()
	renderer, err := laygl.NewRenderer(window)
	if err != nil {
		log.Fatalln("Unable to create renderer:", err)
	}
	log.Println("Time to create the renderer:", time.Now().Sub(t))

	// Load the model.
	t = time.Now()
	model, err := laygl.LoadModel("helmet", "assets/DamagedHelmet.glb")
	//model, err := laygl.LoadModel("helmet", "assets/cowboy.glb")
	//model, err := laygl.LoadModel("helmet", "assets/helmet.glb")
	//model, err := laygl.LoadModel("helmet", "assets/corset.glb")
	//model, err := laygl.LoadModel("helmet", "assets/boombox.glb")
	//model, err := laygl.LoadModel("helmet", "assets/illya.glb")
	//model, err := laygl.LoadModel("helmet", "assets/wakfu.glb")
	//model, err := laygl.LoadModel("helmet", "assets/BoomBox.glb")
	if err != nil {
		log.Fatalln("Unable to load model:", err)
	}
	log.Println("Time to load the model:", time.Now().Sub(t))

	// Create directional light
	directionalLight := lights.NewDirectional()

	// Entity
	entity := laygl.NewEntityFromModel(model)

	// Camera
	camera := laygl.NewCamera()
	camera.MoveAt(0, 0, 0)
	camera.LookAt(0, 0, -1)

	// Scene
	scene := laygl.NewScene()
	scene.AddCamera(camera)
	scene.AddEntity(entity)
	scene.AddLight(&directionalLight)
	//scene.AddLight(&pointLight)

	fps := 0
	lastTime := time.Now()

	entity.Translate(0, 0, -3)

	window.Run(func() {
		// Update.
		entity.Rotate(0, window.Elapsed() * 0.5, 0)

		// Render.
		renderer.Render(scene)

		// Print FPS.
		fps++
		if time.Since(lastTime) > time.Second {
			fmt.Println("FPS:", fps)
			fps = 0
			lastTime = time.Now()
		}
	})
}