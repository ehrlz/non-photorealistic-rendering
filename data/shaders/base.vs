#version 330 core
layout(location = 0) in vec3 pos; 
layout(location = 1) in vec2 uv;
out vec2 UV;
uniform mat4 M; 
uniform mat4 PV; 
void main(){
  gl_Position = PV * M * vec4(pos,1); // Construyo coord homog�neas y aplico matriz transformacion M
  UV=uv;                             // Paso color a fragment shader
 }