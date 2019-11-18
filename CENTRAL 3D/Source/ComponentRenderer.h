#ifndef __COMPONENT_RENDERER_H__
#define __COMPONENT_RENDERER_H__

#include "Component.h"

// Specifying normal vectors length (used when drawing normals)
#define NORMAL_LENGTH 0.5

class ResourceMesh;
class ComponentMaterial;

class ComponentRenderer : public Component
{
public:

	ComponentRenderer(GameObject* ContainerGO);
	virtual ~ComponentRenderer();

	void Draw() const;

private:
	// --- Draw Functiions accessed by main Draw ---
	inline void DrawMesh(ResourceMesh& mesh, ComponentMaterial* mat) const;
	inline void DrawNormals(const ResourceMesh& mesh) const;
	inline void DrawAxis() const;
	inline void DrawBoundingBox()const;

public:
	bool draw_vertexnormals = false;
	bool draw_facenormals = false;
	bool checkers = false;
	
};

#endif