#version 450 core

layout(location = 0) in vec2 aPos;

uniform mat4 cam;

void main(){
    gl_Position = cam * vec4(aPos, 0.0, 1.0);
}