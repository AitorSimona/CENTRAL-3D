#version 440 core 
#define VERTEX_SHADER 
#ifdef VERTEX_SHADER 
layout (location = 0) in vec3 position; 
out vec3 ourColor; 
uniform vec3 Color; 
uniform mat4 model_matrix; 
uniform mat4 view; 
uniform mat4 projection; 
void main(){ 
gl_Position = projection * view * model_matrix * vec4(position, 1.0f); 
ourColor = Color; 
}
#endif //VERTEX_SHADER

#define FRAGMENT_SHADER 
#ifdef FRAGMENT_SHADER 
in vec3 ourColor; 
out vec4 color; 
void main(){ 
color = vec4(ourColor, 1.0); 
} 
#endif //FRAGMENT_SHADER






























































































