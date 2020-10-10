#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aCol;
layout(location = 2) in vec2 aTex;

uniform mat4 cam;

uniform float scale;
uniform vec2 pos;

out vec4 color;
out vec2 texcoord;

void main(){
    gl_Position = cam * vec4(aPos.xy * scale + pos, aPos.z, 1.0);
    color = aCol;
    texcoord = aTex;
}