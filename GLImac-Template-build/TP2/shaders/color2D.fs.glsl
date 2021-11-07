#version 330 core

in vec3 vFragColor;
in vec2 vFragPosition;

out vec3 fFragColor;

void main() {
  fFragColor = 2 * exp(-20 * distance(vec2(0, 0),  vFragPosition) * distance(vec2(0, 0),  vFragPosition)) * vFragColor;
};