#version 330 core

in vec3 vFragColor;
out vec3 fFragColor;

void main() {
  float gris = (vFragColor.r + vFragColor.g + vFragColor.b)/3;
  fFragColor = vec3(gris);
};
