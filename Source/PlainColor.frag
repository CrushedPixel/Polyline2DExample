#version 150

// this shader simply colors everything in a single color,
// specified by the "color" uniform.

out vec4 fragColor;

uniform vec4 color;

void main() {
    fragColor = color;
}