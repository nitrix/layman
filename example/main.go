package main

import (
	"fmt"
	"github.com/nitrix/laygl"
	"github.com/nitrix/laygl/lights"
	"log"
	"runtime/debug"
	"time"
)

func main() {
	// Create a window.
	//window, err := laygl.NewFullScreenWindow("Demo")
	window, err := laygl.NewWindow(1280, 720, "Demo")
	if err != nil {
		log.Fatalln("Unable to create fullscreen window:", err)
	}
	defer window.Destroy()

	// Create the renderer.
	renderer, err := laygl.NewRenderer(window)
	if err != nil {
		log.Fatalln("Unable to create renderer:", err)
	}

	// Load the model.
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

	// Create directional light
	directionalLight := lights.NewDirectional()

	/*
	pointLight := lights.NewPoint()
	pointLight.Position = mgl32.Vec3{
		0, 0, 0,
	}
	pointLight.Irradiance = mgl32.Vec3{
		40.0, 40.0, 40.0,
	}

	pointLight.ConstantAttenuation = 1
	pointLight.LinearAttenuation = 0.3
	pointLight.QuadraticAttenuation = 0.03
	*/

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

	// Reduce the memory residency after loading assets into GPU memory.
	debug.FreeOSMemory()

	fps := 0
	lastTime := time.Now()

	entity.Translate(0, 0, -3)

	window.OnRender(func() {
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

	window.Run()
}
