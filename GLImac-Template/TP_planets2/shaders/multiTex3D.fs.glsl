#version 330 core

in vec3 vPosition_vs;  // Pos. du sommet transformé dans l'espace View
in vec3 vNormal_vs;    // Norma. du sommet transformé dans l'espace View
in vec2 vTexCoords;    // Coordonnées de texture du sommet

out vec3 fFragColor;

uniform sampler2D uEarthTexture;
uniform sampler2D uCloudTexture; 

void main() {
  vec3 couleur1 = vec3(texture(uEarthTexture, vTexCoords).xyz);
  vec3 couleur2 = vec3(texture(uCloudTexture, vTexCoords).xyz);
  vec3 couleur = couleur1 + couleur2;
  
  fFragColor = couleur.xyz;
};