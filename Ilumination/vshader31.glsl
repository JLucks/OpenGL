#version 330 core

attribute vec3 vPosition;
attribute vec3 vColor;

out vec4 fragmentColor;
uniform mat4 MVP;

void main(){	

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vPosition,1);

	// The color of each vertex will be interpolated
	// to produce the color of each fragment
	fragmentColor = vColor;
}

