// minimal.frag
#version 150 core

// precision highp float;

in vec3 ex_Color;
out vec4 out_Color;

void main(void) {
    out_Color = vec4(ex_Color, 1.0);
}


