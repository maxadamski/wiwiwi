#pragma once

/*

A very simple cube model for testing indexed 3D graphics

  7 + + + + + + + + + 6
  +\                  +\
  + \                 + \
  +  \                +  \
  +   \               +   \
  +    3 + + + + + + +++ + 2
  +    +              +    +
  +    +              +    +
  +    +              +    +
  +    +              +    +
  4 + +++ + + + + + + 5    +
   \   +               \   +
    \  +                \  +
     \ +                 \ +
      \+                  \+
       0 + + + + + + + + + 1

*/
  

namespace Cube {
	typedef unsigned int u32;
	typedef float f32;

	const u32 vertexCount = 8;
	const f32 vertex[vertexCount*4] = {
		-1, -1,  1, 1, // 0
		 1, -1,  1, 1, // 1
		 1,  1,  1, 1, // 2
		-1,  1,  1, 1, // 3
		-1, -1, -1, 1, // 4
		 1, -1, -1, 1, // 5
		 1,  1, -1, 1, // 6
		-1,  1, -1, 1, // 7
	};
	const f32 color[vertexCount*4] = {
		1, 0, 0, 1, // red
		1, 0, 1, 1, // magenta
		1, 1, 1, 1, // white
		1, 1, 0, 1, // yellow
		0, 0, 0, 1, // black
		0, 0, 1, 1, // blue
		0, 1, 1, 1, // cyan
		0, 1, 0, 1, // green
	};

	const u32 indexCount = 3*2*8;
	const u32 index[indexCount] = {
		0,1,2, 2,3,0, // front
		1,5,6, 6,2,1, // right
		5,4,7, 7,6,5, // back
		4,0,3, 3,7,4, // left
		3,2,6, 6,7,3, // top
		4,5,0, 1,0,5, // bottom
	};
}

