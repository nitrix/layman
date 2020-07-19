package engine

type Scene struct{
	// Entities, grouped by model for optimization purposes.
	entities map[*Model][]*Entity

	// Terrains
	terrains []*Terrain

	// Lighting stuff.
	lights []*Light
	activeLight *Light

	// Camera stuff.
	cameras []*Camera
	activeCamera *Camera
}

func NewScene() *Scene {
	return &Scene{
		entities: make(map[*Model][]*Entity),
		lights:   make([]*Light, 0),
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

func (s *Scene) ActiveLight(light *Light) {
	s.activeLight = light
}

func (s *Scene) AddLight(light *Light) {
	if len(s.lights) == 0 {
		s.ActiveLight(light)
	}

	s.lights = append(s.lights, light)
}

func (s *Scene) AddTerrain(terrain *Terrain) {
	s.terrains = append(s.terrains, terrain)
}