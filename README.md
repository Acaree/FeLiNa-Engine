# FeLiNa Engine

FeLiNa Engine is a 3D game engine currently developed by:

- Alex Campamar, GitHub account [Acaree](https://github.com/Acaree).
- Alfonso Sanchez, GitHub account [Siitoo](https://github.com/Siitoo).
- Project repository [Felina](https://github.com/Acaree/FeLiNa-Engine).


This engine have been developed in the design & development grade of CITM (Terrasa).

The objective is to develop a engine to use it in the next game we'll create. 

The code is written in c++ 

Link to repository: [FeLiNa Engine](https://github.com/Acaree/FeLiNa-Engine).

## Tool used

-IDE: Mircrosoft Visual Studio 2017.
-SDL 2.0.4 
-OpenGl 3.1.0
-Parson 0.94 
-Pcg 0.94
-Devil 1.7.8
-Glew 2.3.4
-Assimp 3.3.0
-ImGui 1.65
-MathGeoLib 1.5
-Imguizmo 1.0
-MMGR
-Physfs 2.1

## Controls

- Mouse Hold Right:
	- Look around
	- WASD: move forward, left, backward and right
-Mouse Left click:
	- Pick a game object from scene, show his guizmos and his inspector
- E/R: move up/down
- Hold Alt:
	- Mouse Hold Left: look around target
	- Mouse Wheel: zoom in/out
	- Mouse Hold Shift: movement speed x2, zoom speed x2

- H: Enable/Disable wireframe mode
- Shift + M : Open/Close Configuration windows
- Shift + I : Open/Close Inspector windows
- Shift + C : Open/Close Console
- Drag a file into engine: Copy the file in assets folder


-In assets window:
	- Double click on fbx: Add a fbx to the scene
	- Refresh Assets button: If a new file is in assets, generate all the needed files.

-In Hierarchy:
	-Left click: Focus a game object and show his properties in inspector.
	-Right click on gameobject: Delete option is showed. Delete game object on left click.
	-Right click not over a game object: Create new empty object option is showed. Create game object on left click

-In inspector: 
	-Add component: Add a new component to the selected game object.
	-Inside a empty component, there is a place to drag a new material/mesh. The file dragged must be one of library (in assets window from the engine), others will be despreciated. 		 

## Innovation

- Physfs added to read files and acces to folders

## Warning
- If a mesh have some triangle of his faces incomplete (with less of 3 vertices) whole mesh will be discarded.
- In inspector components only our own format files (files from library) can be dragged. We don't take responsibility if another type of file is dragged.

## License

Copyright 2018 MIT Alex Campamar & Alfonso Sánchez-Cortés
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files 
FeLiNa, to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, 
publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE 
OR OTHER DEALINGS IN THE SOFTWARE.