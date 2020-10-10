#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTex;

uniform mat4 cam;

out vec2 texcoord;

void main(){
    gl_Position = cam * vec4(aPos, 1.0);
    texcoord = aTex;
}