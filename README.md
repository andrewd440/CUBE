<h1>A Component-Based Voxel Game Engine</h1>
<h2>Design Overview</h2>
The main goal for this engine is to give users the ability to create voxel based games such as <a href="https://minecraft.net/">Minecraft</a>, <a href="https://picroma.com/cubeworld">Cube World</a>, <a href="https://www.castlestory.net/">Castle Story</a> or <a href="http://www.vox-game.com/">Vox</a>. Major design goals for the engine include the ability to render a large number of dynamic voxels on screen, the ability to randomly generate infinite worlds, and allow users to implement unique gameplay elements.
<h2>Feature Overview</h2>
<ul>
	<li>Custom Math Library w/ SIMD Acceleration</li>
	<li>Custom Memory Management</li>
	<li>Game Asset Management</li>
	<li>In-Game Debugging Tools</li>
	<li>SFML Integration for Window and Input Management</li>
	<li>FMOD Integration for 3D Audio</li>
	<li>Custom 3D Deferred-Rendering Engine with OpenGL</li>
	<li>Bullet Physics Integration</li>
	<li>Game Object/Entity Component System Design</li>
</ul>
<h2></h2>
<h2>Custom Math Library with SIMD Acceleration</h2>
The entire math library will be custom built. This library will include operations that are fundamental for game development such as matrix, vector, and quaternion manipulations, as well as a random number generator (<a href="http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/SFMT/">SFMT</a>) and a game object transform class.
<h2>Custom Memory Management</h2>
To improve the performance of memory based operations and memory access patterns, custom memory allocators will be used throughout the engine subsystems. These allocators will include a stack based allocator and a pool based allocator.
<h2>Game Asset Management</h2>
Facilities will be provided for users to easily load and use game asset files efficiently, including textures, audio files, etc.
<h2>In-Game Debugging Tools</h2>
Two primary debugging tools will be provided to users, these include console logging and debug drawing. For debug drawing, users will be able to draw things such as lines and boxes within the game world.
<h2>SFML Integration for Window and Input Management</h2>
To manage windowing and input, the open source library <a href="http://www.sfml-dev.org/">SFML</a>’s (Simple and Fast Multimedia Library) “Window module” will be integrated within the engine.
<h2>FMOD Integration for 3D Audio</h2>
Audio management within the engine will use the <a href="http://www.fmod.org/">FMOD</a> library, which is free for non-commercial/educational use. A basic wrapper will be provided for this library for simple audio operations. Users will also have access to the low level API of the FMOD library for more advanced use.
<h2>Custom 3D Deferred-Rendering Engine with OpenGL</h2>
The 3D rendering engine will be built with OpenGL. This engine will be capable of rendering a large amount of voxels on screen, feature a deferred rendering pipeline and ambient occlusion. If time allows, more advances rendering techniques such as real-time shadows, cascading shadow maps, anti-aliasing, and more will be added.
<h2>Game Object/Entity Component System Design</h2>
Game objects within the engine will incorporate a component based design, similar to the way Unity treats game objects. Game objects can be assigned any number of components that make up the complete object. To provide custom behavior to objects, users will be able to create custom components to control game objects. For this implementation, I will be using a system that I previously built, <a href="https://github.com/andrewd440/Atlas-ECS">Atlas-ECS</a>. Although the system is currently usable, I will revamp a large amount of the code base to allow what I need within this engine.
