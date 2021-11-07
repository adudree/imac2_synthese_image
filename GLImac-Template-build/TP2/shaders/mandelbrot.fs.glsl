#version 330 core

in vec3 vFragColor;
in vec2 vFragPosition;

out vec3 fFragColor;

vec2 complexSqr(vec2 z) {
  float a = z.x;
  float b = z.y;

  z.x = a*a - b*b;
  z.y = 2*a*b;
  return z;
}

vec3 mandelbrot(vec2 vect, int n) {
  vec2 z = vect;
  
  for (int i =0; i < n; i++)
  {
    z = complexSqr(z) + vect;
    if (length(z) > 2.0)
    {
      return vec3(float(i)/float(n), 0., float(i)/float(n));
    }
  }
  return vec3(0., 0., 0.);
}


void main() {
  // fFragColor = vFragColor;
  fFragColor = mandelbrot(vFragPosition, 100);
};