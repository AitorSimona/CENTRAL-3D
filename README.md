# CENTRAL 3D
- Version 0.3
- [Repository](https://github.com/AitorSimona/CENTRAL-3D)
- [Latest Release](https://github.com/AitorSimona/CENTRAL-3D/releases/tag/Assignment3)

- Initial steps into 3D game engine development, this engine was built during Barcelona - UPC - CITM - Design and Development of Videogames - Game Engines subject (university).


- By [AitorSimona](https://github.com/AitorSimona)

<img src="docs/Photo.jpg">

## CORE SUBSYSTEMS

## SHADER PIPELINE

INSERT two gifs and detailed explanation here.

## OVERVIEW

INSERT video here, focus on last assignment but show overall functionality.

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

## Innovation:

Done:
- Shader binary program loading
