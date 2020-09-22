package laygl

import (
	"bytes"
	"errors"
	"fmt"
	"github.com/go-gl/gl/v4.6-core/gl"
	"github.com/go-gl/mathgl/mgl32"
	"github.com/qmuntal/gltf"
	"image"
	"image/draw"
	_ "image/jpeg"
	_ "image/png"
	"math"
	"os"
	"reflect"
	"unsafe"
)

type Gltf struct {
	texture *Texture
	mesh *Mesh
	model *Model
	document *gltf.Document
}

func (g *Gltf) loadModel(gltfFilepath string) (*Model, error) {
	g.model = NewModel()

	document, err := gltf.Open(gltfFilepath)
	if err != nil {
		return nil, err
	}

	g.document = document

	err = g.loadMeshes()
	if err != nil {
		return nil, err
	}

	g.initialModelTransform()

	return g.model, nil
}

func (g *Gltf) loadMeshes() error {
	meshes := make([]*Mesh, 0)

	for _, m := range g.document.Meshes {
		err := g.loadMesh(m)
		if err != nil {
			return err
		}

		meshes = append(meshes, g.mesh)
	}

	g.model.meshes = meshes

	return nil
}

func (g *Gltf) loadMesh(m *gltf.Mesh) error {
	g.mesh = &Mesh{}
	g.mesh.name = m.Name

	// Create the VAO on the GPU, then use it.
	gl.GenVertexArrays(1, &g.mesh.vao)
	gl.BindVertexArray(g.mesh.vao)

	for _, primitive := range m.Primitives {
		if primitive.Mode != gltf.PrimitiveTriangles {
			return errors.New("only triangle primitives supported")
		}

		for name, accessorId := range primitive.Attributes {
			accessor := g.document.Accessors[accessorId]
			err := g.loadAttribute(name, accessor)
			if err != nil {
				return err
			}
		}

		err := g.loadIndices(g.document.Accessors[*primitive.Indices])
		if err != nil {
			return err
		}

		// Load material.
		err = g.loadMaterial(g.document.Materials[*primitive.Material])
		if err != nil {
			return err
		}

		// Load tangents.
		err = g.loadTangents()
		if err != nil {
			return err
		}
	}

	// Load shader program.
	err := g.loadShader()
	if err != nil {
		return err
	}

	return nil
}

func (g *Gltf) loadAttribute(name string, accessor *gltf.Accessor) error {
	// Vertices.
	// Vertex Buffer Object (VBO).
	if name == "POSITION" {
		if accessor.Type != gltf.AccessorVec3 || accessor.ComponentType != gltf.ComponentFloat {
			return errors.New("position accessor is not a vec3 of floats")
		}

		g.mesh.indiceCount = accessor.Count / 3
		bufferView := g.document.BufferViews[*accessor.BufferView]
		buffer := g.document.Buffers[bufferView.Buffer]
		data := buffer.Data[bufferView.ByteOffset+accessor.ByteOffset:bufferView.ByteOffset+bufferView.ByteLength]

		var vertices []float32
		sliceHeader := (*reflect.SliceHeader)(unsafe.Pointer(&vertices))
		sliceHeader.Data = (uintptr)(unsafe.Pointer(&data[0]))
		sliceHeader.Len = len(data) / int(unsafe.Sizeof(float32(0)))
		sliceHeader.Cap = cap(data) / int(unsafe.Sizeof(float32(0)))
		g.mesh.vertices = vertices

		gl.GenBuffers(1, &g.mesh.verticesBufferId)
		gl.BindBuffer(gl.ARRAY_BUFFER, g.mesh.verticesBufferId)
		gl.BufferData(gl.ARRAY_BUFFER, int(bufferView.ByteLength), gl.Ptr(data), gl.STATIC_DRAW)
		gl.EnableVertexAttribArray(ShaderAttributeVertexCoords)
		gl.VertexAttribPointer(ShaderAttributeVertexCoords, 3, gl.FLOAT, false, int32(bufferView.ByteStride), gl.PtrOffset(0))
	}

	// Texture UVs.
	// TODO: Hardcoded texture attribute.
	// TODO: Support more than one texture per mesh? Oh, multi-texturing?
	if name == "TEXCOORD_0" {
		if accessor.Type != gltf.AccessorVec2 || accessor.ComponentType != gltf.ComponentFloat {
			return errors.New("TEXCOORD_0 accessor is not a vec2 of floats")
		}

		bufferView := g.document.BufferViews[*accessor.BufferView]
		buffer := g.document.Buffers[bufferView.Buffer]
		data := buffer.Data[bufferView.ByteOffset+accessor.ByteOffset:bufferView.ByteOffset+bufferView.ByteLength]

		var uvs []float32
		sliceHeader := (*reflect.SliceHeader)(unsafe.Pointer(&uvs))
		sliceHeader.Data = (uintptr)(unsafe.Pointer(&data[0]))
		sliceHeader.Len = len(data) / int(unsafe.Sizeof(float32(0)))
		sliceHeader.Cap = cap(data) / int(unsafe.Sizeof(float32(0)))
		g.mesh.uvs = uvs

		gl.GenBuffers(1, &g.mesh.uvsBufferId)
		gl.BindBuffer(gl.ARRAY_BUFFER, g.mesh.uvsBufferId)
		gl.BufferData(gl.ARRAY_BUFFER, int(bufferView.ByteLength), gl.Ptr(data), gl.STATIC_DRAW)
		gl.EnableVertexAttribArray(ShaderAttributeTextureCoords)
		gl.VertexAttribPointer(ShaderAttributeTextureCoords, 2, gl.FLOAT, false, 2 * 4, gl.PtrOffset(0))
	}

	// Normals.
	if name == "NORMAL" {
		if accessor.Type != gltf.AccessorVec3 || accessor.ComponentType != gltf.ComponentFloat {
			return errors.New("normal accessor is not a vec3 of floats")
		}

		bufferView := g.document.BufferViews[*accessor.BufferView]
		buffer := g.document.Buffers[bufferView.Buffer]
		data := buffer.Data[bufferView.ByteOffset+accessor.ByteOffset:bufferView.ByteOffset+bufferView.ByteLength]

		var normals []float32
		sliceHeader := (*reflect.SliceHeader)(unsafe.Pointer(&normals))
		sliceHeader.Data = (uintptr)(unsafe.Pointer(&data[0]))
		sliceHeader.Len = len(data) / int(unsafe.Sizeof(float32(0)))
		sliceHeader.Cap = cap(data) / int(unsafe.Sizeof(float32(0)))
		g.mesh.normals = normals

		gl.GenBuffers(1, &g.mesh.normalsBufferId)
		gl.BindBuffer(gl.ARRAY_BUFFER, g.mesh.normalsBufferId)
		gl.BufferData(gl.ARRAY_BUFFER, int(bufferView.ByteLength), gl.Ptr(data), gl.STATIC_DRAW)
		gl.EnableVertexAttribArray(ShaderAttributeNormals)
		gl.VertexAttribPointer(ShaderAttributeNormals, 3, gl.FLOAT, false, 3 * 4, gl.PtrOffset(0))
	}

	return nil
}

func (g *Gltf) loadIndices(accessor *gltf.Accessor) error {
	// Indices.
	// Faces (EBO).
	bufferView := g.document.BufferViews[*accessor.BufferView]
	buffer := g.document.Buffers[bufferView.Buffer]
	data := buffer.Data[bufferView.ByteOffset+accessor.ByteOffset:bufferView.ByteOffset+bufferView.ByteLength]

	if accessor.Type != gltf.AccessorScalar || accessor.ComponentType != gltf.ComponentUshort {
		return errors.New("position accessor is not a scalar ushort")
	}

	var indices []uint16
	sliceHeader := (*reflect.SliceHeader)(unsafe.Pointer(&indices))
	sliceHeader.Data = (uintptr)(unsafe.Pointer(&data[0]))
	sliceHeader.Len = len(data) / int(unsafe.Sizeof(uint16(0)))
	sliceHeader.Cap = cap(data) / int(unsafe.Sizeof(uint16(0)))
	g.mesh.indices = indices

	gl.GenBuffers(1, &g.mesh.indicesBufferId)
	gl.BindBuffer(gl.ELEMENT_ARRAY_BUFFER, g.mesh.indicesBufferId)
	gl.BufferData(gl.ELEMENT_ARRAY_BUFFER, int(bufferView.ByteLength), gl.Ptr(data), gl.STATIC_DRAW)

	g.mesh.indiceCount = accessor.Count

	return nil
}

func (g *Gltf) loadMaterial(m *gltf.Material) error {
	g.mesh.material = DefaultMaterial()

	// PBR
	if m.PBRMetallicRoughness != nil {
		// Albedo (base color) texture.
		if m.PBRMetallicRoughness.BaseColorTexture != nil {
			err := g.loadTexture(TextureAlbedo, g.document.Textures[m.PBRMetallicRoughness.BaseColorTexture.Index])
			if err != nil {
				return err
			}

			g.mesh.albedoTexture = g.texture
		}

		// Metallic/roughness texture.
		if m.PBRMetallicRoughness.MetallicRoughnessTexture != nil {
			err := g.loadTexture(TextureMetallicRoughnessMap, g.document.Textures[m.PBRMetallicRoughness.MetallicRoughnessTexture.Index])
			if err != nil {
				return err
			}

			g.mesh.metallicRoughnessMapTexture = g.texture
		}
	}

	// Normal map texture.
	if m.NormalTexture != nil {
		err := g.loadTexture(TextureNormalMap, g.document.Textures[*m.NormalTexture.Index])
		if err != nil {
			return err
		}

		g.mesh.normalMapTexture = g.texture
	}

	return nil
}

func (g *Gltf) loadTexture(kind TextureKind, t *gltf.Texture) error {
	if t.Source != nil {
		img := g.document.Images[*t.Source]

		g.texture = &Texture{
			kind: kind,
		}

		gl.GenTextures(1, &g.texture.textureId)
		gl.BindTexture(gl.TEXTURE_2D, g.texture.textureId)

		var finalImg image.Image

		// Loading from file.
		if img.URI != "" {
			imgFile, err := os.Open(img.URI)
			if err != nil {
				return fmt.Errorf("unable to open texture file: %w", err)
			}

			img, _, err := image.Decode(imgFile)
			if err != nil {
				return fmt.Errorf("unable to decode texture file: %w", err)
			}

			finalImg = img
		}

		// Loading from buffer.
		if img.BufferView != nil {
			bufferView := g.document.BufferViews[*img.BufferView]
			buffer := g.document.Buffers[bufferView.Buffer]
			data := buffer.Data[bufferView.ByteOffset:bufferView.ByteOffset+bufferView.ByteLength]

			reader := bytes.NewReader(data)
			img, _, err := image.Decode(reader)
			if err != nil {
				return fmt.Errorf("unable to decode texture file: %w", err)
			}

			finalImg = img
		}

		if finalImg == nil {
			return errors.New("no supported texture source")
		}

		rgba := image.NewRGBA(finalImg.Bounds())
		if rgba.Stride != 4 * rgba.Rect.Size().X {
			return fmt.Errorf("unsupported stride")
		}

		draw.Draw(rgba, rgba.Bounds(), finalImg, image.Point{}, draw.Src)
		gl.TexImage2D(gl.TEXTURE_2D, 0, gl.RGBA, int32(rgba.Rect.Size().X), int32(rgba.Rect.Size().Y), 0, gl.RGBA, gl.UNSIGNED_BYTE, gl.Ptr(rgba.Pix))
	}

	gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR)
	gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR)
	gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE)
	gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE)

	// TODO
	if t.Sampler != nil {
		// sampler := g.document.Samplers[*t.Sampler]
	}

	detailed := kind == TextureAlbedo || kind == TextureMetallicRoughnessMap
	detailed = true // FIXME

	// Mipmapping.
	if detailed {
		gl.GenerateMipmap(gl.TEXTURE_2D)
		gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_LINEAR)
		// gl.TexParameterf(gl.TEXTURE_2D, gl.TEXTURE_LOD_BIAS, -0.4) // Incompatible with anisotropic filtering.
	}

	// Anisotropic filtering, if supported.
	if detailed && IsExtensionSupported("GL_EXT_texture_filter_anisotropic") {
		max := float32(0)
		gl.GetFloatv(gl.MAX_TEXTURE_MAX_ANISOTROPY, &max)
		gl.TexParameterf(gl.TEXTURE_2D, gl.TEXTURE_MAX_ANISOTROPY, float32(math.Min(4, float64(max))))
	}

	return nil
}

func (g *Gltf) loadShader() error {
	// FIXME: That's a lot of shader duplication per mesh.
	// FIXME: Hardcoded path.
	shader, err := LoadShader("shaders/vertex.glsl", "shaders/fragment.glsl")
	if err != nil {
		return fmt.Errorf("unable to load shader: %w", err)
	}

	g.mesh.shader = shader

	return nil
}

func (g *Gltf) initialModelTransform() {
	// Apply transforms, by finding it with the scenes and nodes.

	scene := g.document.Scenes[*g.document.Scene]
	rootNode := g.document.Nodes[scene.Nodes[0]]

	// Scale
	scale := rootNode.ScaleOrDefault()
	g.model.initialTransform = g.model.initialTransform.Mul4(mgl32.Scale3D(
		float32(scale[0]), float32(scale[1]), float32(scale[2]),
	))

	// Rotation
	rotation := rootNode.RotationOrDefault()
	v := mgl32.Vec4{
		float32(rotation[0]), float32(rotation[1]), float32(rotation[2]), float32(rotation[3]),
	}
	q := v.Quat()
	g.model.initialTransform = g.model.initialTransform.Mul4(q.Mat4())

	// TODO: Translation (not working?)
	translation := rootNode.TranslationOrDefault()
	g.model.initialTransform = g.model.initialTransform.Mul4(mgl32.Translate3D(
		float32(translation[0]), float32(translation[1]), float32(translation[2]),
	))
}

func (g *Gltf) loadTangents() error {
	// There aren't always present in the glTF file so we might as well just generate it all the time.

	// Actually, only really needed for models with a normal map.
	if g.mesh.normalMapTexture == nil {
		return nil
	}

	g.mesh.tangents = g.generateTangents(g.mesh.indices, g.mesh.vertices, g.mesh.uvs, g.mesh.normals)

	// Tangent/Bitangent.
	gl.GenBuffers(1, &g.mesh.tangentBufferId)
	gl.BindBuffer(gl.ARRAY_BUFFER, g.mesh.tangentBufferId)
	gl.BufferData(gl.ARRAY_BUFFER, len(g.mesh.tangents) * 4, gl.Ptr(g.mesh.tangents), gl.STATIC_DRAW)
	gl.EnableVertexAttribArray(ShaderAttributeTangents)
	gl.VertexAttribPointer(ShaderAttributeTangents, 3, gl.FLOAT, false, int32(3 * 4), gl.PtrOffset(0))

	return nil
}

func (g *Gltf) generateTangents(indices []uint16, vertices, uvs, normals []float32) []float32 {
	tangents := make([]float32, len(normals))
	bitangents := make([]float32, len(normals))

	for i := 0; i < len(indices); i += 3 {
		v1i := indices[i]
		v1x := vertices[v1i*3 + 0]
		v1y := vertices[v1i*3 + 1]
		v1z := vertices[v1i*3 + 2]
		v1u := uvs[v1i*2 + 0]
		v1v := uvs[v1i*2 + 1]
		v1 := mgl32.Vec3{v1x, v1y, v1z}
		vt1 := mgl32.Vec2{v1u, v1v}

		v2i := indices[i+1]
		v2x := vertices[v2i*3 + 0]
		v2y := vertices[v2i*3 + 1]
		v2z := vertices[v2i*3 + 2]
		v2u := uvs[v2i*2 + 0]
		v2v := uvs[v2i*2 + 1]
		v2 := mgl32.Vec3{v2x, v2y, v2z}
		vt2 := mgl32.Vec2{v2u, v2v}

		v3i := indices[i+2]
		v3x := vertices[v3i*3 + 0]
		v3y := vertices[v3i*3 + 1]
		v3z := vertices[v3i*3 + 2]
		v3u := uvs[v3i*2 + 0]
		v3v := uvs[v3i*2 + 1]
		v3 := mgl32.Vec3{v3x, v3y, v3z}
		vt3 := mgl32.Vec2{v3u, v3v}

		// Part 1
		deltaPos1 := v2.Sub(v1)
		deltaPos2 := v3.Sub(v1)
		deltaUV1 := vt2.Sub(vt1)
		deltaUV2 := vt3.Sub(vt1)

		f := 1.0 / (deltaUV1.X() * deltaUV2.Y() - deltaUV1.Y() * deltaUV2.X())

		tangent := mgl32.Vec3{
			(deltaPos1.X() * deltaUV2.Y() - deltaPos2.X() * deltaUV1.Y()) * f,
			(deltaPos1.Y() * deltaUV2.Y() - deltaPos2.Y() * deltaUV1.Y()) * f,
			(deltaPos1.Z() * deltaUV2.Y() - deltaPos2.Z() * deltaUV1.Y()) * f,
		}

		bitangent := mgl32.Vec3{
			(deltaPos1.X() * deltaUV2.X() - deltaPos2.X() * deltaUV1.X()) * f,
			(deltaPos1.Y() * deltaUV2.X() - deltaPos2.Y() * deltaUV1.X()) * f,
			(deltaPos1.Z() * deltaUV2.X() - deltaPos2.Z() * deltaUV1.X()) * f,
		}

		tangents[v1i*3 + 0] += tangent.X()
		tangents[v1i*3 + 1] += tangent.Y()
		tangents[v1i*3 + 2] += tangent.Z()
		tangents[v2i*3 + 0] += tangent.X()
		tangents[v2i*3 + 1] += tangent.Y()
		tangents[v2i*3 + 2] += tangent.Z()
		tangents[v3i*3 + 0] += tangent.X()
		tangents[v3i*3 + 1] += tangent.Y()
		tangents[v3i*3 + 2] += tangent.Z()

		bitangents[v1i*3 + 0] += bitangent.X()
		bitangents[v1i*3 + 1] += bitangent.Y()
		bitangents[v1i*3 + 2] += bitangent.Z()
		bitangents[v2i*3 + 0] += bitangent.X()
		bitangents[v2i*3 + 1] += bitangent.Y()
		bitangents[v2i*3 + 2] += bitangent.Z()
		bitangents[v3i*3 + 0] += bitangent.X()
		bitangents[v3i*3 + 1] += bitangent.Y()
		bitangents[v3i*3 + 2] += bitangent.Z()
	}

	// Part 2
	for i := 0; i < len(normals) / 3; i++ {
		n := mgl32.Vec3{
			normals[i*3+0],
			normals[i*3+1],
			normals[i*3+2],
		}

		t0 := mgl32.Vec3{
			tangents[i*3+0],
			tangents[i*3+1],
			tangents[i*3+2],
		}

		t := t0.Sub(n.Mul(n.Dot(t0)))
		t = t.Normalize()

		tangents[i*3+0] = t.X()
		tangents[i*3+1] = t.Y()
		tangents[i*3+2] = t.Z()
	}

	return tangents
}
