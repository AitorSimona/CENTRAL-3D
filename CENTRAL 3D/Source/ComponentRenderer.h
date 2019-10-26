#ifndef __COMPONENT_RENDERER_H__
#define __COMPONENT_RENDERER_H__

#include "Component.h"

// Specifying normal vectors length (used when drawing normals)
#define NORMAL_LENGTH 0.5

class ComponentMesh;

class ComponentRenderer : public Component
{
public:

	ComponentRenderer(GameObject* ContainerGO);
	virtual ~ComponentRenderer();

	void Draw() const;

private:
	// --- Draw Functiions accessed by main Draw ---
	inline void DrawMesh(ComponentMesh& mesh) const;
	inline void DrawNormals(const ComponentMesh& mesh) const;
	inline void DrawAxis() const;

public:
	bool draw_vertexnormals = false;
	bool draw_facenormals = false;
	bool checkers = false;
	
};

#endif