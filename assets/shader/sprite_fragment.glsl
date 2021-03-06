#version 450 core

out vec4 FragColor;

layout(binding = 0) uniform sampler2D tex;

in vec2 texcoord;

void main(){
    FragColor = texture(tex, texcoord);
    if(FragColor.a == 0)
        discard;
}