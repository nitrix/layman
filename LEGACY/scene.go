package laygl

import "github.com/nitrix/laygl/lights"

type scene struct {
	camera *camera

	// Entities, grouped by model for optimization purposes.
	entities map[*Model][]*Entity

	// Lighting stuff.
	lights      []lights.Light
}

func NewScene() *scene {
	return &scene{
		camera: NewCamera(),
		entities: make(map[*Model][]*Entity),
	}
}

func (s *scene) Camera() *camera {
	return s.camera
}

func (s *scene) AddEntity(entity *Entity) {
	if s.entities[entity.model] == nil {
		s.entities[entity.model] = make([]*Entity, 0)
	}

	s.entities[entity.model] = append(s.entities[entity.model], entity)
}

func (s *scene) AddLight(light lights.Light) {
	s.lights = append(s.lights, light)
}