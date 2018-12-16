# Salty the Slime

### Version of the game
Salty the Slime v.0.0.2

### Description
Platformer game made for Development subject with SDL2. 
Be a salty slime jumping around the levels.
Grow up, divide and be the best.

### Controls
	Game Controls:
	
	A/D: Horizontal movement.

	Space bar: Jump

	Hold Spacebar: Charged jump.
	
	When in God Mode:
	
	W/S: Vertical movement.
	
	A/D: Horizontal movement.
	
	Q: Divide the slime
	
	E: Reagroup the slimes
	
	Debug Keys:
	
	F1: Load first scene.

	F2: Reload current scene.

	F3: Load next scene.

	F5: Save.

	F6: Load.
	
	F8: Debug UI.

	F9: Colliders/Pathfinding Draw.

	F10: God Mode.
	
	F11: Cap/Uncap FPS

	+/-: Increase/Decrease music volume.
	
	SHIFT+(+/-): Increase/Decrease fx volume.
	
	You need to acquire specific score to pass the levels.

### Instructions to execute

1.- Download the last release on

2.- Unzip the file

3.- Execute SaltytheSlime.exe

### Authors
Axel Alavedra Cabello

![Axel Alavedra Screenshot](https://raw.githubusercontent.com/AlmaCeax/ProjectI-AlmaCeax/master/Wiki/0.Home/axel.jpg)

[Github account](https://github.com/AxelAlavedra)

	Player Movement, Pathfinding, Enemies, Entity Structure, GUI.

Alejandro París Gómez

![Alejandro París Screenshot](https://raw.githubusercontent.com/AlmaCeax/ProjectI-AlmaCeax/master/Wiki/0.Home/alejandro.jpg)

[Github account](https://github.com/AlejandroParis)

	Infinite background, FPS Cap, Brofiler, DT implementation, Entity Structure, Player Divide and Grow, GUI.

### Repository link

https://github.com/TheSaltMine/Development_Assignment

### Innovations

Implemented a method to only render the tiles inside the camera.
Implemented infinite background scrolling, two backgrounds move one right before the other, 
when one exits the camera it's repositioned behind the other, creating the effect of infinite parallax.
Charged jump with vertical or diagional force.

### Last Innovations

The player can grow up eating enemies and can divide itself.
The scale of the player affects his movility.
You can only pass the levels by reaching a minimum score eating enemies.
Escalable and dynamic UI.
Loading screen with animated image.

### Video

<video src="Website/Salty_the_Slime_Gameplay_video.mp4" width="640" height="400" controls preload></video>

## Tools used
* IDE: Microsoft Visual Studio 2018
* Language: C++
* Graphics and audio: SDL 2.0
* Data: pugixml 
* Level editor: Tiled 
* Profiler: Brofiler
* Code repository: GitHub
* Others: Adobe Photoshop CS6 and SpriteCow


### Credits

We do not own any of the sprites, tilesets, music or fx used in this game. Authors credited below

Sprites: 
	
Slime by [Yanfly](http://yanfly.moe/)
Downloaded from [here](http://yanfly.moe/wp-content/uploads/2015/12/Slime1_1.png)

All UI Sprites [Game Art 2D](https://www.gameart2d.com/)
Downloaded from [here](https://www.gameart2d.com/free-fantasy-game-gui.html)

Tilesets: 

FREE PLATFORMER GAME TILESET by [Zuhria Alfitra](https://www.gameart2d.com/)

Downloaded from [here](https://www.gameart2d.com/free-platformer-game-tileset.html)
	
FREE DESERT PLATFORMER TILESET by [Zuhria Alfitra](https://www.gameart2d.com/)

Downloaded from [here](https://www.gameart2d.com/free-desert-platformer-tileset.html)

Music: 

Main Menu Song (Zoo Tycoon 1 Theme)

Forest Song by [FoolBoyMedia](https://freesound.org/people/FoolBoyMedia/)

Downloaded from [here](https://freesound.org/people/FoolBoyMedia/sounds/219017/)

Desert Song by [Bird_man](https://freesound.org/people/Bird_man/)

Downloaded from [here](https://freesound.org/people/Bird_man/sounds/401795/)

SFX: 

Jump (Retro video game SFX) by [cabled_mess](https://freesound.org/people/cabled_mess/)

Downloaded from [here](https://freesound.org/people/cabled_mess/sounds/350900/)

### License

MIT License

Copyright (c) 2018 [Axel Alavedra Cabello, Alejandro París Gómez]

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
