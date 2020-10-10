#version 450 core

layout(location = 0) in vec3 aPos;

uniform mat4 cam;

void main(){
    gl_Position = cam * vec4(aPos, 1.0);
}