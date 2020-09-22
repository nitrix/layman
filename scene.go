package laygl

import lights2 "github.com/nitrix/laygl/lights"

type Scene struct {
	// Entities, grouped by model for optimization purposes.
	entities map[*Model][]*Entity

	// Lighting stuff.
	lights      []lights2.Light

	// Camera stuff.
	cameras      []*Camera
	activeCamera *Camera
}

func NewScene() *Scene {
	return &Scene{
		entities: make(map[*Model][]*Entity),
		lights:   make([]lights2.Light, 0),
		cameras:  make([]*Camera, 0),
	}
}

func (s *Scene) ActiveCamera(camera *Camera) {
	s.activeCamera = camera
}

func (s *Scene) AddCamera(camera *Camera) {
	if len(s.cameras) == 0 {
		s.ActiveCamera(camera)
	}

	s.cameras = append(s.cameras, camera)
}

func (s *Scene) AddEntity(entity *Entity) {
	if s.entities[entity.model] == nil {
		s.entities[entity.model] = make([]*Entity, 0)
	}

	s.entities[entity.model] = append(s.entities[entity.model], entity)
}

func (s *Scene) AddLight(light lights2.Light) {
	s.lights = append(s.lights, light)
}
