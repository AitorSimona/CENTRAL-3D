#ifndef __COMPONENT_RENDERER_H__
#define __COMPONENT_RENDERER_H__

#include "Component.h"

// Specifying normal vectors length (used when drawing normals)
#define NORMAL_LENGTH 1

class ComponentMesh;

class ComponentRenderer : public Component
{
public:

	ComponentRenderer(GameObject* ContainerGO);
	virtual ~ComponentRenderer();

	void Draw();

private:
	// --- Draw Functiions accessed by main Draw ---
	void DrawMesh(ComponentMesh& mesh) const;
	void DrawNormals(const ComponentMesh& mesh) const;
	void DrawAxis() const;
	
};

#endif