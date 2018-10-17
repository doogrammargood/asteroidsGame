#version 150

in vec4 vPos;
out vec4 color;
uniform vec4 vColor;
uniform mat4 M;


void
main()
{
  gl_Position = M*vPos;
  color = vColor;
}
