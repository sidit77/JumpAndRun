#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aCol;

uniform mat4 cam;

out vec4 color;

void main(){
    gl_Position = cam * vec4(aPos, 1.0);
    color = aCol;
}