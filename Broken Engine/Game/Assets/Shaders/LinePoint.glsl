#version 440 core 
#define VERTEX_SHADER
#ifdef VERTEX_SHADER
layout (location = 0) in vec3 a_Position;
out vec3 v_Color;
uniform vec3 u_Color;
uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Proj;
void main(){
gl_Position = u_Proj * u_View * u_Model * vec4(a_Position, 1.0f);
v_Color = u_Color;
}
#endif //VERTEX_SHADER

#define FRAGMENT_SHADER
#ifdef FRAGMENT_SHADER
in vec3 v_Color;
out vec4 color;

void main()
{
	color = vec4(v_Color, 1.0);
}

#endif //FRAGMENT_SHADER
