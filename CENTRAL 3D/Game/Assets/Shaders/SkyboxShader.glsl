#version 460 core 
#define VERTEX_SHADER 
#ifdef VERTEX_SHADER 
layout (location = 0) in vec3 position; 
out vec3 TexCoords; 
uniform mat4 view; 
uniform mat4 projection; 
void main(){ 
TexCoords = position; 
vec4 pos = projection * view * vec4(position, 1.0f); 
gl_Position = pos.xyww; 
}
#endif //VERTEX_SHADER

#define FRAGMENT_SHADER 
#ifdef FRAGMENT_SHADER 
in vec3 TexCoords; 
uniform samplerCube skybox;
out vec4 color; 
void main(){ 
color = texture(skybox, TexCoords); 
} 
#endif //FRAGMENT_SHADER 
