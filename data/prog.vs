#version 330 core
layout(location = 0) in vec3 pos; 
layout(location = 1) in vec3 color;
out vec3 col;
uniform mat4 MVP=mat4(1.0f);
void main()
 	{
  gl_Position = MVP*vec4(pos,1); 
  col = color;  
}
