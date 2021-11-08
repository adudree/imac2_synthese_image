#version 330 core

in vec3 vPosition_vs;  // Pos. du sommet transformé dans l'espace View
in vec3 vNormal_vs;    // Norma. du sommet transformé dans l'espace View
in vec2 vTexCoords;    // Coordonnées de texture du sommet

out vec3 fFragColor;

uniform sampler2D moonTexture;

void main() {
  fFragColor = vec3(texture(moonTexture, vTexCoords).xyz);
  //fFragColor = normalize(vNormal_vs);
};