#version 330 core

attribute vec4 vPosition;
attribute vec4 vColor;

out vec4 fragmentColor;
uniform mat4 MVP;

void main(){	

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vPosition;

	// The color of each vertex will be interpolated
	// to produce the color of each fragment
	fragmentColor = vColor;
}

