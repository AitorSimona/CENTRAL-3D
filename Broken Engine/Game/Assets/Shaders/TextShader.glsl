#version 440 core 
#define VERTEX_SHADER 
#ifdef VERTEX_SHADER 
layout (location = 0) in vec3 position; 
layout (location = 1) in vec2 texCoords; 
out vec2 TexCoords; 
uniform mat4 model_matrix; 
uniform mat4 view; 
uniform mat4 projection; 
void main(){ 
gl_Position = projection * view * model_matrix * vec4 (position, 1.0f); 
TexCoords = texCoords; 
}
#endif //VERTEX_SHADER

#define FRAGMENT_SHADER 
#ifdef FRAGMENT_SHADER 
in vec2 TexCoords; 
uniform sampler2D text; 
uniform vec3 textColor; 
out vec4 color; 
void main(){ 
vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r); 
color = vec4(textColor, 1.0) * sampled; 
} 
#endif //FRAGMENT_SHADER















































































































