# CENTRAL 3D
- Version 0.3
- [Repository](https://github.com/AitorSimona/CENTRAL-3D)
- [Latest Release](https://github.com/AitorSimona/CENTRAL-3D/releases/tag/Assignment3)

- Initial steps into 3D game engine development, this engine was built during Barcelona - UPC - CITM - Design and Development of Videogames - Game Engines subject (university).


- By [AitorSimona](https://github.com/AitorSimona)

<img src="docs/Photo.jpg">

## Core subsystems

The engine's pillars...

### Game Objects (ECS)

Based on an Entity-component system, each game object is a unique entity, and has a number of components attached to it.
The available component as of 0.3 are: Transform(fixed), Material, Renderer, Mesh, Camera. All inherit from Component base class.

Every game object has a parent, and a number n of childs. This turns into a tree structure (Unity).

### Resource Manager
In charge of the resources, which are: Material, texture, shader, mesh.

Handles reference counting, every resource is only in memory once and can be used by any number of game object components.

Resource management flow:

- A resource is dropped on the engine, or put in the assets folder (will need restart)
- Engine automatically imports it, generates meta files and own file format files, the latters stored in the library.
- When importing again, engine will detect a meta and load the resource from the library, in own file format, which is way faster.

The user does not have to handle any library file, everything is done by the engine. For a user all there is are the files he/she dropped in assets, in the original format provided.

### Gui

Courtesy of Dear Imgui, the gui has been worked on to be as modular and scalable as possible. All panels inherit from a base panel class and are handled by the GUI module. Adding/modifying panels is very straightforward. 

## Shader Subsystem

<img src="docs/WaveShader.gif">

For the third assignment, the original OpenGL rendering pipeline was replaced by the shader pipeline. Also introducing the scene in a framebuffer meant that a lot of systems had to be adjusted. All rendering had to use shaders.

There is a Shader class, which is a resource and works the same way as other resources. 

### Shader management
There are multiple ways to create a shader program:

1. Write a vertex and fragment shaders, store them in separate files with the .vertex and .fragment extension. Engine will try to import them.

2. Create a program through the editor:

Steps:

- Press new 
- Give it a cool name
- Fill the code
- Coompile 
- Save 

You have just created your own shader! 

### Uniform editing
You can edit all int, float, and vec up to vec4 uniforms through the inspector! Head to the material node, select a shader and edit the uniforms there. Changes are applied automatically. Note that these uniforms are held by the material, so all objects with the same material will share the uniforms. 

<img src="docs/UniformEditing.gif">

### Binary program
Remember to save your shader through the SAVE ALL button in the shader Editor. The engine will save the program in binary format. Once booting the engine again it will try to load the binary, if unsuccessful it will recompile the shaders. 

## Overview

<video src="docs/ShadersOverview.mp4" width="600" height="500" controls preload></video>

[Background Music](https://www.youtube.com/watch?v=E8HY7ypKshA) 

## Other Features
Time Management
Octree and Frustum Culling
Scene Serialization and de-Serialization
Importers (Scene, Mesh, Texture(module), Material and Shader)
Mouse Picking, Guizmo
Scene is rendered to a framebuffer and displayed as texture in panelScene
FileSystem through PhysFS (also using stl's fstream)

## How to use

There is no formal installation process, just look around and play with it

- Navigation:

Mouse and Keyboard controls are enabled both on UI and Scene, working on focused window only.

- Mouse picking

Self-explainable, click on an object to select it, notice how inspector info changes, but if the object is hidden
in the hierarchy you won't notice it there, open parent to see how it is selected.

- Scene Camera:

- Right Mouse button enables Look Around
- Alt + Left Mouse button enables Camera Orbit
- Mouse Wheel click enables camera Pan
- Right Mouse button + WASD enables free movement
- Mouse Wheel movement enables zoom

Editor camera only works when in AppState EDITOR mode.

- Guizmo:

- Move: Q
- Rotate: E
- Scale: R

## Innovation

- Shader binary program loading
- Shader debug draw (normals, aabbs, octree, cameras)
- Z-buffer visualizer
- reversed-z technique

## Tools used

- Microsoft Visual Studio 2017
- 3rd Party Libraries used:
- SDL 2.0.10
- Glew 2.1.0
- ImGui 1.72b (with beta docking)
- ImGuizmo
- MathGeoLib 1.5 (commit 1994)
- OpenGL 4.6
- mmgr
- par_shapes at commit 281 
- DevIL 1.8.0
- JSON for Modern C++
- PhysFS 
