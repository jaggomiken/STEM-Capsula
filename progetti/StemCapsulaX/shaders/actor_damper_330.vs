#version 330

in vec3 vertexPosition;
in vec4 vertexColor;

uniform mat4 mvp;
uniform mat4 partTransform;

out vec4 fragColor;

void main()
{
  fragColor   = vertexColor;
  gl_Position = mvp * partTransform * vec4(vertexPosition, 1.0);
}
