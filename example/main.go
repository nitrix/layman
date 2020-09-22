package main

import (
	"fmt"
	"github.com/go-gl/glfw/v3.3/glfw"
	"github.com/nitrix/laygl"
	"github.com/nitrix/laygl/lights"
	"log"
	"runtime/debug"
	"time"
)

// TODO: Get rid of mgl32 at top-level.

func main() {
	// Create a window.
	//window, err := laygl.NewFullScreenWindow("Layman OpenGL")
	window, err := laygl.NewWindow(1280, 720, "Layman OpenGL")
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
	//model, err := laygl.LoadModel("helmet", "assets/cowboy.glb")
	model, err := laygl.LoadModel("helmet", "assets/helmet.glb")
	//model, err := laygl.LoadModel("helmet", "assets/corset.glb")
	//model, err := laygl.LoadModel("helmet", "assets/boombox.glb")
	//model, err := laygl.LoadModel("helmet", "assets/illya.glb")
	if err != nil {
		log.Fatalln("Unable to load model:", err)
	}

	// glm::vec3 position, GLfloat const_atten, GLfloat linear_atten,
	//    GLfloat quad_atten, glm::vec3 irradiance

	// Create directional light
	directionalLight := lights.NewDirectional()

	/*
	pointLight := lights.NewPoint()
	pointLight.Position = mgl32.Vec3{
		0, 0, 20,
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

	entity.Translate(0, 0, -20)

	previousTime := glfw.GetTime()

	window.OnRender(func() {
		// Update
		t := glfw.GetTime()
		elapsed := t - previousTime
		previousTime = t

		entity.Rotate(0, float32(elapsed)*0.5, 0)

		// Render
		renderer.Render(scene)

		// Print FPS
		fps++
		if time.Since(lastTime) > time.Second {
			fmt.Println("FPS:", fps)
			fps = 0
			lastTime = time.Now()
		}
	})

	window.Run()
}
