# Broken Engine by Broken Gem Studio
Broken Engine is a 3D Game Engine developed with C++ and based on OpenGL and developed by students of the Degree in Videogames Design and Development of Universitat Politecnica de Catalunya for the Project III subject of 3rd course.

We used as basis for the engine (fork), the Aitor Simona's [Central 3D Engine](https://github.com/AitorSimona/CENTRAL-3D).

## TOOLS & LIBRARIES USED

We used [SDL 2.0.10](https://www.libsdl.org/), STL, [OpenGL 4.4](https://www.opengl.org/) with [GLAD 0.1.33](https://glad.dav1d.de/), [ImGui 1.72b](https://github.com/ocornut/imgui), [GPUDetect](https://github.com/GameTechDev/gpudetect), [MMRG](https://www.flipcode.com/archives/Presenting_A_Memory_Manager.shtml), [ParShapes](https://prideout.net/shapes), [Assimp 3.3](http://www.assimp.org/), [Devil 1.8.0](http://openil.sourceforge.net/), [JSON for Modern C++](https://nlohmann.github.io/json/), [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo).

In Addition:

- Microsoft Visual Studio 2019 
- Visual Studio Code
- MathGeoLib 1.5 (commit 1994)
- PhysFS
- PhysX 3.4.2 
- Recast Navigation


## HOW TO USE
There is no formal installation process, just look around and play with it.

* Navigation: Mouse and Keyboard controls are enabled both on UI and Scene, working on focused window only.
* Mouse picking: Self-explainable, click on an object to select it, notice how inspector info changes, but if the object is hidden
in the hierarchy you won't notice it there, open parent to see how it is selected.
* Asset Import: Drop it into the editor.
* Scene Camera:
- Right Mouse button enables Look Around
- Alt + Left Mouse button enables Camera Orbit
- Mouse Wheel click enables camera Pan
- Right Mouse button + WASD enables free movement
- Mouse Wheel movement enables zoom

Editor camera only works when in AppState EDITOR mode.

* Guizmo: Move (W), Rotate (E) and Scale (R), or from EditorUI (while editor camera is moving, guizmo is frozen)

**Note:** Do not modify config file, it will modify engine's configuration. And please, don't delete the dll or exe files, they are needed for the engine to work. In fact, do not touch any engine's inner files in Library or Settings, please.

## DEBUG FEATURES
Game Objects - Redo Octree whenever you make some elements non-static and want it to be recalculated
Head to Scene window and click DebugDraw to see available features: 

- Wireframe
- Color Material
- OpenGL Lighting
- Display Octree
- Show AABBS
- ZDrawer

There are more options in Window -> Settings

## CHANGELOG


## Licenses:

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


PhysX License

Copyright (c) 2018-2019 NVIDIA Corporation. All rights reserved. Redistribution and 
use in source and binary forms, with or without modification, are permitted provided 
that the following conditions are met:

- Redistributions of source code must retain the above copyright notice, this list 
of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice, this list
of conditions and the following disclaimer in the documentation and/or other materials 
provided with the distribution.
- Neither the name of NVIDIA CORPORATION nor the names of its contributors may be used 
to endorse or promote products derived from this software without specific prior written 
permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY EXPRESS OR IMPLIED 
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND 
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.

Recast Navigation License

Copyright (c) 2009 Mikko Mononen memon@inside.org

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

