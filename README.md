# CENTRAL 3D
- Version 0.3
- Initial steps into 3D game engine development
- By Aitor Simona 
* GitHub account: [AitorSimona](https://github.com/AitorSimona)
* GitHub repository: [Repository](https://github.com/AitorSimona/CENTRAL-3D)
* GitHub website: [Website](https://aitorsimona.github.io/CENTRAL-3D/)

## TOOLS USED

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


## HOW TO USE

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

- Shaders:

You can create new shaders by putting into assets folder .vertex and .fragment files, so the engine imports them 
at startup. 

You can also create shaders on the ShaderEditor panel, next to inspector (check tabs). 

Steps:

- Press new 
- Give it a cool name
- Fill the code
- Coompile 
- Save 

You have just created your own shader! 


## ASSET IMPORT

Drop it inside window 2 times, first for importing, second for loading, you can also drag it from project window 
to hierarchy (on the window title).

In this specific release only shaders are imported at startup, default scene is loaded.

## DEBUG

- Debug Features

Game Objects - Redo Octree whenever you make some elements non-static and want it to be recalculated

Head to Scene window and click DebugDraw to see available features: 

- Wireframe
- Color Material
- OpenGL Lighting
- Display Octree
- Show AABBS

There are more options in Window - > Settings

## Features in beta

- Texture pre-visualization on Inspector
- Edit Menu
- Load/Save cameras
- Resource manager (initial implementation)

## CHANGELOG

### 0.3

- Shader pipeline to draw geometry and debug draw
- Shader resource (save & load...)
- Shader binary program loading
- Shader Debug draw (normals, aabbs, octree, cameras)
- Water Shader
- Editor shader support (creation, realtime changes, uniform editing)
- Guizmo
- Framebuffer, scene in window, code adaptation
- Time manager improvements

### 0.2

- Initial Structure of Resource Manager
- Reference Counting for ResourceMesh and ResourceTexture
- Scene serialization to file and loading
- New import pipeline (creates own file format files in library)
- New Time Manager (Options to PLAY STOP STEP)
- Spatial partitioning using Octree
- Creation of cameras
- Frustum culling
- AABB's
- Project window
- Mouse picking 

### 0.1

- Polished Game Object and Component Structure
- Initial Implementation on Inspector (properties for the different components)
- Modification of many Menus/Windows
- Possibility of primitives creation through GameObject Menu
- Unity-like Camera Controls 

### 0.05

- Libraries Integration
- Basic Editor with Inspector/Hierarchy/Console/Menu
- Initial GameObject and Component structure
- Save & Load Editor Configuration
- Load 3D Models through assimp (fbx format)
- Load Textures through DevIL (dds and png format)
- Docking

## Innovation:

Done:
- Shader binary program loading
- Shader debug draw (normals, aabbs, octree, cameras)

Scheduled:
- Z-buffer visualizer
- shader normal draw
- reversed-z technique

## License:

MIT License

Copyright (c) 2019 Aitor Simona Bouzas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.


