# FeLiNa Engine

FeLiNa Engine is a 3D game engine currently developed by:

- Alex Campamar, GitHub account [Acaree](https://github.com/Acaree).
- Alfonso Sanchez, GitHub account [Siitoo](https://github.com/Siitoo).
- Project repository [Felina](https://github.com/Acaree/FeLiNa-Engine).
- [Website](https://acaree.github.io/FeLiNa-Engine/). 

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

- Scripts:
	- In assets folder, you can find a demo scene named "Assignment3.json". This scene contains a tank with a script to move it with i,j,k & l, rotate the turret with mouse motion in X-axis 
	  and shoot a ball (instance) with the mouse middle button.
	- Create an script inside assets folder. Refresh assets and double click to open the node graph window.
	- Right click: 	- In node graph, the add node option will be shown.
			- In a node, the delete option will be shown.
	- CTRL+V: Duplicate the selected node.
	- With left click you can create links between nodes (left anchors are for inputs and right anchors are for outputs. You can only link inputs to outputs). Left click over a node select-it.
	- To close the node graph window, right click outside the node graph window.  
	- In the website, you can find gifs and a video showing how to use it -> https://acaree.github.io/FeLiNa-Engine/		
	- Add all nodes there (Right click->add and select the node type). Only event nodes can react to scene, so functions will never activate if there's no an event node before.
	  A function node activated by other event node, can activate another function node. After putting all nodes, click compile button to save it.
	- After that, drag the script to a game object script component (if the game object don't have it, you can create one script component in inspector). Open the graph from inspector
	  and set all values and references.
	- Nodes need to be edited in assets folder. After editing it, remove the script from all gameobjects that contains it and drag it again. You need to set all references & values again.
	- Press play to execute scripts. If you open from inspector a game object graph, you can see the graph with the debug draw.
	

## Innovation

- Delete nodes (open it from assets in engine)
- Debug draw in node graph (open node graph in inspector while game mode is activated)
- Ctrl+V duplicate the selected node (graph must be opened from assets)
- Nodes have a checkbox active. If it's not marked, the node won't be executed.

## Warning
- If a mesh have some triangle of his faces incomplete (with less of 3 vertices) whole mesh will be discarded.
- In inspector components only our own format files (files from library) can be dragged. We don't take responsibility if another type of file is dragged.
- Follow the instructions above to use and manage scripts. Before editing an script in assets, all references in game objects component script must be removed and dragged again.
  If a script is modified, save again the scene with the new script. If it's not saved, the scene will be corrupted.

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