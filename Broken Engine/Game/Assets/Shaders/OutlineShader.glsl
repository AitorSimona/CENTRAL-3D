#version 440 core 
#define VERTEX_SHADER 
#ifdef VERTEX_SHADER 
layout (location = 0) in vec3 a_Position; 
uniform mat4 u_Model; 
uniform mat4 u_View; 
uniform mat4 u_Proj; 
void main(){ 
gl_Position = u_Proj * u_View * u_Model * vec4(a_Position, 1.0f); 
}
#endif //VERTEX_SHADER

#define FRAGMENT_SHADER 
#ifdef FRAGMENT_SHADER 
out vec4 color; 
void main(){ 
color = vec4(1.0,0.65,0.0, 1.0); 
} 
#endif //FRAGMENT_SHADER 
