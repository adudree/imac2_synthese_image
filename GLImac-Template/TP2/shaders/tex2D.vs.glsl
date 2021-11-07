#version 330 core

layout(location = 0) in vec2 aVertexPosition;
layout(location = 1) in vec2 aVertexColor;

out vec2 vFragColor;
out vec2 vFragPosition; 


uniform mat3 uModelMatrix;


void main() {
  vFragColor = aVertexColor;
  vFragPosition = aVertexPosition;

  vec2 transformed = (uModelMatrix * vec3(aVertexPosition, 1)).xy;

  gl_Position = vec4(transformed, 0, 1);
};
