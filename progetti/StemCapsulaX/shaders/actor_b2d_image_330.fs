#version 330

// Input vertex attributes (from vertex shader)
in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform vec3 viewPos;
uniform float time;

// Output fragment color
out vec4 finalColor;

void main()
{
  vec4 tint  = colDiffuse*fragColor;
  finalColor = tint;
}
